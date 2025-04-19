#define BLYNK_TEMPLATE_ID "TMPL3FEhiHLFH"
#define BLYNK_TEMPLATE_NAME "TEMPERATURE MONITORING"
#define BLYNK_AUTH_TOKEN "GDf9UGC3obHPWVwho2A7AvUAnfwBOVR5"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi Credentials
char ssid[] = "redmi";     
char pass[] = "12345678";  

// DHT Sensor Settings
#define DHT_PIN D4  
#define DHT_TYPE DHT11  
DHT dht(DHT_PIN, DHT_TYPE);

// Peltier Module (Relay) Settings
#define RELAY_PIN1 D7  
#define RELAY_PIN2 D6  

// LCD Setup (I2C Address 0x27, 16 cols, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;  

void sendSensor() {
  float temp = dht.readTemperature();  

  if (isnan(temp)) {
    Serial.println("DHT sensor error!");
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!  ");
    return;
  }

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("°C | Peltier 1: ");

  bool peltier1 = false;
  bool peltier2 = false;

  if (temp > 38.0) {
    digitalWrite(RELAY_PIN1, LOW);  
    peltier1 = true;
    Serial.print("ON ");
  } else {
    digitalWrite(RELAY_PIN1, HIGH);  
    Serial.print("OFF ");
  }

  Serial.print("| Peltier 2: ");
  if (temp < 34.0) {
    digitalWrite(RELAY_PIN2, LOW);  
    peltier2 = true;
    Serial.println("ON");
  } else {
    digitalWrite(RELAY_PIN2, HIGH);  
    Serial.println("OFF");
  }

  Blynk.virtualWrite(V0, temp);

  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("cool:");
  lcd.print(peltier1 ? "1" : "0");
  lcd.print(" heat:");
  lcd.print(peltier2 ? "1" : "0");
}

void checkConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.disconnect();
    WiFi.begin(ssid, pass);
  }
  if (!Blynk.connected()) {
    Serial.println("Blynk disconnected! Reconnecting...");
    Blynk.connect();
  }
}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);

  dht.begin();
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
  digitalWrite(RELAY_PIN1, HIGH);  
  digitalWrite(RELAY_PIN2, HIGH);  

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System initialize");
  delay(2000);
  lcd.clear();

  Serial.println("Dual Peltier Control with LCD & Blynk");

  timer.setInterval(2000L, sendSensor);    
  timer.setInterval(10000L, checkConnection);
}

void loop() {
  if (Blynk.connected()) {
    Blynk.run();
  }
  timer.run();
}
