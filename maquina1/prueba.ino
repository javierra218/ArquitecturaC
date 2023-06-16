// la maquina de estados tiene como primero el ingreso de seguridad, si la clave es correcta me dirijo hacia evento puerta y ventanas (algo que no toca incluir por el momento), entonces paso con un timeout de 2 segundos hacia monitor ambiental que es donde estan los sensores de temperatura, humedad y luz, si la tempertatura es mayor a 32 grados me dirijo hacia alarma ambiental donde hay un buzzer, si la temperatura baja de 30 grados me devuelvo hacia monitor ambiental, si la temperatura es mayor a 32 grados durante 5 segundos que es el mismo tiempo de sonido del buzzer me dirijo hacia el estado de alerta de seguridad donde hay un relero(relay) y de ahi con un timeout de 6 segundos vuelvo hacia evento de puertas y ventanas

// Incluir las bibliotecas necesarias
#include <LiquidCrystal.h>

// Definir los pines y variables

// Asegúrate de ajustar los pines según tus conexiones
#define KEYPAD_PIN A0
#define BUZZER_PIN 9
#define RELAY_PIN 10

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Configura los pines del LCD

// Definir los estados de la máquina de estados
enum State
{
    SECURITY_CHECK,
    EVENT_DOORS_WINDOWS,
    MONITOR_ENVIRONMENT,
    ALARM_ENVIRONMENT,
    SECURITY_ALERT
};

State currentState;
unsigned long stateStartTime;
unsigned long buzzerStartTime;

// Definir la función para cambiar de estado
void changeState(State newState)
{
    currentState = newState;
    stateStartTime = millis();
}

// Configurar el setup
void setup()
{
    // Inicializar los componentes y el LCD
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    lcd.begin(16, 2);
    lcd.print("Ingresar clave:");
    currentState = SECURITY_CHECK;
}

boolean comprobarContrasenia()
{
    for (int i = 0; i < 6; i++)
    {
        if (inPassword[i] != password[i])
        {
            return false;
        }
    }
    return true;
}

void borrarCaracteres()
{
    delay(300);
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 0);
}

// Implementar el loop
void loop()
{
    switch (currentState)
    {
    case SECURITY_CHECK:
        // Lógica para verificar la clave
        // Si la clave es correcta, cambiar de estado a EVENT_DOORS_WINDOWS
        // Si la clave es incorrecta, permanecer en SECURITY_CHECK
        // Puedes utilizar el teclado matricial para capturar la clave

        char key = keypad.getKey();
        if (key)
        {
            lcd.print("*");
            inPassword[count] = key;
            count++;
        }

        if (count == 6)
        {
            count = 0;
            bool estado = comprobarContrasenia();
            if (estado)
            {
                changeState(EVENT_DOORS_WINDOWS);
                // Realizar las accines correspondientes al ingresar correctamente la contraseña
            }
            else
            {
                trycount++;
                if (trycount == 3)
                {
                    // Realizar las acciones correspondientes al ingresar incorrectamente la contraseña 3 veces
                    trycount = 0;
                }
                else
                {
                    //// Realiza las acciones correspondientes al ingresar una contraseña incorrecta
                }
            }
            borrarCaracteres();
            lcd.print("Clave:");
        }

        break;

    case EVENT_DOORS_WINDOWS:
        // Lógica para controlar eventos de puertas y ventanas
        // Puedes implementarla más adelante

        break;

    case MONITOR_ENVIRONMENT:
        // Lógica para monitorear el ambiente (sensores de temperatura, humedad y luz)
        // Si la temperatura es mayor a 32 grados, cambiar de estado a ALARM_ENVIRONMENT
        // Si la temperatura baja de 30 grados, cambiar de estado a MONITOR_ENVIRONMENT
        // Puedes utilizar el sensor DHT11 y el fotoresistor para obtener los valores

        break;

    case ALARM_ENVIRONMENT:
        // Lógica para activar la alarma ambiental (buzzer) y manejar el tiempo de sonido
        // Si la temperatura baja de 32 grados durante 5 segundos, cambiar de estado a MONITOR_ENVIRONMENT
        // Puedes utilizar el buzzer y las variables buzzerStartTime y millis()

        break;

    case SECURITY_ALERT:
        // Lógica para activar el relé durante 6 segundos y luego volver a EVENT_DOORS_WINDOWS
        // Puedes utilizar el relay y las variables stateStartTime y millis()

        break;
    }
}
