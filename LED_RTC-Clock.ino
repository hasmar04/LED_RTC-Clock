//LED_RTC-Clock by Harrison Asmar

// To set the time, enter it into the serial monitor in the order YYMMDDwHHMMSS, with an 'x' at the end
// YY -> Year
// MM -> Month
// DD -> Day
// w -> Day of Week (Sunday is 0)
// HH -> Hour (24hr)
// MM -> Minute
// SS -> Second

//                  LED MATRIX
/*-----------------------------------------------------------------------
Only 7 of the 16 pins on the DMD are actually used:
GND - Hopefully obvious
nOE - Enable / Disable ALL the LEDs on the DMD
A - A&B pins select which 1/4 of the DMD is selected
B - A&B pins select which 1/4 of the DMD is selected
CLK - Used to clock each pixel into the DMD shift registers
SCLK - Latches the current content of the DMD shift registers
R - The raw pixel data stream (NB: The 'R' stands for RED. You might notice an unused 'G' pin which is for
Green on a HUB12 connector)
--------
nOE |1   2| A
Gnd |3   4| B
Gnd |5   6| C
Gnd |6   8| CLK
Gnd |7  10| SCLK
Gnd |11 12| R
Gnd |13 14| G
Gnd |15 16| D
--------------------------------------------------------------------------------*/

#include <DS3231.h>
#include "SPI.h"
#include "DMD.h"
#include "TimerOne.h"
#include "SystemFont5x7.h"

//Declare RTC function
DS3231 RTC;

//booleans for printing time
bool CentBit;
bool hr;
bool PM;

//Global array for RTCdate, RTCtime
String Array[2];

//vars for setting time
byte year;
byte month;
byte date;
byte dOW;
byte hour;
byte minute;
byte second;

void getDateStuff(byte& year, byte& month, byte& date, byte& dOW, byte& hour, byte& minute, byte& second) {  //Function for parsing time data collected from the serial monitor so it can be used to set the RTC.
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

String leadingZero(String num) { //function that adds a leading zero if the number is less than 10 i.e. string length is less than 2
  if (num.length() < 2){
    return String(0)+num;
  }
  return num;
}

void funcRTC() {             //function that contains all RTC functions
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

    //Print time that has been set
    Serial.print('The following time has been saved to the RTC: ');
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

  String RTCdate,RTCtime;


  //Print the date and time output by the RTC
  RTCdate=leadingZero(String(RTC.getDate()))+'/'+leadingZero(String(RTC.getMonth(CentBit)));
  Serial.print(RTCdate+' ');
  RTCtime=leadingZero(String(RTC.getHour(hr,PM)))+':'+leadingZero(String(RTC.getMinute()));
  Serial.println(RTCtime);
  Array[0] = RTCdate;
  Array[1] = RTCtime;
}

// LED MATRIX SETUP

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

void ScanDMD() {
  dmd.scanDisplayBySPI();
}

const byte PanelWidth = 32;
const byte MaxStringLength = 5;
char CharBuf[MaxStringLength + 1];

void center_theDisplay(String input_Str,int y_position) {
  byte charCount, total_charWidth, x_position;
  input_Str.toCharArray(CharBuf, MaxStringLength + 1);  //string to char array
  charCount = input_Str.length();
  if (charCount == 0) exit;
  total_charWidth = 0;
  for (byte thisChar = 0; thisChar < charCount; thisChar++) {
    total_charWidth = total_charWidth + dmd.charWidth(CharBuf[thisChar]) + 1;  //add 1 pixel for space
  }
  total_charWidth = total_charWidth - 1;            //no space for last letter
  x_position = (PanelWidth - total_charWidth) / 2;  //position(x) of first letter
  for (byte thisChar = 0; thisChar < charCount; thisChar++) {
    //dmd.drawChar(x, y,‘@', GRAPHICS_NORMAL)
    dmd.drawChar(x_position, y_position, CharBuf[thisChar], GRAPHICS_NORMAL);
    x_position = x_position + dmd.charWidth(CharBuf[thisChar]) + 1;  //position for next letter
  }
}

void setup() {
  //Initialise Wire for RTC and Serial for debugging
  Wire.begin();
  Serial.begin(9600);

  //Inform user how to set the time
  Serial.println("To set the time, enter it into the serial monitor in the order YYMMDDwHHMMSS, with an 'x' at the end");
  Serial.println("YY -> Year");
  Serial.println("MM -> Month");
  Serial.println("DD -> Day");
  Serial.println("w -> Day of Week (Sunday is 0)");
  Serial.println("HH -> Hour (24hr)");
  Serial.println("MM -> Minute");
  Serial.println("SS -> Second");

  //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
  Timer1.initialize(4500);          //period in microseconds to call ScanDMD. Anything longer than 4999 (4.999ms) and you can see flicker.
  Timer1.attachInterrupt(ScanDMD);  //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()
  dmd.selectFont(SystemFont5x7);

  //Initial Display draw
  funcRTC();
  dmd.clearScreen(true);
  center_theDisplay(Array[0],0);
  center_theDisplay(Array[1],8);
}

void loop() {
  delay(1000);
  funcRTC();
  if (RTC.getSecond() == 0){
    dmd.clearScreen(true);
    center_theDisplay(Array[0],0);
    center_theDisplay(Array[1],8);
  }
}
