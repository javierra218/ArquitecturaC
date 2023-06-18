/*!
\file   password.ino
\date   2023
\author Javier Rojas <javierra@unicauca.edu.co>
\brief  password.
*****************************************************************************/
#include <LiquidCrystal.h>
#include <Keypad.h>

/* Display */
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

/********************************************/ /**
 *  Keypad
 ***********************************************/
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2}; // R1 = 5, R2 = 4, R3 = 3, R4 = 2
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

char password[6];
const char realpass[] = "12345";
int cont = 0;
int contpass = 3;


/**
 * @brief Configures the LCD and displays a welcome message.
 */
void setup() {
  lcd.begin(16, 2);
  pinMode(LED_RGB_RED, OUTPUT);
  pinMode(LED_RGB_GREEN, OUTPUT);
  pinMode(LED_RGB_BLUE, OUTPUT);
  lcd.print("Bienvenido");
  delay(2000);
  lcd.clear();
  lcd.print("Ingrese clave");
  lcd.setCursor(5, 1);
}

/**
 * @brief Displays a processing message with animated dots on the LCD.
 */
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

/**
 * @brief Unlocks the system after entering the correct password.
 */
void unlockSystem() {
  lcd.clear();
  procesando();
  digitalWrite(LED_RGB_GREEN, HIGH);
  delay(500);
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Es correcto!");
  delay(1000000);
}

/**
 * @brief Displays an incorrect password message and allows for more attempts or locks the system if no more attempts are available.
 */
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
  }
}

/**
 * @brief Locks the system when the password attempts are exhausted.
 */
void lockSystem() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Intentos");
  lcd.setCursor(0, 1);
  lcd.println("Agotados.");
  digitalWrite(LED_RGB_RED, HIGH);
  delay(100000);
}

/**
 * @brief Main function of the program. Reads the keypad input and performs the corresponding actions.
 */
void loop() {
  char key = keypad.getKey();

  if (key) {
    password[cont] = key;
    lcd.print("*");
    cont++;

    if (cont == 5) {
      int resultado = strncmp(password, realpass, 5);
      if (resultado == 0) {
        unlockSystem();
      } else {
        incorrectPassword();
      }
      cont = 0;
      if (contpass == 0) {
        lockSystem();
      }
    }
  }
}