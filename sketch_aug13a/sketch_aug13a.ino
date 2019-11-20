#include <DHT.h>
#define DHTPIN1 2
#define DHTPIN2 3

DHT dht1(DHTPIN1, DHT22);
DHT dht2(DHTPIN2, DHT22);
int ledPin = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht1.begin();
  //dht2.begin();
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(3000);
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();
  //float h2 = dht2.readHumidity();
  //float t2 = dht2.readTemperature();
  if (isnan(h1) || isnan(t1))
  {
    Serial.println("DHT1:Reading error!");
    return;
  }
  //if (isnan(h2) || isnan(t2))
  //{
  //  Serial.println("DHT2:Reading error!");
  //  return;
  //}
  Serial.print("DHT1 Humidity: ");
  Serial.print(h1);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t1);
  Serial.println(" *C");

  //Serial.print("DHT2 Humidity: ");
  //Serial.print(h2);
  //Serial.print(" %\t");
  //Serial.print("Temperature: ");
  //Serial.print(t2);
  //Serial.println(" *C");
}
