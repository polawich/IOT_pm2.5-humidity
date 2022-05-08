#include "DHT.h"
#include <TridentTD_LineNotify.h>
#include <ESP8266WiFi.h>
#define RELAY_PIN 13 //ขา Input Relay
#define DHTPIN 14     // Digital pin connected to the DHT sensor
// Uncomment whatever type you're using!
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

int measurePin = A0; //Pin วัดฝุ่น
int ledPower = D6;    //Pin Power ที่วัดฝุ่น

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

DHT dht(DHTPIN, DHTTYPE); //วัดความชื้น

// ตั้งค่า ชื่อ และ password wifi เพื่อเชื่อมต่ออินเทอร์เน็ต.

#define SSID "Nice_home2.4G" // ชื่อ  wifi ที่จะเชื่อมต่อ  ในส่วนของผู้สอนใช้ wifi ชื่อ Registers
#define PASSWORD "polawich123" // รหัส wifi ที่จะเชื่อมต่อ  ในส่วนของผู้สอนใช้ รหัส wifi  12345678
#define LINE_TOKEN "gaLqXq3EgV8bjtxWuHj9dT4mEBUU8fAOEg9yeO9VoSI" // ใส่ Token ที่ได้มาจากขั้นตอนก่อนหน้า

void setup() {
  Serial.begin(9600);

  //เครื่องวัดฝุ่น
  pinMode(ledPower, OUTPUT);
  
  //Wifi
  Serial.begin(115200); Serial.println();
  Serial.println(LINE.getVersion());
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %sn", SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.println(F("วัดความชื้นใช้งานได้"));
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT); //ขา Relay Output
  //Line
  LINE.setToken(LINE_TOKEN);
  //LINE.notify("ทดสอบส่งข้อความ");
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

  //วัดฝุ่น

  digitalWrite(ledPower, LOW); // เปิด Power เพื่ออ่านค่า
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin); // อ่านค่าฝุ่น
  
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // ปิด Power เพื่ออ่านค่า
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured * (3.3 / 1024);

  dustDensity = 0.17 * calcVoltage - 0.1; //ความหนาเเน่นของฝุ่น

  //Serial.print("Raw Signal Value (0-1023): ");
  //Serial.print(voMeasured);

  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);

  if (dustDensity <= 0.00) {
    dustDensity = 0.00;
  }

  dustDensity = dustDensity * 1000;

  Serial.print(" - Dust Density: ");
  Serial.print(dustDensity);
  Serial.println(" µg/m³");
  delay(50);
  //

  //วัดความชื้น
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
  Serial.println("--------------");
  
  //

  if ((h) >= 92 || (dustDensity) >= 250){ // ถ้าความชื้นเเละควันถึงค่าที่ตั้งไว้ให้ส่งไปที่ไลน์
      Serial.print("High Humidity & High DustDensity");
      //LINE.notify("ส่งข้อความสำเร็จ"); //ส่งข้อความเฉยๆ
      //delay(2000);
      //func Relay Alert
    while (digitalRead(RELAY_PIN) == LOW) {
      digitalWrite(RELAY_PIN, HIGH);
      delay(50);
      digitalWrite(RELAY_PIN, LOW);
      delay(50);
      digitalWrite(RELAY_PIN, HIGH);
      delay(50);
      digitalWrite(RELAY_PIN, LOW);
      delay(50);
      
      Serial.println("");
      Serial.print(F("Humidity: ")); // Humidity
      Serial.print(h);
      Serial.println(F("%"));
      Serial.print(" - Dust Density: ");
      Serial.print(dustDensity);
      Serial.println(" µg/m³");
      if ((h) >= 90 || (dustDensity) >= 200 ){ //ความชื้นต่ำกว่า 91 ให้ออกจาก Loop
        break;
      }     
    }
  }
}
