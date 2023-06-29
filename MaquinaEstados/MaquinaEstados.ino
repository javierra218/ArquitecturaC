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
void leerPass();

void intHallSensor();
void intTrackingSensor();
void intMetalTouchSensor();

#define DEBUG(a)          \
  Serial.print(millis()); \
  Serial.print(": ");     \
  Serial.println(a);

// Def liquid and dht
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
DHTStable DHT;

#pragma region TareasAsinc
AsyncTask asyncTaskTime_1(2000, timeout1);
AsyncTask asyncTaskTime_2(10000, timeout2);
AsyncTask asyncTaskTime_3(6000, timeout3);
AsyncTask asyncTaskTime_4(5000, timeout4);
AsyncTask asyncTaskTemp(500, true, medirTemp);
#pragma endregion TareasAsinc

// Keypad setup
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', '+'},
    {'4', '5', '6', '-'},
    {'7', '8', '9', '*'},
    {'.', '0', '=', '/'}};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);


int fre; // set the variable to store the frequence value
int execute = 0;

float tempOnState = 0;
char inputPassword[5];
char password[5] = "12345";
unsigned char idx = 0;
int failCount = 0;
bool flag = false;
bool buzzer = false;

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

// Create new StateMachine
StateMachine stateMachine(5, 8);

// Stores last user input
Input input;
//====================================================
// Entrys
//====================================================
void entryIngreso()
{

  idx = 0;
  flag = false;

  lcd.clear();

  lcd.print("Ingrese la clave:");
  lcd.setCursor(0, 1);
}
void entryEvents()
{
  buzzer = false;
  lcd.clear();
  asyncTaskTime_1.Start(); // timeout 2 sec
  asyncTaskTemp.Stop();    // Detener escaneo de temperatura
  lcd.print("En eventos");
}
void entryMonitor()
{

  lcd.clear();
  asyncTaskTime_2.Start(); // timeout 10 sec
  asyncTaskTemp.Start();   // Revisar temperatura
  tempOnState = 25.3;
  lcd.print("En monitor");
}
void entryAlertSecurity()
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
void entryAlarmEnvironment()
{
  asyncTaskTime_2.Stop();
  lcd.clear();

  input = (DHT.getTemperature() < 25.5) ? Input::tempVeriffy : input;

  lcd.print("Alarma ambiente");
}

//====================================================
//====================================================
// Setup the State Machine
//====================================================
void setupStateMachine()
{
  // Add transitions
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

  // Add actions
  stateMachine.SetOnEntering(STATE_SEGURIDAD, entryIngreso);
  stateMachine.SetOnEntering(STATE_PUERTA_VENTANA, entryEvents);
  stateMachine.SetOnEntering(STATE_MONITOR, entryMonitor);
  stateMachine.SetOnEntering(STATE_ALARMA, entryAlarmEnvironment);
  stateMachine.SetOnEntering(STATE_ALERTA_SEGURIDAD, entryAlertSecurity);
}

//====================================================
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

  attachInterrupt(digitalPinToInterrupt(intPinHall), intHallSensor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(intPinTracking), intTrackingSensor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(intPinMetal), intMetalTouchSensor, CHANGE);

  Serial.begin(115200);

  setupStateMachine();

  delay(1000);
  lcd.clear();

  delay(500);
  lcd.clear();

  // Initial state
  stateMachine.SetState(STATE_SEGURIDAD, false, true);
}
//====================================================

//====================================================
void loop()
{
  // Read user input
  input = static_cast<Input>(readInput());

  // Update async tasks
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
  // Update State Machine
  stateMachine.Update();
}
//====================================================

//====================================================
// Auxiliar function that reads the user input
//====================================================
int readInput()
{
  Input currentInput = Input::Unknown;
  char key = keypad.getKey();

  if (key)
  {
    inputPassword[idx++] = key;
    lcd.print("*");

    if (idx == 5)
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
          entryIngreso();
          flag = false;
        }
      }
    }
  }

  return static_cast<int>(currentInput);
}

//====================================================
// buzz
//====================================================

void buzz()
{
  tone(buzzerPin, 800);
}
//====================================================
// Interrupciones
//====================================================

void intHallSensor()
{
  if (stateMachine.GetState() == STATE_PUERTA_VENTANA)
  {
    input = Input::gateOpen;
  }
}

void intTrackingSensor()
{
  if (stateMachine.GetState() == STATE_PUERTA_VENTANA)
  {
    input = Input::gateOpen;
  }
}

void intMetalTouchSensor()
{
  if (stateMachine.GetState() == STATE_PUERTA_VENTANA)
  {
    input = Input::gateOpen;
  }
}

//====================================================
// timeouts
//====================================================
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
//====================================================
// medirTemp
//====================================================
void medirTemp()
{
  input = Input::Unknown;

  int chk = DHT.read22(DHT11_PIN);

  // DISPLAY DATA
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
}
