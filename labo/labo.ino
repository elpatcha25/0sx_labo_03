#include "OneButton.h"
#include <Wire.h>
#include <LCD_I2C.h>

int ThermistorPin = A0;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc;
float c1 = 1.129148e-03, c2 = 2.34125e-04, c3 = 8.76741e-08;
int numeroEtudiant = 22;
int tempsAffiche = 3000;
byte caract[8] = {
  B01000,
  B10100,
  B00100,
  B01010,
  B11101,
  B00001,
  B00010,
  B00111
};
byte degree[8] = {
  B01110,
  B01010,
  B01110,
  B00000,
 B00000,
  B00000,
  B00000,
  B00000
};

#define THERMISTOR_PIN A0
#define JOYSTICK_X A1
#define JOYSTICK_Y A2
#define JOYSTICK_BTN 2
#define PIN_LED 9
#define PIN_INPUT 2

LCD_I2C lcd(0x27, 16, 2);

unsigned long currentTime = 0;
bool pageAffichage = false;
int vitesse = 0;
int angle = 0;
bool climatisation = false;

void setup() {
  Serial.begin(115200);
  pinMode(THERMISTOR_PIN, INPUT);
  pinMode(PIN_INPUT, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(JOYSTICK_BTN, INPUT_PULLUP);

  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, caract);
  lcd.createChar(1, degree);
  demarrage();
}

void demarrage() {
  lcd.setCursor(0, 0);
  lcd.print("TEMATIO");
  lcd.setCursor(14, 1);
  lcd.print(22);
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  delay(tempsAffiche);
  lcd.clear();
}

void temperature() {
  Vo = analogRead(THERMISTOR_PIN);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;

  climatisation = Tc > 25;
  
  digitalWrite(PIN_LED, climatisation ? HIGH : LOW);
}

void thermistance() {
  temperature();

  int valY = analogRead(JOYSTICK_Y);
  int valX = analogRead(JOYSTICK_X);

  vitesse = map(valY, 0, 1023, -25, 120);
  if (valY < 512) {
    vitesse = map(valY, 0, 511, -25, 0);
  } else {
    vitesse = map(valY, 512, 1023, 0, 120);
  }

  angle = map(valX, 0, 1023, -90, 90);

  if (digitalRead(JOYSTICK_BTN) == LOW) {
   
    pageAffichage = !pageAffichage;
  }
}

void gererAffichage() {
  lcd.clear();
  if (!pageAffichage) {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(Tc);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("AC: ");
      lcd.print(climatisation ? "ON" : "OFF");
  } else {
    lcd.setCursor(0, 0);
    if (vitesse<0){
 lcd.print( "Recule: ");
  }
  else {
    lcd.print("Avance: ");
  } 
  
    lcd.print(abs(vitesse));
    lcd.print("km/h");
    lcd.setCursor(0, 1);
    lcd.print("Dir: ");
    lcd.print(angle);
     lcd.setCursor(8, 1);
  lcd.write(byte (1));
  if (angle<0){
 lcd.print( "G");
  }
  else {
    lcd.print("D");
  } 
  }
  delay(500);
}

void trame() {
  Serial.print("etd:2407822,");
  Serial.print("x: "); Serial.print(analogRead(JOYSTICK_X));
  Serial.print(", y: "); Serial.print(analogRead(JOYSTICK_Y));
  Serial.print(", sys: "); 
  if (climatisation){
Serial.println( 1 );
  }
  else{
    Serial.println( 0 );
  }
}

void loop() {
  currentTime = millis();
  thermistance();
  gererAffichage();
  trame();
}
