#include <Arduino.h>
#include<SPIMemory.h>
#include <Streaming.h>

SPIFlash flash;
uint32_t maxPage, capacity, addr;
const size_t bs = 256;
uint8_t buff[bs];

void setup() {
 Serial.begin(115200); //,SERIAL_8E1);
 flash.begin();
 maxPage = flash.getMaxPage(); 
 capacity = flash.getCapacity();
}

void loop() {
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == 't') {
      while (!Serial.available())
      {
        delay(1);
      }
      size_t br = Serial.readBytes(buff,bs);
      Serial << br << " bytes read" << endl;
      Serial.write(buff, bs);
    }
    else if (ch == 'i')
    {
      uint32_t JEDEC = flash.getJEDECID();
      Serial << "JEDEC ID: " << JEDEC << endl;
      uint8_t b = (JEDEC >> 8);
      Serial << "Manufacturer ID: " << flash.getManID() << " Memory Type: " << b << " Capacity: " << capacity << endl;
      Serial << "Max page: " << maxPage << endl;
    }
    else if (ch == 'r')
    {
      Serial << maxPage << endl;
      Serial << capacity << endl;
      for (size_t i = 0; i < maxPage; i++)
      {
        addr=i*(uint32_t)bs;
        flash.readByteArray(addr, buff, bs);
        Serial.write(buff, bs);
      }
    }
    else if (ch == 'e')
    {
      if (flash.eraseChip()) { Serial << "Chip erased" << endl; }
      else { Serial << "Chip erase failed" << endl; }
    }
    else if (ch == 'v')
    {
      size_t a=1024;
      uint32_t b;
      byte bff[1024];
      Serial << maxPage << endl;
      Serial << capacity << endl;
      while (!Serial.available()) { }
      b = Serial.readString().toInt();
      addr = a*b;
      char str[16];
      sprintf(str,"%lu",addr);
      Serial << "Address: " << str << endl;
      while (!Serial.available()) {}
      Serial << Serial.readBytes(bff, a) << " bytes read" << endl;
      flash.eraseSection(addr,(uint32_t)a);
      Serial << "Section erased" << endl;
      bool ret = flash.writeByteArray(addr, bff, a);
      if (ret) Serial << "Write finished" << endl;
      else Serial << "Terminated " << endl;
    }
    else if (ch == 'w')
    {
      bool ret;
      Serial << maxPage << endl;
      Serial << capacity << endl;
      for (size_t i = 0; i < maxPage; i++)
      {
        addr=i*(uint32_t)bs;
        while (!Serial.available()) {}
        if (Serial.readBytes(buff, bs) != bs) {
          Serial << "Read bytes error" << endl;
          ret = false;
          break;
        }
        //flash.eraseSection(addr,bs);
        ret = flash.writeByteArray(addr, buff, bs);
        if (!ret)
        {
          Serial << "Terminated " << i << endl;
          break;
        }
      }
      if (ret) Serial << "Write finished" << endl;
    }
    else if (ch == 'c')
    {
      size_t a=2048;
      uint32_t b;
      while (!Serial.available()) { }
      b = Serial.readString().toInt();
      addr = a*b;
      char str[16];
      sprintf(str,"%lu",addr);
      Serial << "Address: " << str << endl;
      flash.readByteArray(addr, buff, a);
      Serial.write(buff, a);
      delay(5);
      flash.readByteArray(addr, buff, a);
      Serial.write(buff, a);
    }
    
    Serial << "Character read: " << ch << endl;
  }
}