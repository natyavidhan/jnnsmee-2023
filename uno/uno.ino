#include "LiquidCrystal_I2C.h"
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define IRSENSOR_PIN 3
#define buzz 4
long newTime = millis();
long oldTime = millis();
bool lastRead = 0;
bool newRead = 0;
#define BLACK 0
#define WHITE 1
float Time;
float sped;
SoftwareSerial serial_connection(14, 12);

void setup () {
  lcd.begin();
  pinMode(buzz, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  newRead = digitalRead(IRSENSOR_PIN);
  if (newRead == BLACK && newRead != lastRead) {
    lastRead = newRead;
  }else 
  if (newRead == WHITE && newRead != lastRead) {
    newTime = millis();
    lastRead = newRead;
    Time = (newTime - oldTime);
    oldTime =  millis();
    sped = (6/(Time/1000));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Speed");
    lcd.setCursor(0, 1);
    lcd.print(sped);
    if (sped > 50) {
      Serial.println(sped);
      digitalWrite(buzz, HIGH);
      delay(500);
      digitalWrite(buzz, LOW);      
      delay(4500);
      Serial.println(sped);
    }
  }

}
