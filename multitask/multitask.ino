/**
 * @file multitask.ino 
 * @author  Javier Rojas <javierra@unicauca.edu.co
 * @brief Multitask.
 */

#include "AsyncTaskLib.h"
#include "DHTStable.h"
#include <LiquidCrystal.h>
#include "pinout.h"

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

DHTStable DHT;

int lightValue = 0;

/**
 * @brief Displays humidity and temperature on the LCD.
 */
void displayHumidityAndTemperature(){
  lcd.setCursor(4,0);
  lcd.print("                ");
  lcd.setCursor(4,0);
  DHT.read11(DHT11_PIN);
  lcd.print(DHT.getHumidity());
  lcd.print("\t");
  lcd.println(DHT.getTemperature());
}

/**
 * @brief Displays the light value on the LCD.
 */
void displayLightValue(){
  lcd.setCursor(4,1);
  lcd.print("                ");
  lcd.setCursor(4,1);
  lightValue = analogRead(PHOTOCELL_PIN);
  lcd.print(lightValue);
}

AsyncTask humidityAndTemperatureTask(3000, true, displayHumidityAndTemperature); /**< Asynchronous task to display humidity and temperature. */
AsyncTask lightValueTask(1000, true, displayLightValue); /**< Asynchronous task to display light value. */

/**
 * @brief Initial setup of the Arduino.
 */
void setup()
{
  Serial.begin(115200);
  lcd.begin(16, 2);

  lcd.print("H - T:");
  lcd.setCursor(0,1);
  lcd.print("L:");

  humidityAndTemperatureTask.Start();
  lightValueTask.Start();
}

/**
 * @brief Main loop of the Arduino.
 */
void loop()
{
  humidityAndTemperatureTask.Update();
  lightValueTask.Update();
}
