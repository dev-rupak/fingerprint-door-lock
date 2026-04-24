# 🔐 Smart Biometric Door Lock with UI

A comprehensive smart security system featuring biometric authentication, power-isolated mechanical actuation, and an interactive LCD user interface. The system interfaces an Arduino Uno with an R307 fingerprint sensor, an I2C 16x2 LCD display, and a 12V solenoid lock controlled via a relay.

## 🚀 Key Features
* **Biometric Authentication:** High-accuracy optical fingerprint scanning via the R307 module.
* **Interactive UI:** A 16x2 I2C LCD guides users through the enrollment process and displays real-time lock status.
* **Dynamic Identity Mapping:** The firmware automatically maps raw numeric fingerprint IDs to string-based user names (e.g., "Welcome, Aditi"), providing a personalized access experience.
* **Isolated Power Control:** Utilizes a relay module to safely isolate the Arduino's 5V logic circuitry from the high-current demands of the 12V solenoid lock.

## 🛠️ Hardware Requirements
* Arduino Uno R3
* R307 Optical Fingerprint Sensor
* 16x2 LCD Display with I2C Backpack
* 12V Solenoid Door Lock
* 1-Channel 5V Relay Module
* 12V DC Power Supply (for the Solenoid)
* Resistors (for 5V to 3.3V voltage divider)

## 🔌 Circuit Configuration & Wiring

### 1. I2C LCD Display
* **VCC:** -> Arduino 5V
* **GND:** -> Arduino GND
* **SDA:** -> Arduino Analog Pin A4
* **SCL:** -> Arduino Analog Pin A5

### 2. Fingerprint Sensor (R307)
*Note: The R307 logic operates at 3.3V. A voltage divider is strictly required on the Arduino's TX pin to step down the 5V signal.*
* **VCC:** -> Arduino 5V
* **GND:** -> Arduino GND
* **TX (Green):** -> Arduino Digital Pin 2 (RX)
* **RX (White):** -> Arduino Digital Pin 3 (TX) *[Via Voltage Divider]*

### 3. Relay & Solenoid Control
* **Relay IN:** -> Arduino Digital Pin 8
* **Relay VCC/GND:** -> Arduino 5V / GND
* **12V Power (+) ->** Relay `COM` (Common)
* **Solenoid (+) ->** Relay `NO` (Normally Open)
* **Solenoid (-) ->** 12V Power (-)

## 💻 Firmware Overview & Dynamic Memory Allocation

*Requires the `Adafruit Fingerprint Sensor Library` and `LiquidCrystal I2C` library.*

This system utilizes a two-script architecture, leveraging both the flash memory of the R307 sensor and the internal EEPROM of the Arduino Uno.

* **`Admin_Panel.ino` (Enroll & Delete)**: An interactive command-line interface via the Serial Monitor. 
  * **Dynamic Mapping:** When a new user is enrolled, the script asks for a text string (the user's name). The fingerprint biometric data is saved to the R307 module, while the name string is formatted and saved to a specific calculated address in the Arduino's EEPROM.
  * **Data Management:** Features a built-in deletion protocol that removes the biometric data from the sensor and concurrently wipes the associated name from the EEPROM to ensure memory hygiene.
* **`Verification_and_Lock.ino`**: The main execution loop. Upon a successful biometric scan, the Arduino receives the verified ID from the sensor, calculates the exact memory address in the EEPROM, retrieves the user's string name, and displays a personalized welcome message on the LCD while triggering the mechanical lock.
