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

void restartLCD()
{
    //delay
    delay(500);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 0);
}

void sensorTemp(){
    digitalWrite(LED_GREEN,LOW);

    restartLCD();
    lcd.print("Temp: ");

    lcd.setCursor(4, 0);

    lcd.setCursor(4, 0);
    DHT.read22(DHT_PIN);
    lcd.print(DHT.getTemperatureC());
}

void sensorLuz(){
    lcd.setCursor(0, 1);
    lcd.print("Luz: ");
    lcd.setCursor(4, 1);
    lcd.print(analogRead(PHOTOCELL_PIN));
    lcd.print(outputValue);
}

void alarma(){
    restartLCD();

    lcd.print("Alarma");

    lcd.setCursor(0, 1);
    lcd.print("Activada");

    lcd.setCursor(0, 1);
    DHT.read22(DHT_PIN);

    //BUZZER SONIDO ALARMA
    tone(BUZZER, 1000);
    delay(500);
    noTone(BUZZER);
}

//tareas asincronas y su procedimientos
AsyncTask asyncTask1(3000,true,sensorTemp);
AsyncTask asyncTask2(3000,true,sensorLuz);
AsyncTask asyncTask3(3000,true,alarma);

void setup(){
    Serial.begin(9600);
    lcd.begin(16, 2);

    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    asyncTask1.start();
    asyncTask2.start();
    asyncTask3.start();

}

void loop(){
    DHT.read22(DHT_PIN);
    if(DHT.getTemperatureC() > 30){
        asyncTask3.start();
    }
    else{
        //actualiza tarea 1 y 2 
        asyncTask1.update();
        
    }
}