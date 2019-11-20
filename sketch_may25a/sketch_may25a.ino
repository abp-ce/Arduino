#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // http://arduiniana.org/libraries/streaming/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // setSyncProvider() causes the Time library to synchronize with the
  // external RTC by calling RTC.get() every five minutes by default.
  setSyncProvider(RTC.get);
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
  
  t = now();
  if (Serial.available()) {
    char ch = Serial.read();
    Serial << ch << endl;
    if (ch == '$') {
      time_t st;
      st = Serial.parseInt();
      Serial << (st-t) << endl;
    }
  }
}
