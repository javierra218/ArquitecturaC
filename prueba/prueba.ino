#include <LiquidCrystal.h>
#include <Keypad.h>
#include <DHT.h>

/* Display */
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

/********************************************/ /**
 *  Keypad
 ***********************************************/
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'.', '0', '=', '/'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

const int LED_RGB_RED = 6;
const int LED_RGB_GREEN = 13;
const int LED_RGB_BLUE = 1;
const int BUZZER_PIN = 4;
const int DHT_PIN = A4;

char password[6];
const char realpass[] = "12345";
int cont = 0;
int contpass = 3;

DHT dht(DHT_PIN, DHT22);

enum State {
  ENTER_PASSWORD,
  CHECK_PASSWORD,
  UNLOCKED,
  TEMPERATURE_MEASUREMENT,
  TEMPERATURE_HIGH,
};

State currentState = ENTER_PASSWORD;

unsigned long timeoutStart = 0;
const unsigned long TIMEOUT_DURATION = 2000; // 2 segundos
const float TEMPERATURE_THRESHOLD_HIGH = 32.0;
const float TEMPERATURE_THRESHOLD_LOW = 30.0;
unsigned long buzzerStart = 0;
const unsigned long BUZZER_DURATION = 5000; // 5 segundos

void setup() {
  lcd.begin(16, 2);
  pinMode(LED_RGB_RED, OUTPUT);
  pinMode(LED_RGB_GREEN, OUTPUT);
  pinMode(LED_RGB_BLUE, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  lcd.print("Bienvenido");
  delay(2000);
  lcd.clear();
  lcd.print("Ingrese clave");
  lcd.setCursor(5, 1);
  dht.begin();
}

void procesando() {
  for (int contador = 0; contador < 3; contador++) {
    lcd.setCursor(2, 0);
    lcd.print("Espera...");
    for (int i = 12; i < 15; i++) {
      lcd.setCursor(i, 0);
      lcd.print(".");
      delay(200);
    }
    lcd.clear();
  }
}

void unlockSystem() {
  lcd.clear();
  procesando();
  digitalWrite(LED_RGB_GREEN, HIGH);
  delay(500);
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Clave Correcta");
  currentState = UNLOCKED;
  timeoutStart = millis(); // Inicia el timeout de 2 segundos
}

void incorrectPassword() {
  lcd.clear();
  procesando();
  delay(300);
  lcd.clear();
  contpass--;
  if (contpass > 0) {
    lcd.setCursor(2, 0);
    digitalWrite(LED_RGB_BLUE, HIGH);
    lcd.print("Incorrecto!");
    delay(300);
    lcd.setCursor(0, 1);
    lcd.print("Intente de nuevo");
    delay(2000);
    lcd.clear();
    digitalWrite(LED_RGB_BLUE, LOW);
    lcd.setCursor(1, 0);
    lcd.print("Ingrese Clave");
    lcd.setCursor(5, 1);
    cont = 0; // Reinicia el contador de la contraseña para permitir ingresarla nuevamente
    currentState = ENTER_PASSWORD; // Cambia al estado de ingresar contraseña nuevamente
  } else {
    lockSystem();
  }
}



void lockSystem() {
  contpass = 3; // Restablecer el valor de contpass a 3
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Intentos");
  lcd.setCursor(0, 1);
  lcd.println("Agotados.");
  digitalWrite(LED_RGB_RED, HIGH);
  currentState = ENTER_PASSWORD;
}


void measureTemperature() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    lcd.clear();
    lcd.print("Error al leer");
    lcd.setCursor(0, 1);
    lcd.print("la temperatura");
    currentState = UNLOCKED;
    timeoutStart = millis();
    return;
  }

  lcd.clear();
  lcd.print("Temperatura: ");
  lcd.print(temperature);
  lcd.print(" C");

  if (temperature > TEMPERATURE_THRESHOLD_HIGH) {
    currentState = TEMPERATURE_HIGH;
    buzzerStart = millis();
  } else if (temperature < TEMPERATURE_THRESHOLD_LOW) {
    currentState = TEMPERATURE_MEASUREMENT;
  }
}

void buzzerAlarm() {
  unsigned long currentTime = millis();
  if (currentTime - buzzerStart < BUZZER_DURATION) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    currentState = TEMPERATURE_MEASUREMENT;
  }
}

void loop() {
  switch (currentState) {
    case ENTER_PASSWORD:
      enterPasswordState();
      break;
    case CHECK_PASSWORD:
      checkPasswordState();
      break;
    case UNLOCKED:
      unlockedState();
      break;
    case TEMPERATURE_MEASUREMENT:
      temperatureMeasurementState();
      break;
    case TEMPERATURE_HIGH:
      temperatureHighState();
      break;
  }
}

void enterPasswordState() {
  char key = keypad.getKey();
  if (key) {
    if (cont < 5) {
      password[cont] = key;
      lcd.print("*");
      cont++;
    }

    if (cont == 5) {
      currentState = CHECK_PASSWORD;
    }
  }
}


void checkPasswordState() {
  if (cont == 5) {
    if (strcmp(password, realpass) == 0) {
      unlockSystem();
      currentState = UNLOCKED; // Cambio de estado después de ingresar la contraseña correcta
    } else {
      incorrectPassword();
    }
  }
}



void unlockedState() {
  unsigned long currentTime = millis();
  if (currentTime - timeoutStart >= TIMEOUT_DURATION) {
    lcd.clear();
    lcd.print("Ingrese clave");
    lcd.setCursor(5, 1);
    currentState = ENTER_PASSWORD;
    contpass = 3;
  } else {
    currentState = TEMPERATURE_MEASUREMENT;
  }
}

void temperatureMeasurementState() {
  measureTemperature();
  unsigned long currentTime = millis();
  if (currentTime - timeoutStart >= TIMEOUT_DURATION) {
    currentState = UNLOCKED;
    timeoutStart = millis();
  }
}

void temperatureHighState() {
  buzzerAlarm();
  unsigned long currentTime = millis();
  if (currentTime - timeoutStart >= TIMEOUT_DURATION) {
    currentState = UNLOCKED;
    timeoutStart = millis();
  }
}
