/*
    Project: WiFi SSID and RSSI monitor

    Description: List near by wifi access points to test connectivity of locations for future IoT projects. Display their SSID and RSSI via a 16x2 LCD screen With interface to control LCD screen.

    Resources:
        https://deepbluembedded.com/esp32-lcd-display-16x2-without-i2c-arduino/#content
        https://www.makerguides.com/interfacing-esp32-and-16x2-lcd-parallel-data-without-i2c/
        https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/#2

*/
#include <LiquidCrystal.h>
#include <WiFi.h>

LiquidCrystal lcd(13, 12, 25, 26, 27, 14);

void setup() {
  Serial.begin(115200);
  
  lcd.begin(16, 2);
  delay(100);
  lcd.clear();
  delay(100);

  Serial.println("LCD On");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Wifi Setup");
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning");
  Serial.println("Scanning");
  int n = WiFi.scanNetworks();
  
  lcd.setCursor(0, 1);
  if(n==0){
    lcd.print("No Networks");
    Serial.print("No networks");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    // for (int i = 0; i < n; ++i) {
    //   // Print SSID and RSSI for each network found
    //   Serial.print(i + 1);
    //   Serial.print(": ");
    //   Serial.print(WiFi.SSID(i));
    //   Serial.print(" (");
    //   Serial.print(WiFi.RSSI(i));
    //   Serial.print(")");
    //   Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
    //   delay(10);
    // }
    String name = WiFi.SSID(0);
    if (name.length() > 6){
      name = WiFi.SSID().substring(0,6);
    }  
    lcd.print(name);
    lcd.print(": ");
    lcd.print(WiFi.RSSI(0));
    delay(10);
  }

  Serial.println("");
  delay(5000);
}