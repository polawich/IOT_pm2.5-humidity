#include "DHT.h"
#include <TridentTD_LineNotify.h>
#include <ESP8266WiFi.h>
#define RELAY_PIN 13 //ขา Input Relay
#define DHTPIN 14     // Digital pin connected to the DHT sensor
// Uncomment whatever type you're using!
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

// ตั้งค่า ชื่อ และ password wifi เพื่อเชื่อมต่ออินเทอร์เน็ต.

#define SSID "Nice_home2.4G" // ชื่อ  wifi ที่จะเชื่อมต่อ  ในส่วนของผู้สอนใช้ wifi ชื่อ Registers
#define PASSWORD "polawich123" // รหัส wifi ที่จะเชื่อมต่อ  ในส่วนของผู้สอนใช้ รหัส wifi  12345678
#define LINE_TOKEN "gaLqXq3EgV8bjtxWuHj9dT4mEBUU8fAOEg9yeO9VoSI" // ใส่ Token ที่ได้มาจากขั้นตอนก่อนหน้า

void setup() {
  Serial.begin(9600);
  
  Serial.begin(115200); Serial.println();
  Serial.println(LINE.getVersion());
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %sn", SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.println(F("DHTxx test!"));
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT); //ขา Relay Output
  //Line
  LINE.setToken(LINE_TOKEN);
  LINE.notify("ทดสอบส่งข้อความ");
}

void loop() {
  // Wait a few seconds between measurements.
  delay(50);
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  //Serial.println("");
  Serial.print(F("Humidity: ")); // Humidity
  Serial.print(h);
  Serial.println(F("%"));

  if ((h) >= 91){ // ถ้าถึงค่าที่ตั้งไว้ให้ส่งไปที่ไลน์
      Serial.print("High Humidity");
 
      //func Relay Alert
    while (digitalRead(RELAY_PIN) == LOW) {
      digitalWrite(RELAY_PIN, HIGH);
      delay(100);
      digitalWrite(RELAY_PIN, LOW);
      delay(100);
      
      Serial.println("");
      Serial.print(F("Humidity: ")); // Humidity
      Serial.print(h);
      Serial.println(F("%"));
      if ((h) >= 91){ //ความชื้นต่ำกว่า 91 ให้ออกจาก Loop
        break;
      }     
    }
  }
  //LINE.notify("ส่งข้อความสำเร็จ"); //ส่งข้อความเฉยๆ
  //delay(2000);
}
