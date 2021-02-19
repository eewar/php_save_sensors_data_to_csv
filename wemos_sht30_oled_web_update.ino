#include <SPI.h>
#include <Wire.h>
#include <WEMOS_SHT3X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>


// unit var
const char* UID = "u0000";
const char* ssid = "xxxxxxxxxx";
const char* password = "xxxxxxxxxx";

const char* proxy_server = "http://192.168.2.11/iot_to_csv.php?data=";
const long signal_interval = 60000;

unsigned long previous_millis = 0;


// oled var
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// sht30 var
#define I2C_SHT30_ADDRESS 0x45
SHT3X sht30(I2C_SHT30_ADDRESS);


String generateV() {
  String v;
  v += sht30.cTemp;
  v += ",";   
  v += sht30.fTemp;
  v += ",";   
  v += sht30.humidity;  
  return v;
}


String generateJSON() {
  unsigned long uptime = millis() / 1000;

  String v = generateV();
  String json;
  json += "{";
  json += "\"uid\":\"" + String(UID) + "\",";
  json += "\"up\":" + String(uptime) + ",";
  json += "\"v\":[" + v + "]";
  json += "}";

  return json;
}


void setup () {
  Serial.begin(115200);
  delay(500);


  if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 shield failed"));
    for(;;);
  }
  display.setTextColor(SSD1306_WHITE);


  startup_screen();

  // OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Connecting to Wi-Fi"));
  display.println(ssid);
  display.display();
  
  // Serial
  Serial.println("");
  Serial.println(F("Connecting to Wi-Fi"));
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

    // OLED
    display.print(F(". "));
    display.display();
    
    // Serial
    Serial.print(F(". "));
  }

  // enabled dns
  if (!MDNS.begin(UID)) {
    while (1) {
      delay(1000);
    }
  }
  MDNS.addService("http", "tcp", 80);


  // OLED
  display.println(F(""));
  display.println(F("Connected!"));
  display.print(F("IP : "));
  display.println(WiFi.localIP());
  display.display();
  
  // Serial
  Serial.println("");
  Serial.println(F("Connected!"));
  Serial.print(F("IP : "));
  Serial.println(WiFi.localIP());

  delay(1000);
}


void loop() {
  MDNS.update();

  unsigned long current_millis = millis();
  if (current_millis - previous_millis >= signal_interval) {
    previous_millis = current_millis;
  
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      
      if(sht30.get() == 0){
        // OLED
        display.print(sht30.cTemp);
        display.println(F(" C"));
    
        display.print(sht30.fTemp);
        display.println(F(" F"));
        
        display.print(sht30.humidity);
        display.println(F(" %"));
        display.display();
      
        // Serial
        Serial.println(F("SHT30 Data"));
        Serial.print(sht30.cTemp);
        Serial.println(F(" C"));
        Serial.print(sht30.fTemp);
        Serial.println(F(" F"));
        Serial.print(sht30.humidity);
        Serial.println(F(" %"));
        Serial.println();
      } else {
        // OLED
        display.println(F("SHT30 Error!"));
        display.display();
            
        // Serial
        Serial.println(F("SHT30 Error!"));
      }
    
      
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        
        String url_string = proxy_server;
        url_string +=  generateJSON();
          
        http.begin(url_string);
        int httpCode = http.GET();
    
        // OLED
        display.setTextSize(1);
        display.print(F("HTTP : "));
        display.println(httpCode);
        display.display();
        
        // Serial
        Serial.print(F("HTTP : "));
        Serial.println(httpCode);

          if (httpCode > 0) {
            String result = http.getString();
            
            // OLED
            display.println(result);
            display.display();
            
            // Serial
            Serial.println(result);
          }
          http.end();
        
      } else {
        // OLED
        display.setTextSize(1);
        display.println(F("Wi-Fi not connected!"));
        display.display();
        
        // Serial
        Serial.println(F("Wi-Fi not connected!"));
      }
      Serial.println("");    
  }
}


void startup_screen(void) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(F("Sweet"));
  display.println(F("Green"));
  display.println(F("Tech"));
  display.setTextSize(1);
  display.print(F("UID : "));
  display.println(UID);
  display.display();
  delay(2000);
}
