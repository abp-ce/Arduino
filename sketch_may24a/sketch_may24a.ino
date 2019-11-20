#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // http://arduiniana.org/libraries/streaming/
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"

/* initialize global TM1637 Display object
*  The constructor takes two arguments, the number of the clock pin and the digital output pin:
* SevenSegmentTM1637(byte pinCLK, byte pinDIO);
*/
const byte PIN_CLK = 3;   // define CLK pin (any digital pin)
const byte PIN_DIO = 4;   // define DIO pin (any digital pin)
SevenSegmentExtended      display(PIN_CLK, PIN_DIO);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // setSyncProvider() causes the Time library to synchronize with the
  // external RTC by calling RTC.get() every five minutes by default.
  setSyncProvider(RTC.get);
  setSyncInterval(900);
  // Serial << F("RTC Sync");
  display.begin();            // initializes the display
  display.setBacklight(30);  // set the brightness to 100 %
  // Clear in serial buffer
  while(Serial.available()) Serial.read();
}

void loop() {
  time_t t = now();
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == '$') {
      t = Serial.parseInt();
      RTC.set(t);
      // Aging offset
      //RTC.writeRTC(0x10,0b00000000);
      //Serial << RTC.readRTC(0x10) << endl;
      Serial << weekday(t) << ' ' << day(t) << '-' << month(t) << '-' << year(t) << ' ' << hour(t) << ':' << minute(t) << ':' << second(t) << endl;
    }
    // Clear in serial buffer
    while(Serial.available()) Serial.read();
  }
  display.printTime(hour(t), minute(t), true);
  Serial << hour(t) << ':' << minute(t) << ':' << second(t) << endl;
  delay(1000);
}
