/*
 * FINGERPRINT VERIFICATION & DOOR LOCK SCRIPT 
 * Reads dynamically mapped names from EEPROM memory.
 */

#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const int RELAY_PIN = 8; 
const int LOCK_DELAY = 5000; 
const int NAME_LENGTH = 8; 

// --- EEPROM NAME RETRIEVAL ---
String getUserName(uint8_t id) {
  char nameBuffer[NAME_LENGTH + 1] = {0};
  int eepromAddress = (id - 1) * NAME_LENGTH;
  
  EEPROM.get(eepromAddress, nameBuffer);
  
  // Check if EEPROM is empty at this location (reads as 255/0xFF)
  if ((byte)nameBuffer[0] == 255 || String(nameBuffer).trim() == "") {
    return "User " + String(id);
  }
  
  return String(nameBuffer);
}

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Active-Low Relay
  
  lcd.setCursor(0,0);
  lcd.print("Smart Door Lock");
  lcd.setCursor(0,1);
  lcd.print("Initializing...");

  finger.begin(57600);
  delay(100);
  
  if (finger.verifyPassword()) {
    lcd.clear();
    lcd.print("System Ready!");
    delay(1000);
  } else {
    lcd.clear();
    lcd.print("Sensor Error!");
    while (1) { delay(1); }
  }
}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("System Locked   ");
  lcd.setCursor(0,1);
  lcd.print("Scan Finger...  ");
  
  getFingerprintID();
  delay(50); 
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return p;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return p;
  
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    String name = getUserName(finger.fingerID);
    unlockDoor(name);
  } else if (p == FINGERPRINT_NOTFOUND) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Access Denied!");
    lcd.setCursor(0,1);
    lcd.print("Unknown Finger");
    delay(2000);
    lcd.clear();
  } 
  
  return finger.fingerID;
}

void unlockDoor(String userName) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome,");
  lcd.setCursor(0,1);
  lcd.print(userName);
  
  digitalWrite(RELAY_PIN, LOW); // Unlock
  
  delay(LOCK_DELAY); 
  
  digitalWrite(RELAY_PIN, HIGH); // Lock
  lcd.clear();
}
