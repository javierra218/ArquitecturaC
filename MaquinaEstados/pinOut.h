const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};

#define DHT11_PIN A15
const int buzzerPin = A14;
int photocellPin = A10; // Pin utilizado para el sensor de luz

const int redPin = 15;
const int greenPin = 16;
const int bluePin = 17;
const int intPinHall = 2;
const int intPinMetal = 1;
const int intPinTracking = 0;