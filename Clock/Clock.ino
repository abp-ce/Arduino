#include <Streaming.h>      // http://arduiniana.org/libraries/streaming/
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#include <TimerOne.h>
#include <TimeLib.h>
#include <IRremote.h>

#define CLK_CORRECTION 
#define AJUST_SPAN 615000

const byte PIN_CLK = 3;   // define CLK pin (any digital pin)
const byte PIN_DIO = 4;   // define DIO pin (any digital pin)
const byte PIN_BUZ = 8;

uint16_t lphour=0, lpmin=0, lpsec=0, hi=0, lo=0; /* Loop time */

int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

SevenSegmentExtended      display(PIN_CLK, PIN_DIO);

time_t lasttm=0 /* коррекция */, sectm=0, wtm=0 /* Таймер звонка */, wptm=0 /* Тыймер паузы звонка */;
int mode=0, wh=7 /* Будильник часы */, wm=30 /* Будильник минуты */;
bool wakeOn=false /* Будильник ыключен */, ringOn=false /* Звонок звенит */, wakeMenu=false; 

void setup() {
  Serial.begin(9600);

  pinMode(PIN_BUZ, OUTPUT);
  digitalWrite(PIN_BUZ, LOW);
  //tone(PIN_BUZ, 1047);
  delay(1000);
  digitalWrite(PIN_BUZ, HIGH);
  //noTone(PIN_BUZ);
  
  display.begin();            // initializes the display
  display.setBacklight(30);  // set the brightness to 30 %
  irrecv.enableIRIn(); // Start the receiver

  display.printTime(hi, lo, true);
  
  // Clear in serial buffer
  while(Serial.available()) Serial.read();
}

void loop() {
  time_t t;
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == '$') {
      t = Serial.parseInt();
      lphour=hour(t); lpmin=minute(t); lpsec=second(t); hi=lphour; lo=lpmin;
      setTime(t);
      display.printTime(hi, lo, true);
      Serial << weekday(t) << ' ' << day(t) << '-' << month(t) << '-' << year(t) << ' ' << lphour << ':' << lpmin << ':' << lpsec << endl;
    }
    // Clear in serial buffer
    while(Serial.available()) Serial.read();
  }
  t = now();
  lphour=hour(t); lpmin=minute(t); lpsec=second(t);
  
  if (wakeMenu) { hi=wh; lo=wm;}
  else { hi=lphour; lo=lpmin; }
  
  if (second(t)==0) {
    display.printTime(hi, lo, true);
    Serial << lphour << ':' << lpmin << ':' << lpsec << endl;
  }

  time_t tm = millis();

// Коррекция времени
#ifdef CLK_CORRECTION
  if ((tm-lasttm) > AJUST_SPAN) {
    adjustTime(5);
    lasttm = tm;
  }
#endif

  if ((tm-sectm) > 1000) {
    Serial << lphour << ':' << lpmin << ':' << lpsec << endl;
    sectm = tm;
    // Зуммер вкл./выкл.
    if (wakeOn==true && lphour == wh && lpmin == wm && (tm-wptm) > 800) {
      if (ringOn == false) {
        digitalWrite(PIN_BUZ, LOW);
        ringOn = true;
        wtm = tm;
      }
      else if ((tm-wtm) > 3000) {
        digitalWrite(PIN_BUZ, HIGH);
        ringOn = false;
        wptm = tm;
      }
    }
  }

  if (irrecv.decode(&results)) {
    Serial << _HEX(results.value) << endl;        
    switch (results.value) {
      case 0xFF629D:                           // Arrow Up
        if (wakeMenu) { hi=++wh; lo=wm; }
        else { adjustTime(3600); hi=++lphour; lo=lpmin; }
        break;  
      case 0xFFA857:                           // Arrow Down   
        if (wakeMenu) { hi=--wh; lo=wm; } 
        else { adjustTime(-3600); hi=--lphour; lo=lpmin; }
        break; 
      case 0xFFC23D:                           // Arrow Right
        if (wakeMenu) { hi=wh; lo=++wm; }
        else { adjustTime(60); hi=lphour; lo=++lpmin; }
        break;    
      case 0xFF22DD:                           // Arrow Left
        if (wakeMenu) { hi=wh; lo=--wm; }
        else { adjustTime(-60); hi=lphour; lo=--lpmin; }
        break;
      case 0xFF52AD:                           // #   
        // Выключить зуммер
        if (ringOn == true) { digitalWrite(PIN_BUZ, HIGH); ringOn = false; wakeOn = false; }
        break;
      case 0xFF02FD:                           // OK
        // Переключение будильника 
        if (wakeMenu) {
          if (wakeOn == true) wakeOn = false;
          else {
            wakeOn = true;
            // Подтверждение включения будильника
            digitalWrite(PIN_BUZ, LOW);
            delay(500);
            digitalWrite(PIN_BUZ, HIGH);
          }        
          // Вышли из меню будильника
          hi=lphour; lo=lpmin;
          wakeMenu = false;
        }
        else {
          // Вошли в меню будильника
          hi=wh; lo=wm;
          wakeMenu = true;
        }
                
    }
    
    display.printTime(hi, lo, true);    
    irrecv.resume(); // Receive the next value
  }    

}
