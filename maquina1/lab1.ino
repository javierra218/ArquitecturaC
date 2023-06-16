#include <StateMachineLib.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

// Definición de estados
enum States {
  SECURITY_CHECK,
  EVENT_DOORS_WINDOWS,
  MONITOR_ENVIRONMENT,
  ALARM_ENVIRONMENT,
  SECURITY_ALERT
};

// Variables y objetos globales
char password[6] = {'3', '2', '1', '3', '2', '1'};
char inPassword[6];
short int count = 0;
short int trycount = 0;

// Keypad
const uint8_t ROWS = 4;
const uint8_t COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
uint8_t colPins[COLS] = {26, 27, 28, 29};
uint8_t rowPins[ROWS] = {22, 23, 24, 25};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LiquidCrystal
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

// Instancia de StateMachine
StateMachine stateMachine;

// Función para verificar la contraseña
bool checkPassword() {
  for (int i = 0; i < 6; i++) {
    if (inPassword[i] != password[i]) {
      return false;
    }
  }
  return true;
}

// Función para borrar caracteres en la pantalla LCD
void clearCharacters() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 0);
}

// Acciones para cada estado
void securityCheckState() {
  char key = keypad.getKey();

  if (key) {
    lcd.print("*");
    inPassword[count] = key;
    count++;
  }

  if (count == 6) {
    count = 0;
    bool isPasswordCorrect = checkPassword();

    if (isPasswordCorrect) {
      lcd.setCursor(0, 1);
      lcd.print("Correcta :)");
      delay(1000);
      clearCharacters();
      lcd.print("Clave:");
      stateMachine.transitionTo(EVENT_DOORS_WINDOWS);
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Incorrecta >:(");
      delay(1000);
      clearCharacters();
      lcd.print("Clave:");
      trycount++;

      if (trycount >= 3) {
        stateMachine.transitionTo(SECURITY_ALERT);
      }
    }
  }
}

void eventDoorsWindowsState() {
  // Lógica y acciones correspondientes al estado de evento de puertas y ventanas
}

void monitorEnvironmentState() {
  // Lógica y acciones correspondientes al estado de monitoreo ambiental
}

void alarmEnvironmentState() {
  // Lógica y acciones correspondientes al estado de alarma ambiental
}

void securityAlertState() {
  // Lógica y acciones correspondientes al estado de alerta de seguridad
}

void setup() {
  // Inicialización de LCD
  lcd.begin(16, 2);
  lcd.print("Clave:");

  // Agregar estados a la máquina de estados
  stateMachine.addState(SECURITY_CHECK, securityCheckState);
  stateMachine.addState(EVENT_DOORS_WINDOWS, eventDoorsWindowsState);
  stateMachine.addState(MONITOR_ENVIRONMENT, monitorEnvironmentState);
  stateMachine.addState(ALARM_ENVIRONMENT, alarmEnvironmentState);
  stateMachine.addState(SECURITY_ALERT, securityAlertState);

  // Establecer el estado inicial
  stateMachine.setInitialState(SECURITY_CHECK);
}

void loop() {
  // Actualizar la máquina de estados
  stateMachine.update();

  // Otras lógicas o acciones que desees realizar en el bucle principal
}
