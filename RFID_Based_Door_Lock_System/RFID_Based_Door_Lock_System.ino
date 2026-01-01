/* The source Code from : https://github.com/riyadhasan24
 * By Md. Riyad Hasan
 */
 
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

const byte RST_Pin     = 9;    // RC522 RST
const byte SDA_SS_Pin  = 10;   // RC522 SDA / SS
const int  Servo_Pin   = 3;
const int  Buzzer_Pin  = 5; 

const byte LCD_Addr    = 0x25;
const byte LCD_Cols    = 16;
const byte LCD_Rows    = 2;

int Lock_Angle         = 84; 
int Open_Angle         = 150;
int Servo_Step         = 1;
int Servo_Step_Delay   = 2;

unsigned long Door_Open_Time  = 5000;
unsigned long Door_Open_Start = 0;

unsigned long Boot_Time          = 4000; 

int Boot_Beep_Frequency = 1500; // Hz
int Boot_Beep_Duration  = 120;  // ms
int Boot_Beep_Gap       = 120;  // ms between beeps

// ----- AUTHORIZED UID -----
// Allowed card UID: F3 89 97 0F
byte Allowed_UID[4] = { 0xF3, 0x89, 0x97, 0x0F };
byte Allowed_UID_Length = 4;

MFRC522 Rfid( SDA_SS_Pin, RST_Pin );
Servo Door_Servo;
LiquidCrystal_I2C Lcd( LCD_Addr, LCD_Cols, LCD_Rows );

bool Door_Is_Open = false;

const int Unlock_Melody_Notes[]     = { 1200, 1400, 1600 };
const int Unlock_Melody_Durations[] = { 120, 120, 200 };
const int Unlock_Melody_Length      = sizeof(Unlock_Melody_Notes) / sizeof(int);

const int Lock_Melody_Notes[]       = { 1600, 1300 };
const int Lock_Melody_Durations[]   = { 150, 220 };
const int Lock_Melody_Length        = sizeof(Lock_Melody_Notes) / sizeof(int);

const int Wrong_Melody_Notes[]      = 
{
  800, 900, 1000, 900, 800, 700, 600, 700, 800
};
const int Wrong_Melody_Durations[]  = 
{
  150, 150, 150, 150, 150, 150, 150, 150, 250
};
const int Wrong_Melody_Length       = sizeof(Wrong_Melody_Notes) / sizeof(int);

void Play_Melody(const int *notes, const int *durations, int length) 
{
  for (int i = 0; i < length; i++) 
  {
    int freq = notes[i];
    int dur  = durations[i];
    tone(Buzzer_Pin, freq, dur);
    delay(dur + 30);
    noTone(Buzzer_Pin);
  }
}

void Play_Boot_Double_Beep() 
{
  tone(Buzzer_Pin, Boot_Beep_Frequency, Boot_Beep_Duration);
  delay(Boot_Beep_Duration + 10);
  noTone(Buzzer_Pin);

  delay(Boot_Beep_Gap);

  tone(Buzzer_Pin, Boot_Beep_Frequency, Boot_Beep_Duration);
  delay(Boot_Beep_Duration + 10);
  noTone(Buzzer_Pin);
}

void Lcd_Print_Centered(byte row, const char *text) 
{
  int len = strlen(text);
  int pos = (LCD_Cols - len) / 2;
  if (pos < 0) pos = 0;
  Lcd.setCursor(pos, row);
  Lcd.print(text);
}

void Move_Servo_Slow(int fromAngle, int toAngle) 
{
  if (fromAngle == toAngle) return;

  if (fromAngle < toAngle) 
  {
    for (int pos = fromAngle; pos <= toAngle; pos += Servo_Step) 
    {
      Door_Servo.write(pos);
      delay(Servo_Step_Delay);
    }
  } 
  else 
  {
    for (int pos = fromAngle; pos >= toAngle; pos -= Servo_Step) 
    {
      Door_Servo.write(pos);
      delay(Servo_Step_Delay);
    }
  }
}

