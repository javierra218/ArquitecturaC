#include "AsyncTaskLib.h"
#include "DHTStable.h"
#include <LiquidCrystal.h>

// Definición de constantes para los pines usados
#define DHT_PIN A1
#define PHOTOCELL_PIN A0
#define LED_RED 51
#define LED_GREEN 52
#define LED_BLUE 53
#define BUZZER 5

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

DHTStable DHT;

int outputValue = 0;

// Procedimiento para reiniciar el LCD
void reiniciarLCD() {
  delay(300);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 0);
}

// Procedimiento utilizado en la tarea asíncrona 1 (Humedad y Temperatura)
void activarDHC() {
  digitalWrite(LED_GREEN, LOW); // Desactivar led

  reiniciarLCD();
  lcd.print("H/T:");

  lcd.setCursor(4, 0);
  lcd.print("                ");

  lcd.setCursor(4, 0);
  DHT.read22(DHT_PIN); // Lectura del sensor DHT22
  lcd.print(DHT.getHumidity()); // Obtener humedad y mostrar en LCD

  lcd.print(" ");
  lcd.print(DHT.getTemperature()); // Obtener temperatura y mostrar en LCD
}

// Procedimiento utilizado en la tarea asíncrona 2 (Luz)
void activarPhotocell() {
  lcd.setCursor(0, 1);
  lcd.print("Luz:");

  lcd.setCursor(4, 1);
  lcd.print("                ");

  lcd.setCursor(4, 1);
  outputValue = analogRead(PHOTOCELL_PIN);
  lcd.print(outputValue);
}

// Procedimiento utilizado en la tarea asíncrona 3 (Alarma ambiental con Led y Buzzer)
void activarAlarmaAmbiental() {
  reiniciarLCD();

  digitalWrite(LED_GREEN, HIGH);

  lcd.print("ALERTA DE TEMP!");

  lcd.setCursor(0, 1);
  lcd.print("TEMP:");

  lcd.setCursor(5, 1);
  lcd.print("                ");
  lcd.setCursor(5, 1);
  DHT.read22(DHT_PIN); // Lectura del sensor DHT22
  lcd.print(DHT.getTemperature()); // Obtener temperatura y mostrar en LCD

  // Activar buzzer, tono de alerta
  tone(BUZZER, 1000, 200);
  delay(200);
  noTone(BUZZER);
}

// Creación de tareas asíncronas con sus respectivos procedimientos
AsyncTask asyncTask1(3000, true, activarDHC);
AsyncTask asyncTask2(1000, true, activarPhotocell);
AsyncTask asyncTask3(1500, true, activarAlarmaAmbiental);

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);

  lcd.print("H/T:");
  lcd.setCursor(0, 1);
  lcd.print("Luz:");

  pinMode(LED_RED, OUTPUT);   // Sistema bloqueado
  pinMode(LED_BLUE, OUTPUT);  // Clave incorrecta
  pinMode(LED_GREEN, OUTPUT); // Clave correcta
  pinMode(BUZZER, OUTPUT);

  // Inicio de tareas asíncronas
  asyncTask1.Start();
  asyncTask2.Start();
  asyncTask3.Start();
}

void loop() {
  DHT.read22(DHT_PIN);
  if (DHT.getTemperature() > 20) {
    asyncTask3.Update(); // Actualización de la tarea asíncrona 3 cuando la temperatura es mayor a x grados Celsius
  } else {
    // Actualización de tareas 1 y 2 cuando la temperatura es inferior a x grados Celsius
    asyncTask1.Update();
    asyncTask2.Update();
  }
}
