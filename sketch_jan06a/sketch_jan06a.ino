#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Ticker.h>
//#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>

const char* ssid = "ABP_Network";
const char* password = "xk5y-604e-h6cl";
const char* host[2] = { "api.openweathermap.org", "narodmon.ru"};
String line[2]; 
bool isTickerSet = true;
bool isSecond = true;
bool isAster = true;
int hst = 1;

Ticker tickerRequest;
Ticker secondRequest;
WiFiServer server(23);


LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x3F

void setTicker() {
  isTickerSet = true;
}

void setSecond() {
  isSecond = true;
}

void setup() {
  Wire.begin(12,13); //9 as SDA and 10 as SCL
  lcd.begin(12,13);  // sda=0, scl=2
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Connecting to ");
  lcd.setCursor(0, 1);

  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);

  // put your setup code here, to run once:
  Serial.begin (115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  tickerRequest.attach(300,setTicker);
  secondRequest.attach(1,setSecond);
  
  // ждем соединения:
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");  //  "Подключились к "
  Serial.println(ssid);
  Serial.print("IP address: ");  //  "IP-адрес: "
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected to");
  lcd.setCursor(0, 1);
  lcd.print("IP ");
  lcd.println(WiFi.localIP());

  server.begin();
  Serial.println("Server started");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Server started");

  Serial.println("End of setup"); 
}

void weatherRequest() {
  WiFiClient client;
  char *buf[2] = { "GET /data/2.5/weather?id=515879&units=metric&lang=ru&appid=a1eaeeb1587b0f8f19fa1f8e33d404be HTTP/1.1",
    "GET /api/sensorsOnDevice?id=1222&uuid=b7ded2ab8ea69e6b2ed8ec240c93ec47&api_key=pZ6sg0Ew6rxkD&lang=en HTTP/1.1"};
  char *buf1[2] = { "Host: api.openweathermap.org", "Host: narodmon.ru"};
  char *usr = "User-Agent: ABP_esp8266";
  Serial.println("Start of weatherRequest");
  Serial.printf("\n[Connecting to %s ... ", host[hst]);
  if (client.connect(host[hst], 80))
  {
    Serial.println("connected]");

    Serial.println("[Sending a request]");
    Serial.println(buf[hst]);
    client.println(buf[hst]);
    if ( hst == 1 ) {
      Serial.println(usr);
      client.println(usr);      
    }
    Serial.println(buf1[hst]);
    client.println(buf1[hst]);
    client.println("Connection: close");
    client.println();

    delay(1500);
    Serial.println("[Response:]");

    while (client.connected())
    {
      if (client.available())
      {
        line[hst] = client.readStringUntil('\r');
      }
      Serial.println(line[hst]);
    }
    client.stop();
    Serial.println("\n[Disconnected]");
  }
  else
  {
    Serial.println("connection failed!]");
    client.stop();
  }  
}

void loop() {

  if ( isTickerSet ) {
    weatherRequest();
    isTickerSet = false;
    parseWeather();
  }
  
  if ( isSecond ) {
    lcd.setCursor(15, 1);
    if ( isAster ) {
      lcd.print("*");
      isAster = false; 
    }
    else {
      lcd.print(" ");
      isAster = true;
    }
    isSecond = false;
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  int cn = 0;
  while(!client.available()){
    delay(1);
    cn = cn + 1;
    if ( cn == 15000 ) break;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  client.println(req);
  Serial.println(req);
  if ( req == "Check" ) {
    if ( hst == 0 ) hst = 1;
    else hst = 0;
    lcd.clear();
    lcd.print("Client send:");
    lcd.setCursor(0, 1);
    lcd.print(req);
    delay(15000);
    weatherRequest();
    parseWeather();
    client.flush();
  } 
}

void parseWeather() {
   StaticJsonBuffer<2000> jsonBuffer;                   /// буфер на 2000 символов
   JsonObject& root = jsonBuffer.parseObject(line[hst]);     // скармиваем String
   if (!root.success()) {
    Serial.println("parseObject() failed");             // если ошибка, сообщаем об этом
    return;                                             // и запускаем заного 
  }
  
  
  lcd.clear();
  Serial.println();  
/*  String name = root["name"];                           // достаем имя, 
  Serial.print("name:");
  Serial.println(name); */

  int pressurehPa, humidity, winddeg;
  float tempC, pressure, windspeed;

  if ( hst == 0 ) {
    tempC = root["main"]["temp"];
    pressurehPa = root["main"]["pressure"]; 
    pressure = pressurehPa/1.333;
    humidity = root["main"]["humidity"]; 
    windspeed = root["wind"]["speed"]; 
    winddeg = root["wind"]["deg"]; 
  }
  else {
    tempC = root["sensors"][0]["value"];
    pressure = root["sensors"][2]["value"]; 
    humidity = root["sensors"][1]["value"];     
  }
  
  Serial.print("temp: ");
  Serial.print(tempC);                                  // отправляем значение в сериал
  Serial.println(" C");
  lcd.setCursor(0, 0);
  lcd.print(tempC);
  lcd.print("C");

  Serial.print("pressure: ");
  Serial.print(pressure);
  Serial.println(" mmHc");
  lcd.print(" ");
  lcd.print(pressure);
  lcd.print("mmHc");

  Serial.print("humidity: ");
  Serial.print(humidity);  
  Serial.println(" %");
  lcd.setCursor(0, 1);
  lcd.print(humidity);
  lcd.print("%");

  if ( hst == 0) {
    Serial.print("wind speed: ");
    Serial.print(windspeed);  
    Serial.println(" m/s");
    lcd.print(" ");
    lcd.print(windspeed);
    lcd.print("m/s");

    Serial.print("wind deg :");
    Serial.println(winddeg);
    lcd.print(" ");
    if ( windspeed <= 10 ) lcd.print("N");
    else if ( windspeed <= 80 ) lcd.print("N-W");
         else if ( windspeed <= 100 ) lcd.print("W");
              else if ( windspeed <= 170 ) lcd.print("S-W");
                   else if ( windspeed <= 190 ) lcd.print("S");
                        else if ( windspeed <= 260 ) lcd.print("S-E");
                             else if ( windspeed <= 280 ) lcd.print("E");
                                  else if ( windspeed <= 350 ) lcd.print("N-E");
                                       else if ( windspeed <= 360 ) lcd.print("N");
  }  
 

  Serial.println();  
  Serial.println();  
}

