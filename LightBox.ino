#include <LiquidCrystal.h>
#include "ArduinoGraphics.h"       
#include "Arduino_LED_Matrix.h"


LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
ArduinoLEDMatrix matrix;

// Pins
const int PIR_PIN  = 10;   // PIR OUT
const int LDR_PIN  = A0;   // Photoresistor
const int LED_PIN  = 9;    // LED 1
const int LED2_PIN = 8;    // LED 2

// LDR 
const int MIN_LIGHT = 200;
const int MAX_LIGHT = 900;

// PIR warm-up
unsigned long bootTime = 0;
const unsigned long PIR_WARMUP = 5000UL;  // Ignore PIR for first 5 sec

// Timeout
unsigned long motionStartTime = 0;
const unsigned long TIMEOUT_MS = 60000UL; // 1 minute

// System state
bool systemOn = false;     // start OFF
int lastPirVal = LOW;


String lastLCDLine1 = "";
String lastLCDLine2 = "";

// Update LCD
void updateLCD(String line1, String line2) {
  if (line1 != lastLCDLine1 || line2 != lastLCDLine2) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
    lastLCDLine1 = line1;
    lastLCDLine2 = line2;
  }
}

// Serial monitor + LED matrix 
void logMessage(String msg) {
  Serial.println(msg);

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);        
  matrix.textScrollSpeed(50);       
  matrix.textFont(Font_5x7);        
  matrix.beginText(0, 1, 0xFFFFFF); 
  matrix.println(msg.c_str());      
  matrix.endText(SCROLL_LEFT);      
  matrix.endDraw();
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  Serial.begin(9600);
  lcd.begin(16, 2);
  matrix.begin();

  updateLCD("Starting...", "");
  logMessage("Booting...");
  delay(800);
  updateLCD("", "");

  bootTime = millis();   
}

void loop() {
  int pirVal = LOW;

  // Ignore PIR during warm-up
  if (millis() - bootTime > PIR_WARMUP) {
    pirVal = digitalRead(PIR_PIN);
  }

  // Detect new motion only if system is OFF
  if (pirVal == HIGH && lastPirVal == LOW && !systemOn) {
    systemOn = true;
    motionStartTime = millis();
    logMessage("Motion -> System ON");
  }
  lastPirVal = pirVal;

  // Force OFF after timeout
  if (systemOn && millis() - motionStartTime > TIMEOUT_MS) {
    systemOn = false;
    logMessage("Timeout -> System OFF");
  }


  // System OFF

  if (!systemOn) {
    analogWrite(LED_PIN, 0);
    analogWrite(LED2_PIN, 0);
    updateLCD("SYSTEM OFF", "Waiting Motion");
    return;
  }

  // System ON

  int ldrVal = analogRead(LDR_PIN);
  int brightness = map(ldrVal, MIN_LIGHT, MAX_LIGHT, 255, 0);
  brightness = constrain(brightness, 0, 255);

  analogWrite(LED_PIN, brightness);
  analogWrite(LED2_PIN, brightness);

  String mode;
  if (brightness > 200) mode = "RELAX MODE";
  else if (brightness < 50) mode = "FOCUS MODE";
  else mode = "AMBIENT MODE";

  updateLCD("SYSTEM ON", mode);
  logMessage("LDR:" + String(ldrVal) + " Brightness:" + String(brightness));
}