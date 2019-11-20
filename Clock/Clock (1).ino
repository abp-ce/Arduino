#include <Streaming.h>      // http://arduiniana.org/libraries/streaming/
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#include <TimerOne.h>
#include <TimeLib.h>
#include <IRremote.h>

#define CLK_CORRECTION
/* initialize global TM1637 Display object
*  The constructor takes two arguments, the number of the clock pin and the digital output pin:
* SevenSegmentTM1637(byte pinCLK, byte pinDIO);
*/
const byte PIN_CLK = 3;   // define CLK pin (any digital pin)
const byte PIN_DIO = 4;   // define DIO pin (any digital pin)

int RECV_PIN = 9;
IRrecv irrecv(RECV_PIN);
decode_results results;

SevenSegmentExtended      display(PIN_CLK, PIN_DIO);

time_t lasttm=0, sectm=0;
int mode=0;

void setup() {
  Serial.begin(9600);
  
  display.begin();            // initializes the display
  display.setBacklight(30);  // set the brightness to 30 %
  irrecv.enableIRIn(); // Start the receiver
  
  // Clear in serial buffer
  while(Serial.available()) Serial.read();
}

void loop() {
  time_t t;
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == '$') {
      t = Serial.parseInt();
      setTime(t);
      display.printTime(hour(t), minute(t), true);
      Serial << weekday(t) << ' ' << day(t) << '-' << month(t) << '-' << year(t) << ' ' << hour(t) << ':' << minute(t) << ':' << second(t) << endl;
    }
    // Clear in serial buffer
    while(Serial.available()) Serial.read();
  }
  t = now();
  if (second(t)==0) {
    display.printTime(hour(t), minute(t), true);
    Serial << hour(t) << ':' << minute(t) << ':' << second(t) << endl;
  }
  time_t tm = millis();

#ifdef CLK_CORRECTION
  if ((tm-lasttm) > 600000) {
    adjustTime(5);
    lasttm = tm;
  }
#endif

  if ((tm-sectm) > 1000) {
    Serial << hour(t) << ':' << minute(t) << ':' << second(t) << endl;
    sectm = tm;
  }
  if (irrecv.decode(&results)) {
    Serial << _HEX(results.value) << endl;
    switch (results.value) {
      case 0xFF629D: adjustTime(3600); break;  // Arrow Right
      case 0xFFA857: adjustTime(-3600); break; // Arrow Left
      case 0xFFC23D: adjustTime(60); break;    // Arrow Up
      case 0xFF22DD: adjustTime(-60); break;   // Arrow Down
      case 0xFF02FD:                           // OK
        display.printTime(0, 0, true);
        results.value = 0x000000;
        do {
          irrecv.resume();
          delay(600);
          irrecv.decode(&results);
          Serial << _HEX(results.value) << endl;
          if (results.value == 0xFF02FD) break;
        } while (true);
    }
    t = now();
    display.printTime(hour(t), minute(t), true);
    irrecv.resume(); // Receive the next value
  }    
}
