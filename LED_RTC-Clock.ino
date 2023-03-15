#include <DS3231.h>

DS3231 RTC;

//booleans for printing time
bool CentBit;
bool hr;
bool PM;

//vars for setting time
byte year;
byte month;
byte date;
byte dOW;
byte hour;
byte minute;
byte second;

void getDateStuff(byte& year, byte& month, byte& date, byte& dOW,
                  byte& hour, byte& minute, byte& second) {
  // Call this if you notice something coming in on
  // the serial port. The stuff coming in should be in
  // the order YYMMDDwHHMMSS, with an 'x' at the end.
  boolean gotString = false;
  char inChar;
  byte temp1, temp2;
  char inString[20];

  byte j = 0;
  while (!gotString) {
    if (Serial.available()) {
      inChar = Serial.read();
      inString[j] = inChar;
      j += 1;
      if (inChar == 'x') {
        gotString = true;
      }
    }
  }
  Serial.println(inString);
  // Read year first
  temp1 = (byte)inString[0] - 48;
  temp2 = (byte)inString[1] - 48;
  year = temp1 * 10 + temp2;
  // now month
  temp1 = (byte)inString[2] - 48;
  temp2 = (byte)inString[3] - 48;
  month = temp1 * 10 + temp2;
  // now date
  temp1 = (byte)inString[4] - 48;
  temp2 = (byte)inString[5] - 48;
  date = temp1 * 10 + temp2;
  // now Day of Week
  dOW = (byte)inString[6] - 48;
  // now hour
  temp1 = (byte)inString[7] - 48;
  temp2 = (byte)inString[8] - 48;
  hour = temp1 * 10 + temp2;
  // now minute
  temp1 = (byte)inString[9] - 48;
  temp2 = (byte)inString[10] - 48;
  minute = temp1 * 10 + temp2;
  // now second
  temp1 = (byte)inString[11] - 48;
  temp2 = (byte)inString[12] - 48;
  second = temp1 * 10 + temp2;
}

void funcRTC() { //function that contains all RTC functions
  if (Serial.available()) {  //if data is entered into the serial monitor, parse it as a time update
    getDateStuff(year, month, date, dOW, hour, minute, second);

    RTC.setClockMode(false);  // set to 24h

    RTC.setYear(year);
    RTC.setMonth(month);
    RTC.setDate(date);
    RTC.setDoW(dOW);
    RTC.setHour(hour);
    RTC.setMinute(minute);
    RTC.setSecond(second);
  }

  //Print the date and time output by the RTC
  Serial.print(RTC.getDoW(), DEC);
  Serial.print(' ');
  Serial.print(RTC.getYear(), DEC);
  Serial.print('/');
  Serial.print(RTC.getMonth(CentBit), DEC);
  Serial.print('/');
  Serial.print(RTC.getDate(), DEC);
  Serial.print(' ');
  Serial.print(RTC.getHour(hr, PM), DEC);
  Serial.print(':');
  Serial.print(RTC.getMinute(), DEC);
  Serial.print(':');
  Serial.print(RTC.getSecond(), DEC);
  Serial.println();
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  delay(1000);
  funcRTC();
}
