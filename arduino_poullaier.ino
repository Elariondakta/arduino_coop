int photocellPin = 0; // the cell and 10K pulldown are connected to a0
int photocellReading; // the analog reading from the analog resistor divider
int pinMoteurPWM = 3;
int pinMoteurDir = 2;
bool isopen;

void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);
  pinMode(pinMoteurPWM, OUTPUT);
  pinMode(pinMoteurDir, OUTPUT);
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
  } 
  else if (photocellReading > 300 && isopen == false) {
    Serial.println(" - Lumiere");
    digitalWrite(pinMoteurDir, LOW);
    digitalWrite(pinMoteurPWM, HIGH);
    isopen = true;
  } 
  delay(1000);
}

