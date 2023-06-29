/**
 * @file MaquinaEstados.ino
 * @brief Descripción breve del archivo
 *
 * Descripción más detallada del archivo, incluyendo su propósito y cualquier información relevante.
 *
 * @author Javier Rojas Agredo, Diana Carolina Bravo, Daniel Alejandro Garcia.
 * @date 29/06/2023
 *
 *
 * @par Información de contacto:
 *
 * Correo electrónico: javierra@unicauca.edu.co
 */
#include "StateMachineLib.h"
#include "DHTStable.h"
#include "AsyncTaskLib.h"
#include <LiquidCrystal.h>
#include <Keypad.h>
#include "pinOut.h"

void timeout1();
void timeout2();
void timeout3();
void timeout4();
void medirTemp();
void readLight();
void leerPass();

void sensorHall();
void sensorTracking();
void sensorMetal();

#define DEBUG(a)          \
  Serial.print(millis()); \
  Serial.print(": ");     \
  Serial.println(a);

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
DHTStable DHT;

#pragma region TareasAsinc
AsyncTask asyncTaskTime_1(2000, timeout1);
AsyncTask asyncTaskTime_2(10000, timeout2);
AsyncTask asyncTaskTime_3(6000, timeout3);
AsyncTask asyncTaskTime_4(5000, timeout4);
AsyncTask asyncTaskTemp(500, true, medirTemp);
#pragma endregion TareasAsinc

char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', '+'},
    {'4', '5', '6', '-'},
    {'7', '8', '9', '*'},
    {'.', '0', '=', '/'}};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

int fre; /**< Variable para almacenar el valor de frecuencia */
int execute = 0; /**< Variable para indicar si se debe ejecutar una tarea */

unsigned long lastReadLightTime = 0; /**< Tiempo de la última lectura del sensor de luz */
const unsigned long readLightInterval = 1000; /**< Intervalo de tiempo para leer el sensor de luz */

float tempOnState = 0; /**< Temperatura actual en el estado actual de la máquina de estados */
char inputPassword[5]; /**< Contraseña ingresada por el usuario */
char password[5] = "12345"; /**< Contraseña predeterminada */
unsigned char i = 0; /**< Índice para la contraseña ingresada */
int failCount = 0; /**< Contador de intentos fallidos de ingreso */
bool bandera = false; /**< Bandera para indicar si se debe ingresar la contraseña */
bool buzzer = false; /**< Bandera para indicar si debe sonar el zumbador */
int outputValue = 0; /**< Valor de salida actual */



#pragma region enums
enum State
{
  STATE_SEGURIDAD = 0,
  STATE_MONITOR = 1,
  STATE_PUERTA_VENTANA = 2,
  STATE_ALERTA_SEGURIDAD = 3,
  STATE_ALARMA = 4
};

enum Input
{
  passwordCorrect = 0,
  timeOut = 1,
  gateOpen = 2,
  tempVeriffy = 3,
  tempAndTime = 4,
  Unknown = 5,
};

#pragma endregion enums

StateMachine stateMachine(5, 8);

Input input;

/**
 * @brief Función para ingresar la clave
 *
 * Esta función se llama cuando se ingresa al estado de ingreso de clave en una máquina de estados.
 *
 * @param void
 * @return void
 */
void ingresar()
{
  i = 0;
  bandera = false;

  lcd.clear();
  lcd.print("Clave:");
  lcd.setCursor(0, 1);
  lcd.print(">");
  lcd.setCursor(0, 2);
}

/**
 * @brief Función para manejar eventos
 *
 * Esta función se llama cuando se ingresa al estado de eventos en una máquina de estados.
 *
 * @param void
 * @return void
 */
void eventos()
{
  buzzer = false;
  lcd.clear();
  asyncTaskTime_1.Start(); // timeout 2 sec
  asyncTaskTemp.Stop();    // Detener escaneo de temperatura
  // lcd.print("En eventos");
}

/**
 * @brief Función para monitorear el entorno
 *
 * Esta función se llama cuando se ingresa al estado de monitoreo en una máquina de estados.
 *
 * @param void
 * @return void
 */
void monitoreo()
{
  lcd.clear();
  asyncTaskTime_2.Start(); // timeout 10 sec
  asyncTaskTemp.Start();   // Revisar temperatura
  tempOnState = 25.3;
}

