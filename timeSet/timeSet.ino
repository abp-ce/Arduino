#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // http://arduiniana.org/libraries/streaming/

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  // setSyncProvider() causes the Time library to synchronize with the
  // external RTC by calling RTC.get() every five minutes by default.
  setSyncProvider(RTC.get);
  setSyncInterval(900);
  Serial << F("RTC Sync");
  if (timeStatus() != timeSet) Serial << F(" FAIL!");
  Serial << endl;
  Serial << "Buffer ";
  while(Serial.available()) Serial << Serial.read();
  Serial << endl;
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  time_t t;
  // Aging offset
  Serial << RTC.readRTC(0x10) << endl;
  
  t = now();
  Serial << t << " " << weekday(t) << ' ' << day(t) << '-' << month(t) << '-' << year(t) << ' ' << hour(t) << ':' << minute(t) << ':' << second(t) << endl;
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == '$') {
      time_t st;
      st = Serial.parseInt();
      RTC.set(st);
      Serial << (st-t) << endl;
      setTime(st);
      // Aging offset
      RTC.writeRTC(0x10,0b01111111);
      Serial << weekday(st) << ' ' << day(st) << '-' << month(st) << '-' << year(st) << ' ' << hour(st) << ':' << minute(st) << ':' << second(st) << endl;
      delay(5000);
    }
    Serial << "Buffer ";
    while(Serial.available()) Serial << Serial.read();
    Serial << endl;
  }
}

