/*!
\file   password.ino
\date   2023
\author Javier Rojas <javierra@unicauca.edu.co>
\brief  password.
*****************************************************************************/
#include <LiquidCrystal.h>  // Library for the LiquidCrystal object
#include <Keypad.h>         // Library for the Keypad object
#include <string.h>         // Library for string comparison

/********************************************/ /**
 *  Keypad
 ***********************************************/
const char password[] = "123456";              // Password to be entered
char inPassword[7];                            // Array to store the entered password
const byte MAX_PASSWORD_LENGTH = 6;            // Maximum length of the password
byte passwordLength = 0;                       // Length of the entered password
byte tryCount = 0;                             // Number of attempts to enter the password

const byte ROWS = 4;       // Number of rows in the keypad
const byte COLS = 4;       // Number of columns in the keypad
char keys[ROWS][COLS] = {  // Array to store the keys in the keypad
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte colPins[COLS] = { 26, 27, 28, 29 };                                 // Pins connected to C1, C2, C3, C4
byte rowPins[ROWS] = { 22, 23, 24, 25 };                                 // Pins connected to R1, R2, R3, R4
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);  // Keypad object

// LiquidCrystal
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);  // LiquidCrystal object

/**
 * @brief This function clears the LCD and sets the cursor to the first position of the first row.
 * 
 */
void borrarCaracteres() {
  delay(300);           // Delay for 300 milliseconds
  lcd.clear();          // Clear the LCD
  lcd.setCursor(0, 0);  // Set the cursor to the first position of the first row
}

/**
 * @brief This function compares the entered password with the correct password.
 * 
 * @return true if the entered password is correct, false otherwise.
 */
bool comprobarContrasenia() {
  return strcmp(inPassword, password) == 0;  // Compare the entered password with the correct password
}

void setup() {
  // Keypad
  Serial.begin(9600);  // Initialize serial communication

  // LiquidCrystal
  lcd.begin(16, 2);     // Initialize the LCD
  lcd.print("Clave:");  // Display "Clave:" on the LCD
}

void loop() {
  // Mostrar cursor
  if (millis() / 250 % 2 == 0 && tryCount < 3) {  // Blink the cursor if the number of attempts is less than 3
    lcd.cursor();                                 // Show the cursor
  } else {
    lcd.noCursor();  // Hide the cursor
  }

  if (tryCount == 3) {
    lcd.setCursor(0, 1);
    lcd.print("Sis.bloqueado!!");
  } else if (passwordLength == MAX_PASSWORD_LENGTH) {
    passwordLength = 0;
    bool estado = comprobarContrasenia();
    if (estado) {
      lcd.setCursor(0, 1);
      lcd.print("Correcta :)");
      delay(1000);
      borrarCaracteres();
      lcd.print("Clave:");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Incorrecta >:(");
      delay(1000);
      borrarCaracteres();
      lcd.print("Clave:");
      tryCount++;
    }
  }

  char key = keypad.getKey();  // Get the key pressed on the keypad

  if (key) {                                     // If a key is pressed
    lcd.print("*");                              // Display "*" on the LCD
    if (passwordLength < MAX_PASSWORD_LENGTH) {  // If the length of the entered password is less than the maximum length
      inPassword[passwordLength] = key;          // Store the pressed key in the array
      passwordLength++;                          // Increment the length of the entered password
    }
  }
}