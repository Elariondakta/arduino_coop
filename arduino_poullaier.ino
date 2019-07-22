#include <LiquidCrystal.h>
 // https://www.arduino.cc/en/Reference/LiquidCrystal
int photocellPin = 0; // the cell and 10K pulldown are connected to a0
int photocellReading; // the analog reading from the analog resistor divider
int pinMoteurPWM = 3;
int pinMoteurDir = 2;
int pinButton1 = 4, pinButton2 = 5, pinButton3 = 6;
bool isopen;

LiquidCrystal screen(13, 12, 11, 10, 9, 8);
void setup(void) {
    // We'll send debugging information via the Serial monitor
    Serial.begin(9600);
    pinMode(pinMoteurPWM, OUTPUT);
    pinMode(pinMoteurDir, OUTPUT);
    pinMode(pinButton1, INPUT_PULLUP);
    pinMode(pinButton2, INPUT_PULLUP);
    pinMode(pinButton3, INPUT_PULLUP);

    // 16 rows 2 columns
    screen.begin(16, 2);
}

void loop(void) {
    photocellReading = analogRead(photocellPin);

    Serial.print("Analog reading = ");
    Serial.print(photocellReading); // the raw analog reading

    
    if (photocellReading < 200 && isopen == true) {
        Serial.println(" - Sombre");
        digitalWrite(pinMoteurDir, HIGH);
        digitalWrite(pinMoteurPWM, HIGH);
        isopen = false;
    } else if (photocellReading > 300 && isopen == false) {
        Serial.println(" - Lumiere");
        digitalWrite(pinMoteurDir, LOW);
        digitalWrite(pinMoteurPWM, HIGH);
        isopen = true;
    }
}