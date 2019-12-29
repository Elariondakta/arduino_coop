#include "DS3231.h"
#include <Wire.h>
#include <string.h>

DS3231 Clock;

const int pinMoteurPWM = 3;
const int pinMoteurDir = 2;
bool isopen = true;

byte Year;
byte Month;
byte Date;
byte DoW;
byte Hour;
byte Minute;
byte Second;

bool Century=false;
bool h12;
bool PM;

const int maxMonth[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const char sunrise[52][5] = {
        "08:48",
    "08:46",
    "08:43",
    "08:37",
    "08:29",
    "08:19",
    "08:08",
    "07:55",
    "07:42",
    "07:28",
    "07:14",
    "06:59",
    "06:45",
    "06:30",
    "06:16",
    "06:02",
    "05:48",
    "05:36",
    "05:24",
    "05:14",
    "05:06",
    "04:59",
    "04:54",
    "04:51",
    "04:50",
    "04:52",
    "04:55",
    "05:01",
    "05:08",
    "05:16",
    "05:24",
    "05:34",
    "05:43",
    "05:53",
    "06:03",
    "06:13",
    "06:23",
    "06:32",
    "06:42",
    "06:52",
    "07:03",
    "07:13",
    "07:24",
    "07:35",
    "07:46",
    "07:58",
    "08:09",
    "08:19",
    "08:28",
    "08:36",
    "08:42",
    "08:46"
};
const char sunset[52][5] = {
    "17:59",
    "18:07",
    "18:16",
    "18:26",
    "18:38",
    "18:49",
    "19:01",
    "19:12",
    "19:24",
    "19:35",
    "19:46",
    "19:57",
    "20:07",
    "20:18",
    "20:28",
    "20:38",
    "20:49",
    "20:59",
    "21:09",
    "21:19",
    "21:28",
    "21:36",
    "21:43",
    "21:48",
    "21:52",
    "21:53",
    "21:53",
    "21:50",
    "21:45",
    "21:38",
    "21:29",
    "21:19",
    "21:07",
    "20:54",
    "20:41",
    "20:27",
    "20:12",
    "19:57",
    "19:42",
    "19:27",
    "19:13",
    "18:59",
    "18:45",
    "18:32",
    "18:21",
    "18:11",
    "18:02",
    "17:56",
    "17:51",
    "17:49",
    "17:50",
    "17:53",
};

void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW, 
		byte& Hour, byte& Minute, byte& Second) {
	// Call this if you notice something coming in on 
	// the serial port. The stuff coming in should be in 
	// the order YYMMDDwHHMMSS, with an 'x' at the end.
	boolean GotString = false;
	char InChar;
	byte Temp1, Temp2;
	char InString[20];

	byte j=0;
	while (!GotString) {
		if (Serial.available()) {
			InChar = Serial.read();
			InString[j] = InChar;
			j += 1;
			if (InChar == 'x') {
				GotString = true;
			}
		}
	}
	Serial.println(InString);
	// Read Year first
	Temp1 = (byte)InString[0] -48;
	Temp2 = (byte)InString[1] -48;
	Year = Temp1*10 + Temp2;
	// now month
	Temp1 = (byte)InString[2] -48;
	Temp2 = (byte)InString[3] -48;
	Month = Temp1*10 + Temp2;
	// now date
	Temp1 = (byte)InString[4] -48;
	Temp2 = (byte)InString[5] -48;
	Day = Temp1*10 + Temp2;
	// now Day of Week
	DoW = (byte)InString[6] - 48;		
	// now Hour
	Temp1 = (byte)InString[7] -48;
	Temp2 = (byte)InString[8] -48;
	Hour = Temp1*10 + Temp2;
	// now Minute
	Temp1 = (byte)InString[9] -48;
	Temp2 = (byte)InString[10] -48;
	Minute = Temp1*10 + Temp2;
	// now Second
	Temp1 = (byte)InString[11] -48;
	Temp2 = (byte)InString[12] -48;
	Second = Temp1*10 + Temp2;
}

int getWeekOfTheYear() {
    int spentDay = 0;
    for (int i = 0; i < (int)Clock.getMonth(Century)-1; i++) {
        spentDay += maxMonth[i];
    }
    spentDay += (int)Clock.getDate();
    //si day == 0 on retourne 1 pour pas avoir de sefault array[-1]
    int spentWeek = (int)spentDay/7;
    if (spentWeek < 1) return 1;
    else if (spentWeek > 52) return 52;
    else return spentWeek;
}

