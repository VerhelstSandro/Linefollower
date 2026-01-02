/*
---------------------------------==-=- Proof Of concept H-Brug -=-==---------------------------------
*/

#include <SPI.h>

//Linker Motor
#define PWMA 17
#define AIN1 19  //Vooruit
#define AIN2 18

//Rechter Motor
#define PWMB 23
#define BIN1 21
#define BIN2 22  //Vooruit

//status LED
#define LED 2

//--------------------------------------------=-==- SETUP -==-=---------------------------------------------------------
void setup() {
  SPI.end();

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(LED, OUTPUT);
}

//--------------------------------------------=-==- LOOP -==-=---------------------------------------------------------
void loop() {
  digitalWrite(LED, HIGH);
  
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  for (int speed = 0; speed <= 255; speed += 1) {
    analogWrite(PWMA, speed);
    delay(15);
  }
  for (int speed = 0; speed <= 255; speed += 1) {
    analogWrite(PWMB, speed);
    delay(15);
  }
  delay(100);
  for (int speed = 255; speed >= 0; speed -= 1) {
    analogWrite(PWMA, speed);
    analogWrite(PWMB, speed);
    delay(5);
  }

  // ACHTERUIT
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  for (int speed = 0; speed <= 255; speed += 1) {
    analogWrite(PWMA, speed);
    delay(15);
  }
  for (int speed = 0; speed <= 255; speed += 1) {
    analogWrite(PWMB, speed);
    delay(15);
  }
  delay(100);
  for (int speed = 255; speed >= 0; speed -= 1) {
    analogWrite(PWMA, speed);
    analogWrite(PWMB, speed);
    delay(5);
  }
}