# RFID_Based_Door_Lock_With_Servo

  Features
Only authorized card (UID: F3 89 97 0F) can unlock
Servo lock mechanism with:
Lock position: 82°
Open position: 110°
Smooth, slow movement (step-based)
Auto-close after a set time (default: 5 seconds)

  Buzzer sounds:
Double-beep at boot
Short melody before opening
Short melody before closing
Long warning music for wrong cards

  16x2 I2C LCD:
I2C Address: 0x25
Boot screen: RFID DOOR LOCK / Initializing...
Status messages: Access Granted/Denied, Door Open, Auto Closing, etc.

  Hardware Used
Arduino (UNO / Nano / similar)
RC522 RFID reader (SPI)
SG90 / similar servo motor
Active buzzer
16x2 LCD with I2C backpack (address: 0x25)
Jumper wires
5V power (from Arduino or external, depending on the servo)

How It Works

  Power On
Servo moves to Lock_Angle (82°).
The buzzer plays a double-beep.
LCD shows:
RFID DOOR LOCK
Initializing...
After boot time, it shows: Scan Your Card / Door Locked.

  Authorized Card Detected
UID matches F3 89 97 0F.
LCD: Access Granted / Door Opening.
Buzzer plays short unlock melody.
The servo slowly moves from 82° to 110° (opens).
LCD: Door Open/Auto Closing...
Door stays open for Door_Open_Time (e.g., 5 seconds).

  Auto-Close
After time ends, LCD: Door Closing / Please Wait.
Buzzer plays the  lock melody.
Servo moves from 110° → 82° (locks).
LCD: Scan Your Card / Door Locked.

  Wrong Card
UID doesn’t match the allowed UID.
LCD: Access Denied / Wrong Card.
Buzzer plays long warning melody.
The servo does not move (door stays locked).

  Setup & Upload
Install libraries in Arduino IDE:
MFRC522
Servo
LiquidCrystal_I2C
Wire everything according to the pin table.
Paste the code into the Arduino IDE.
Select your board and COM port.
Upload.
Open Serial Monitor (9600 baud) to see card UIDs if you want to add more cards later.


📩 Contact, For help, 
suggestions, or collaboration, feel free to reach out. 
📧 Email: riyadhasan24a@gmail.com 
📱 WhatsApp: +88 01730 288553