void open() {
    Serial.println("Ouverture de la porte ...");
    digitalWrite(pinMoteurDir, LOW);
    digitalWrite(pinMoteurPWM, HIGH);
    isopen = true;
}

void close() {
    Serial.println("Fermeture de la porte ...");
    digitalWrite(pinMoteurDir, HIGH);
    digitalWrite(pinMoteurPWM, HIGH);
    isopen = false;
}
void beep() {
    digitalWrite(pinMoteurDir, HIGH);
    digitalWrite(pinMoteurPWM, HIGH);
    delay(500);
    digitalWrite(pinMoteurDir, LOW);
    digitalWrite(pinMoteurPWM, HIGH);
}
void setup(void) {
    // We'll send debugging information via the Serial monitor
    pinMode(pinMoteurPWM, OUTPUT);
    pinMode(pinMoteurDir, OUTPUT);

    // Start the serial port
	Serial.begin(9600);
	// Start the I2C interface
	Wire.begin();
    beep();
    delay(5000);
    Serial.println("Redémarrage de la carte...");
}

void loop(void) {
	while (true) {
		// If something is coming in on the serial line, it's
		// a time correction so set the clock accordingly.
		if (Serial.available()) {
			Serial.println("Entrée recu");
			GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);

			Clock.setClockMode(false);	// set to 24h
			//setClockMode(true);	// set to 12h

			Clock.setYear(Year);
			Clock.setMonth(Month);
			Clock.setDate(Date);
			Clock.setDoW(DoW);
			Clock.setHour(Hour);
			Clock.setMinute(Minute);
			Clock.setSecond(Second);
		}

        Serial.println();
		Serial.print(Clock.getDate(), DEC);
		Serial.print("/");
		Serial.print(Clock.getMonth(Century), DEC);
		Serial.print("/");
		Serial.print(Clock.getYear(), DEC);
		Serial.print(" ");
		Serial.print(Clock.getHour(h12, PM), DEC); //24-hr
		Serial.print(":");
		Serial.println(Clock.getMinute(), DEC);
        Serial.println();

        int weekOfTheYear = getWeekOfTheYear();

        int rise_h = String(sunrise[weekOfTheYear-1]).substring(0, 2).toInt();
        int rise_m = String(sunrise[weekOfTheYear-1]).substring(3, 5).toInt();
        int set_h = String(sunset[weekOfTheYear-1]).substring(0, 2).toInt();
        int set_m = String(sunset[weekOfTheYear-1]).substring(3, 5).toInt();              

        DateTime now = DateTime((int)Clock.getYear(), (int)Clock.getMonth(Century), (int)Clock.getDate(), (int)Clock.getHour(h12, PM), (int)Clock.getMinute(), (int)Clock.getSecond());
        DateTime rise = DateTime((int)Clock.getYear(), (int)Clock.getMonth(Century), (int)Clock.getDate(), rise_h, rise_m, 0);
        DateTime set = DateTime((int)Clock.getYear(), (int)Clock.getMonth(Century), (int)Clock.getDate(), set_h, set_m, 0);
        
        //Si l'heure actuelle est comprise entre le levé et le coucher de soleil
        //si il fait jour
        if (now.unixtime() < set.unixtime() && now.unixtime() > rise.unixtime()) {
            if (!isopen) {
                open();
            }
        } else {    //si il fait nuit
            if (isopen) {
                close();
            }
        }


        Serial.print("Coucher de soleil : ");
        Serial.print(set.hour());
        Serial.print(":");
        Serial.println(set.minute());
        Serial.println("----------------");

        Serial.print("Lever de soleil : ");
        Serial.print(rise.hour());
        Serial.print(":");
        Serial.println(rise.minute());
        Serial.println("----------------");

        Serial.print("Date : ");
		Serial.print(Clock.getDate(), DEC);
		Serial.print("/");
		Serial.print(Clock.getMonth(Century), DEC);
		Serial.print("/");
		Serial.print(Clock.getYear(), DEC);
		Serial.print(" ");
		Serial.print(Clock.getHour(h12, PM), DEC); //24-hr
		Serial.print(":");
		Serial.println(Clock.getMinute(), DEC);
        Serial.println("----------------");

		delay(10000);
	}
}