/**
 * @brief Función para activar la alerta de seguridad
 *
 * Esta función se llama cuando se activa la alerta de seguridad en una máquina de estados.
 *
 * @param void
 * @return void
 */
void alertaSeguridad()
{
  execute = 0;
  lcd.clear();
  asyncTaskTime_1.Stop(); // que deje de contar 2 segundos
  asyncTaskTime_2.Stop();
  asyncTaskTime_4.Stop();
  asyncTaskTemp.Stop();
  asyncTaskTime_3.Start();
  tempOnState = 24.5;
  lcd.print("Alerta seguridad");
  buzzer = true;
}

/**
 * @brief Función para activar la alarma de los sensores
 *
 * Esta función se llama cuando se activa la alarma de los sensores en una máquina de estados.
 *
 * @param void
 * @return void
 */
void alarmaSensores()
{
  asyncTaskTime_2.Stop();
  lcd.clear();

  input = (DHT.getTemperature() < 25.5) ? Input::tempVeriffy : input;

  lcd.print("Alarma ambiente");
}

/**
 * @brief Función para configurar la máquina de estados
 *
 * Esta función configura la máquina de estados y establece las transiciones y las funciones de entrada para cada estado.
 *
 * @param void
 * @return void
 */
void setupStateMachine()
{
  stateMachine.AddTransition(STATE_SEGURIDAD, STATE_PUERTA_VENTANA, []()
                             { return input == passwordCorrect; });

  stateMachine.AddTransition(STATE_PUERTA_VENTANA, STATE_MONITOR, []()
                             { return input == timeOut; });
  stateMachine.AddTransition(STATE_PUERTA_VENTANA, STATE_ALERTA_SEGURIDAD, []()
                             { return input == gateOpen; });
  stateMachine.AddTransition(STATE_ALERTA_SEGURIDAD, STATE_PUERTA_VENTANA, []()
                             { return input == timeOut; });

  stateMachine.AddTransition(STATE_MONITOR, STATE_ALARMA, []()
                             { return input == tempVeriffy; });
  stateMachine.AddTransition(STATE_MONITOR, STATE_PUERTA_VENTANA, []()
                             { return input == timeOut; });
  stateMachine.AddTransition(STATE_ALARMA, STATE_MONITOR, []()
                             { return input == tempVeriffy; });
  stateMachine.AddTransition(STATE_ALARMA, STATE_ALERTA_SEGURIDAD, []()
                             { return input == tempAndTime; });

  stateMachine.AddTransition(STATE_ALERTA_SEGURIDAD, STATE_PUERTA_VENTANA, []()
                             { return input == timeOut; });

  stateMachine.SetOnEntering(STATE_SEGURIDAD, ingresar);
  stateMachine.SetOnEntering(STATE_PUERTA_VENTANA, eventos);
  stateMachine.SetOnEntering(STATE_MONITOR, monitoreo);
  stateMachine.SetOnEntering(STATE_ALARMA, alarmaSensores);
  stateMachine.SetOnEntering(STATE_ALERTA_SEGURIDAD, alertaSeguridad);
}

void setup()
{
  lcd.begin(16, 2);
  lcd.clear();

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(intPinHall, INPUT);
  pinMode(intPinTracking, INPUT);
  pinMode(intPinMetal, INPUT);

  lcd.print("Bienvenido!");
  delay(800);
  lcd.clear();

  attachInterrupt(digitalPinToInterrupt(intPinHall), sensorHall, CHANGE);
  attachInterrupt(digitalPinToInterrupt(intPinTracking), sensorTracking, CHANGE);
  attachInterrupt(digitalPinToInterrupt(intPinMetal), sensorMetal, CHANGE);

  Serial.begin(115200);

  setupStateMachine();

  delay(1000);
  lcd.clear();

  delay(500);
  lcd.clear();

  stateMachine.SetState(STATE_SEGURIDAD, false, true);
}

void loop()
{
  input = static_cast<Input>(leerEntrada());

  asyncTaskTime_1.Update();
  asyncTaskTime_2.Update();
  asyncTaskTime_3.Update();
  asyncTaskTime_4.Update();
  asyncTaskTemp.Update();
  if (buzzer)
  {
    buzz();
  }
  else
  {
    noTone(buzzerPin);
  }
  stateMachine.Update();
}