bool Is_Authorized_Card() 
{
  if (Rfid.uid.size != Allowed_UID_Length) 
  {
    return false;
  }
  for (byte i = 0; i < Allowed_UID_Length; i++) 
  {
    if (Rfid.uid.uidByte[i] != Allowed_UID[i]) 
    {
      return false;
    }
  }
  return true;
}

void Show_Idle_Screen() 
{
  Lcd.clear();
  Lcd_Print_Centered(0, "Scan Your Card");
  Lcd_Print_Centered(1, "Door Locked");
}

void Show_Open_Screen() 
{
  Lcd.clear();
  Lcd_Print_Centered(0, "Access Granted");
  Lcd_Print_Centered(1, "Door Opening");
}

void Show_Door_Open_Hold_Screen() 
{
  Lcd.clear();
  Lcd_Print_Centered(0, "Door Open");
  Lcd_Print_Centered(1, "Auto Closing...");
}

void Show_Closing_Screen() 
{
  Lcd.clear();
  Lcd_Print_Centered(0, "Door Closing");
  Lcd_Print_Centered(1, "Please Wait");
}

void Show_Denied_Screen() 
{
  Lcd.clear();
  Lcd_Print_Centered(0, "Access Denied");
  Lcd_Print_Centered(1, "Wrong Card");
}

void Run_Boot_Animation() 
{
  Door_Servo.write(Lock_Angle);
  delay(500); // small pause before animation

  Play_Boot_Double_Beep();

  unsigned long startTime = millis();

  while (millis() - startTime < Boot_Time) 
  {
    Lcd.clear();
    Lcd_Print_Centered(0, "RFID DOOR LOCK");
    Lcd_Print_Centered(1, "Initializing...");
    delay(500);
  }

  Show_Idle_Screen();
}

void Open_Door() 
{
  Show_Open_Screen();
  Play_Melody(Unlock_Melody_Notes, Unlock_Melody_Durations, Unlock_Melody_Length);

  int currentAngle = Lock_Angle;
  Move_Servo_Slow(currentAngle, Open_Angle);

  Door_Is_Open = true;
  Door_Open_Start = millis();

  Show_Door_Open_Hold_Screen();
}

void Close_Door() 
{
  Show_Closing_Screen();
  Play_Melody(Lock_Melody_Notes, Lock_Melody_Durations, Lock_Melody_Length);

  int currentAngle = Open_Angle;
  Move_Servo_Slow(currentAngle, Lock_Angle);

  Door_Is_Open = false;
  Show_Idle_Screen();
}

void setup() 
{
  Serial.begin(9600);
  SPI.begin();
  Rfid.PCD_Init();

  Door_Servo.attach(Servo_Pin);
  Door_Servo.write(Lock_Angle);

  Lcd.init();
  Lcd.backlight();

  pinMode(Buzzer_Pin, OUTPUT);
  noTone(Buzzer_Pin);

  Run_Boot_Animation();
}

void loop() 
{
  if (Door_Is_Open) 
  {
    unsigned long now = millis();
    if (now - Door_Open_Start >= Door_Open_Time) 
    {
      Close_Door();
    }
  }

  if (Door_Is_Open) 
  {
    return;
  }

  if (!Rfid.PICC_IsNewCardPresent()) 
  {
    return;
  }
  if (!Rfid.PICC_ReadCardSerial()) 
  {
    return;
  }

  Serial.print("Card UID: ");
  for (byte i = 0; i < Rfid.uid.size; i++)
  {
    Serial.print(Rfid.uid.uidByte[i], HEX);
    Serial.print(i == Rfid.uid.size - 1 ? "" : " ");
  }
  Serial.println();

  if (Is_Authorized_Card()) 
  {
    Open_Door();
  } 
  else 
  {
    Show_Denied_Screen();
    Play_Melody(Wrong_Melody_Notes, Wrong_Melody_Durations, Wrong_Melody_Length);
    delay(800);
    Show_Idle_Screen();
  }
  
  Rfid.PICC_HaltA();
  Rfid.PCD_StopCrypto1();
}
