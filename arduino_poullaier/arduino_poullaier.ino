#include "DS3231.h"
#include <Wire.h>
#include <string.h>

DS3231 Clock;

int pinMoteurPWM = 3;
int pinMoteurDir = 2;
bool isopen;
bool day;

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

int maxMonth[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
String sunrise[52] = {
    String("09:48"),
    String("09:46"),
    String("09:43"),
    String("09:37"),
    String("09:29"),
    String("09:19"),
    String("09:08"),
    String("08:55"),
    String("08:42"),
    String("08:28"),
    String("08:14"),
    String("07:59"),
    String("07:45"),
    String("07:30"),
    String("07:16"),
    String("07:02"),
    String("06:48"),
    String("06:36"),
    String("06:24"),
    String("06:14"),
    String("06:06"),
    String("05:59"),
    String("05:54"),
    String("05:51"),
    String("05:50"),
    String("05:52"),
    String("05:55"),
    String("06:01"),
    String("06:08"),
    String("06:16"),
    String("06:24"),
    String("06:34"),
    String("06:43"),
    String("06:53"),
    String("07:03"),
    String("07:13"),
    String("07:23"),
    String("07:32"),
    String("07:42"),
    String("07:52"),
    String("08:03"),
    String("08:13"),
    String("08:24"),
    String("08:35"),
    String("08:46"),
    String("08:58"),
    String("09:09"),
    String("09:19"),
    String("09:28"),
    String("09:36"),
    String("09:42"),
    String("09:46"),
};
String sunset[52] = {
    String("17:59"),
    String("18:07"),
    String("18:16"),
    String("18:26"),
    String("18:38"),
    String("18:49"),
    String("19:01"),
    String("19:12"),
    String("19:24"),
    String("19:35"),
    String("19:46"),
    String("19:57"),
    String("20:07"),
    String("20:18"),
    String("20:28"),
    String("20:38"),
    String("20:49"),
    String("20:59"),
    String("21:09"),
    String("21:19"),
    String("21:28"),
    String("21:36"),
    String("21:43"),
    String("21:48"),
    String("21:52"),
    String("21:53"),
    String("21:53"),
    String("21:50"),
    String("21:45"),
    String("21:38"),
    String("21:29"),
    String("21:19"),
    String("21:07"),
    String("20:54"),
    String("20:41"),
    String("20:27"),
    String("20:12"),
    String("19:57"),
    String("19:42"),
    String("19:27"),
    String("19:13"),
    String("18:59"),
    String("18:45"),
    String("18:32"),
    String("18:21"),
    String("18:11"),
    String("18:02"),
    String("17:56"),
    String("17:51"),
    String("17:49"),
    String("17:50"),
    String("17:53")
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

//
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
    Serial.println("Fermeture de la porte ...");
    digitalWrite(pinMoteurDir, HIGH);
    digitalWrite(pinMoteurPWM, HIGH);
    isopen = false;
}

void close() {
    Serial.println("Ouverture de la porte ...");
    digitalWrite(pinMoteurDir, LOW);
    digitalWrite(pinMoteurPWM, HIGH);
    isopen = true;
}
void setup(void) {
    // We'll send debugging information via the Serial monitor
    pinMode(pinMoteurPWM, OUTPUT);
    pinMode(pinMoteurDir, OUTPUT);

    // Start the serial port
	Serial.begin(9600);

	// Start the I2C interface
	Wire.begin();
}

void loop(void) {
	// Give time at next five seconds
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

		// Serial.print(Clock.getDate(), DEC);
		// Serial.print("/");
		// Serial.print(Clock.getMonth(Century), DEC);
		// Serial.print("/");
		// Serial.print(Clock.getYear(), DEC);
		// Serial.print(" ");
		// Serial.print(Clock.getHour(h12, PM), DEC); //24-hr
		// Serial.print(":");
		// Serial.print(Clock.getMinute(), DEC);


        int weekOfTheYear = getWeekOfTheYear();

        // delay(100);
        String sunrise = sunrise[weekOfTheYear-1];
        // String sunset = sunset[weekOfTheYear-1];
        // String sunrise_h = sunrise[weekOfTheYear-1];
        // String sunrise_m = sunrise[weekOfTheYear-1];
        delay(1000);
        Serial.println(sunrise[weekOfTheYear-1]);
        // Serial.println(sunrise_m);
        

		delay(1000);
	}
}