/**
 * @brief Función para leer la entrada del teclado
 *
 * Esta función lee la entrada del teclado y verifica si se ha ingresado la clave correcta.
 *
 * @param void
 * @return int - El valor de la entrada actual, representado como un entero.
 */
int leerEntrada()
{
  Input currentInput = Input::Unknown;
  char key = keypad.getKey();

  if (key)
  {
    inputPassword[i++] = key;
    lcd.print("*");

    if (i == 5)
    {
      bool correctPassword = true;

      for (int i = 0; i < 5; i++)
      {
        if (inputPassword[i] != password[i])
        {
          correctPassword = false;
          break;
        }
      }

      lcd.clear();

      if (correctPassword)
      {
        lcd.print("Clave correcta.");

        delay(1000);
        currentInput = Input::passwordCorrect;
      }
      else
      {
        lcd.print("Clave incorrecta.");

        delay(1000);
        failCount++;

        if (failCount >= 3)
        {
          lcd.clear();
          lcd.print("Sistema bloqueado.");

          delay(1000);
        }
        else
        {
          ingresar();
          bandera = false;
        }
      }
    }
  }

  return static_cast<int>(currentInput);
}

void buzz()
{
  tone(buzzerPin, 800);
}

void sensorHall()
{
  if (stateMachine.GetState() == STATE_PUERTA_VENTANA)
  {
    input = Input::gateOpen;
  }
}

void sensorTracking()
{
  if (stateMachine.GetState() == STATE_PUERTA_VENTANA)
  {
    input = Input::gateOpen;
  }
}

void sensorMetal()
{
  if (stateMachine.GetState() == STATE_PUERTA_VENTANA)
  {
    input = Input::gateOpen;
  }
}

void timeout1()
{

  input = Input::timeOut;
}
void timeout2()
{

  input = Input::timeOut;
}
void timeout3()
{

  input = Input::timeOut;
}
void timeout4()
{

  input = Input::tempAndTime;
}

/**
 * @brief Función para medir la temperatura y actualizar el estado de la máquina de estados
 *
 * Esta función mide la temperatura y actualiza el estado de la máquina de estados según la temperatura medida. Si la temperatura es mayor o igual a 25 grados Celsius y el estado actual es STATE_MONITOR, se establece la entrada como "tempVeriffy". Si la temperatura es mayor o igual a 25 grados Celsius y el estado actual es STATE_ALARMA, se inicia una tarea asíncrona y se detiene la tarea asíncrona actual. Si la temperatura es menor que 25 grados Celsius y el estado actual es STATE_ALARMA, se establece la entrada como "tempVeriffy". También se lee la luz cada cierto intervalo de tiempo.
 *
 * @param void
 * @return void
 */
void medirTemp()
{
  input = Input::Unknown;

  int chk = DHT.read22(DHT11_PIN);

  float value_Temp = DHT.getTemperature();
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(value_Temp);

  State currentState = stateMachine.GetState();
  bool isAmbientMonitor = (currentState == STATE_MONITOR);
  bool isAmbientAlarm = (currentState == STATE_ALARMA);

  if (isAmbientMonitor && value_Temp >= 25)
  {
    input = Input::tempVeriffy;
  }

  if (isAmbientAlarm && value_Temp >= 25 && execute < 1)
  {
    execute++;
    asyncTaskTime_4.Start();
    asyncTaskTemp.Stop();
  }

  if (isAmbientAlarm && value_Temp < 25)
  {
    input = Input::tempVeriffy;
  }

  unsigned long currentTime = millis();
  if (currentTime - lastReadLightTime >= readLightInterval)
  {
    delay(3000);
    readLight();
    lastReadLightTime = currentTime;
  }
}

/**
 * @brief Función para leer el valor del sensor de luz y mostrarlo en la pantalla LCD
 *
 * Esta función lee el valor del sensor de luz y lo muestra en la pantalla LCD.
 *
 * @param void
 * @return void
 */
void readLight()
{

  int sensorValue = analogRead(photocellPin); // Leer el valor del sensor de luz

  // Mostrar el valor del sensor de luz en la pantalla LCD
  lcd.setCursor(0, 2);
  lcd.print("Luz: ");
  lcd.print(sensorValue);
  delay(2000);
  lcd.clear();
}