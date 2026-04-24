/*
 * FINGERPRINT ADMIN PANEL (ENROLL & DELETE WITH EEPROM)
 * Use the Serial Monitor to manage fingerprints and names.
 */

#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const int NAME_LENGTH = 8; // Max characters for a name
uint8_t id;

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Admin Mode");
  
  while (!Serial);  
  delay(100);

  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Sensor Found!");
  } else {
    lcd.clear();
    lcd.print("Sensor Error!");
    while (1) { delay(1); }
  }
}

void loop() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Admin Panel");
  lcd.setCursor(0,1);
  lcd.print("Check PC Serial");
  
  Serial.println("\n--- FINGERPRINT ADMIN PANEL ---");
  Serial.println("Type '1' to ENROLL a new fingerprint");
  Serial.println("Type '2' to DELETE an existing fingerprint");
  
  while (!Serial.available());
  int choice = Serial.parseInt();
  
  if (choice == 1) {
    enrollProcess();
  } else if (choice == 2) {
    deleteProcess();
  }
}

void enrollProcess() {
  Serial.println("\n-- ENROLLMENT --");
  Serial.println("Enter ID # (1 to 127) for this user:");
  
  while (!Serial.available());
  id = Serial.parseInt();
  if (id == 0 || id > 127) {
    Serial.println("Invalid ID. Aborting.");
    return;
  }

  // Clear serial buffer
  while(Serial.available()) Serial.read();

  Serial.println("Enter Name (Max 8 characters):");
  while (!Serial.available());
  String userName = Serial.readStringUntil('\n');
  userName.trim(); 
  
  // Format the string to exactly 8 chars
  char nameBuffer[NAME_LENGTH + 1] = {0}; 
  userName.toCharArray(nameBuffer, NAME_LENGTH + 1);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enrolling: ");
  lcd.print(id);
  lcd.setCursor(0,1);
  lcd.print(nameBuffer);
  
  Serial.print("Enrolling ID "); Serial.print(id); 
  Serial.print(" as "); Serial.println(nameBuffer);

  // Attempt to scan and save the fingerprint
  if (getFingerprintEnroll()) {
    // If successful, save the name to EEPROM
    int eepromAddress = (id - 1) * NAME_LENGTH;
    EEPROM.put(eepromAddress, nameBuffer);
    Serial.println("Name saved to EEPROM successfully.");
  }
}

void deleteProcess() {
  Serial.println("\n-- DELETION --");
  Serial.println("Enter ID # (1 to 127) to delete:");
  
  while (!Serial.available());
  id = Serial.parseInt();
  if (id == 0 || id > 127) return;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Deleting ID: ");
  lcd.print(id);

  uint8_t p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprint deleted from Sensor.");
    
    // Wipe name from EEPROM
    char blank[NAME_LENGTH + 1] = "        "; 
    int eepromAddress = (id - 1) * NAME_LENGTH;
    EEPROM.put(eepromAddress, blank);
    
    lcd.setCursor(0,1);
    lcd.print("Deleted!");
    Serial.println("Name wiped from EEPROM.");
  } else {
    Serial.println("Error deleting fingerprint.");
    lcd.setCursor(0,1);
    lcd.print("Error!");
  }
  delay(2000);
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place Finger");
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_OK) {
      lcd.setCursor(0,1);
      lcd.print("Image Taken!   ");
    }
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return p;
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Remove Finger");
  delay(2000);
  
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place Same");
  lcd.setCursor(0,1);
  lcd.print("Finger Again");
  
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) return p;
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Processing...");
  
  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    lcd.setCursor(0,1);
    lcd.print("Mismatch!");
    delay(2000);
    return p;
  }   
  
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Success!");
    lcd.setCursor(0,1);
    lcd.print("ID "); lcd.print(id); lcd.print(" Saved.");
    delay(3000);
  } else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Save Error");
    delay(2000);
  }   
  return true;
}
