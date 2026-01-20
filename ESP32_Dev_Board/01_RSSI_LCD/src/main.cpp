#include <Arduino.h>
#include <LiquidCrystal.h>
#include <vector>
#include <WiFi.h>

const int aPin = 18;
const int bPin = 4;
int countA = 0;
int aState = 0;
int bState = 0;
int lastAState = LOW;
int lastBState = LOW;

unsigned long lastDebounceA = 0;
unsigned long lastDebounceB = 0;
unsigned long debounceDelay = 50;

int start = 0;
int idx = 0;
int pos = 0;
int n = 0; 

std::vector<String> screen({"", ""});
LiquidCrystal lcd(33, 13, 25, 26, 27, 14); 

void startImage() {
    lcd.clear();

    byte image23[8] = {B00001, B00111, B01000, B10000, B10000, B10000, B01000, B00111};
    byte image24[8] = {B00000, B11111, B00000, B00000, B11111, B00000, B00000, B11111};
    byte image25[8] = {B00000, B11111, B00000, B00000, B00101, B00000, B00000, B11111};
    byte image26[8] = {B10000, B11100, B00010, B00001, B10001, B00001, B00010, B11100};
    byte image07[8] = {B00000, B00000, B00001, B00001, B00001, B00001, B00001, B00001};
    byte image10[8] = {B00000, B00000, B10000, B10000, B10000, B10000, B10000, B10000};
    byte image08[8] = {B00000, B00000, B00111, B01000, B10000, B00011, B00100, B00000};
    byte image09[8] = {B00000, B00000, B11100, B00010, B00001, B11000, B00100, B00000};

    lcd.createChar(0, image23);
    lcd.createChar(1, image24);
    lcd.createChar(2, image25);
    lcd.createChar(3, image26);
    lcd.createChar(4, image07);
    lcd.createChar(5, image10);
    lcd.createChar(6, image08);
    lcd.createChar(7, image09);

    lcd.setCursor(6, 1);
    lcd.write(byte(0));
    lcd.setCursor(7, 1);
    lcd.write(byte(1));
    lcd.setCursor(8, 1);
    lcd.write(byte(2));
    lcd.setCursor(9, 1);
    lcd.write(byte(3));
    lcd.setCursor(6, 0);
    lcd.write(byte(4));
    lcd.setCursor(9, 0);
    lcd.write(byte(5));
    lcd.setCursor(7, 0);
    lcd.write(byte(6));
    lcd.setCursor(8, 0);
    lcd.write(byte(7));
}

void arrowImage(int col, int row) {
    byte image01[8] = {B00000, B00000, B01000, B01100, B01110, B01100, B01000, B00000};
    lcd.createChar(0, image01);
    lcd.setCursor(col, row);
    lcd.write(byte(0));
}

void listNetwork(int row, int i){
    if (i >= n) {
        return;
    }

    lcd.setCursor(0, row);

    screen[row] = " ";
    screen[row] += WiFi.SSID(i);
    
    if (screen[row].length() > 13){
        screen[row] = screen[row].substring(0,13);
    } else {
        while (screen[row].length() < 13) {
            screen[row] += " ";
        }
    }

    screen[row] += WiFi.RSSI(i);  

    lcd.print(screen[row]);
    
    delay(10);
}

void scrollDown() {
    if (idx + 1 >= n) {
        return;
    }

    if (pos == 0) {
        lcd.setCursor(0, 0);
        lcd.print(" ");
        arrowImage(0, 1);
        pos = 1;
        return;
    }

    idx++;
    
    lcd.setCursor(0, 1);
    lcd.print(" ");
    
    listNetwork(0, idx);
    arrowImage(0, 0);
    
    if (idx + 1 < n) {
        listNetwork(1, idx + 1);
    } else {
        lcd.setCursor(1, 1);
        lcd.print("               ");
    }
    
    pos = 0;
}

void scan(){
    lcd.clear();
    lcd.setCursor(0, 0);

    screen[0] = "Networks:"; 
    lcd.print(screen[0]);

    n = WiFi.scanNetworks();

    Serial.println("\n=== WiFi Scan Result ===");
    Serial.print("Networks found: ");
    Serial.println(n);
    
    for (int i = 0; i < n; i++) {
        Serial.print(i);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(" dBm) ");
        Serial.print("Channel: ");
        Serial.print(WiFi.channel(i));
        Serial.print(" BSSID: ");
        Serial.println(WiFi.BSSIDstr(i));
    }
    Serial.println("======================\n");

    lcd.setCursor(0, 1);
    if (n == 0){
        screen[1] = "No Networks..."; 
        lcd.print(screen[1]);
        return;
    }
    
    idx = 0;
    pos = 1;
    listNetwork(1, idx);
    lcd.setCursor(0,1);
    arrowImage(0,1);
}

void setup() {
    Serial.begin(115200);

    lcd.begin(16,2);
    lcd.clear();

    pinMode(aPin, INPUT);
    pinMode(bPin, INPUT);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
}

void loop() {
    if (start == 0){
        delay(2000);
        startImage();
        delay(5000);
        scan();
        start = 1;
    }

    int readingA = digitalRead(aPin);
    int readingB = digitalRead(bPin);

    if (readingA != lastAState) {
        lastDebounceA = millis();
    }

    if (readingB != lastBState) {
        lastDebounceB = millis();
    }

    if ((millis() - lastDebounceA) > debounceDelay) {
        if (readingA == HIGH && aState == LOW) {
            scrollDown();
        }
        aState = readingA;
    }

    if ((millis() - lastDebounceB) > debounceDelay) {
        if (readingB == HIGH && bState == LOW) {
            scan();
        }
        bState = readingB;
    }

    lastAState = readingA;
    lastBState = readingB;
}