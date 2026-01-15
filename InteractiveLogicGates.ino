#include <LiquidCrystal.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
ArduinoLEDMatrix matrix;

// Tones
#define TONE_FAIL 200
#define TONE_AND  392
#define TONE_OR   440
#define TONE_NOT  880

// Logic switches (INPUT_PULLUP)
const int AND_A_PIN = A1;
const int AND_B_PIN = A2;
const int OR_A_PIN  = A3;
const int OR_B_PIN  = A4;
const int NOT_PIN   = A5;

const int BUZZER_PIN = 9;

bool prevNotPressed = false;


// LED MATRIX display
void showMatrixWord(const char* word) {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.print(word);
  matrix.endText();
  matrix.endDraw();
}

// Setup
void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("System Starting");
  delay(800);

  pinMode(AND_A_PIN, INPUT_PULLUP);
  pinMode(AND_B_PIN, INPUT_PULLUP);
  pinMode(OR_A_PIN,  INPUT_PULLUP);
  pinMode(OR_B_PIN,  INPUT_PULLUP);
  pinMode(NOT_PIN,   INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);

  matrix.begin();
  showMatrixWord("IDLE");

  lcd.clear();
}

// Update LCD
void updateLCD_AndOr(String gate, int A, int B, int OUT) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(gate);
  lcd.setCursor(0, 1);
  lcd.print("A:");
  lcd.print(A);
  lcd.print(" B:");
  lcd.print(B);
  lcd.print(" OUT:");
  lcd.print(OUT);
}

void updateLCD_Not(int A, int OUT) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NOT GATE");
  lcd.setCursor(0, 1);
  lcd.print("A:");
  lcd.print(A);
  lcd.print(" OUT:");
  lcd.print(OUT);
}

// Logic
void loop() {

  // Read inputs
  int A = !digitalRead(AND_A_PIN);
  int B = !digitalRead(AND_B_PIN);
  int C = !digitalRead(OR_A_PIN);
  int D = !digitalRead(OR_B_PIN);
  int E = !digitalRead(NOT_PIN);

  // -------- AND GATE --------
  if (A || B) {
    int AND_out = A && B;

    updateLCD_AndOr("AND GATE", A, B, AND_out);
    showMatrixWord("AND");

    if (AND_out) tone(BUZZER_PIN, TONE_AND);
    else noTone(BUZZER_PIN);

    delay(80);
    return;
  }

  // -------- OR GATE --------
  if (C || D) {
    int OR_out = C || D;

    updateLCD_AndOr("OR GATE", C, D, OR_out);
    showMatrixWord("OR");

    if (OR_out) tone(BUZZER_PIN, TONE_OR);
    else noTone(BUZZER_PIN);

    delay(80);
    return;
  }

  // -------- NOT GATE --------
  int NOT_out = !E;

  if (E || E != prevNotPressed) {
    updateLCD_Not(E, NOT_out);
    showMatrixWord("NOT");

    if (E != prevNotPressed) {
      if (NOT_out) tone(BUZZER_PIN, TONE_NOT, 200);
      else tone(BUZZER_PIN, TONE_FAIL, 200);
      prevNotPressed = E;
    }

    delay(200);
    return;
  }

  // -------- IDLE --------
  lcd.clear();
  lcd.print("Waiting Input...");
  showMatrixWord("IDLE");
  noTone(BUZZER_PIN);
  delay(120);
}
