#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int tempPin = A0;
const int levelPin = A1;
const int flowPin  = A2;
const int leakPin  = 7;

const int deviceOnPin = 8;
const int resetPin    = A3;

const int loc1Pin = 6;   // زر 1 = Operation
const int loc2Pin = 13;  // زر 2 = Kids

const int ledPin   = 9;
const int buzzerPin = 10;

String deviceID = "A01";
int errorCount = 0;
unsigned long lastAlarm = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  pinMode(leakPin, INPUT_PULLUP);
  pinMode(deviceOnPin, INPUT_PULLUP);
  pinMode(resetPin, INPUT_PULLUP);

  pinMode(loc1Pin, INPUT_PULLUP);
  pinMode(loc2Pin, INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.setCursor(0,0);
  lcd.print("Class A System");
  delay(1500);
}

void loop() {

  int tempRaw = analogRead(tempPin);
  float voltage = tempRaw * (5.0 / 1023.0);
  float tempC = (voltage - 0.5) * 100.0;

  int waterLevel = analogRead(levelPin);
  int waterFlow = analogRead(flowPin);

  bool leak = (digitalRead(leakPin) == LOW);
  bool deviceOn = (digitalRead(deviceOnPin) == LOW);

  int sw1 = digitalRead(loc1Pin);  
  int sw2 = digitalRead(loc2Pin);

  int locCode = 0;
  if (sw1 == LOW) locCode += 1;
  if (sw2 == LOW) locCode += 2;

  String location = "Unknown";
  if(locCode == 1) location = "Operation";
  if(locCode == 2) location = "Kids";
  if(locCode == 3) location = "ICU";

  bool alarm = false;
  if (tempC > 50) alarm = true;
  if (waterLevel < 200) alarm = true;
  if (waterFlow < 100) alarm = true;
  if (leak) alarm = true;

  if (alarm) {
    errorCount++;
    lastAlarm = millis();
  }

  if (digitalRead(resetPin) == LOW) {
    errorCount = 0;
  }

  int health = 100 - (errorCount * 2);
  if (health < 0) health = 0;

  // -----------------------------
  // 🔥 نغمة إنذار متغيّرة حسب نوع المشكلة
  // -----------------------------
  if (alarm) {

    digitalWrite(ledPin, HIGH);

    // حرارة عالية
    if (tempC > 50) {
      tone(buzzerPin, 1200, 150);
      delay(150);
    }

    // تسرب ماء
    if (leak) {
      tone(buzzerPin, 800, 80);
      delay(80);
      noTone(buzzerPin);
      delay(80);
    }

    // تدفق منخفض
    if (waterFlow < 100) {
      tone(buzzerPin, 600, 300);
      delay(300);
    }

    // مستوى ماء منخفض
    if (waterLevel < 200) {
      tone(buzzerPin, 400, 500);
      delay(500);
    }

  } else {
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
  }

  // -----------------------------
  // شاشات العرض
  // -----------------------------

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(tempC);
  lcd.print(" F:");
  lcd.print(waterFlow);

  lcd.setCursor(0,1);
  lcd.print("Lvl:");
  lcd.print(waterLevel);
  lcd.print(" Lk:");
  lcd.print(leak ? "Y" : "N");

  delay(400);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ID:");
  lcd.print(deviceID);
  lcd.print(" ");
  lcd.print(location);

  lcd.setCursor(0,1);
  lcd.print("Err:");
  lcd.print(errorCount);
  lcd.print(" H:");
  lcd.print(health);
  lcd.print("%");

  delay(800);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Device:");
  lcd.print(deviceOn ? "ON " : "OFF");

  lcd.setCursor(0,1);
  lcd.print("Status:");
  lcd.print(alarm ? "ALARM" : "OK");

  delay(800);
}
