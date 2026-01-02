
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

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

//Linesensor Array
#define SENS8 26
#define SENS7 36
#define SENS6 39
#define SENS5 34
#define SENS4 35
#define SENS3 32
#define SENS2 33
#define SENS1 25

#define SENSLED 12


//zelf gemaakte' variabelen
bool led = false;
unsigned long currentMillis;
unsigned long previousMillis;
unsigned long safcurrentMillis;
unsigned long safpreviousMillis;
float output;
int position;


int wit[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int zwart[8] = { 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095 };
int offset[8];
float scaleInt[8];
int sensPins[8] = { SENS1, SENS2, SENS3, SENS4, SENS5, SENS6, SENS7, SENS8 };
int sens[8];
int norm[8];
int tot;

const int8_t weights[8] = { -35, -25, -15, -5, 5, 15, 25, 35 };

//variabelen voor BT
float Kp = 1, Ki = 0;
int v;
bool running = false;

//--------------------------------------------=-==- SETUP -==-=---------------------------------------------------------
void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT");  // verander naam indien nodig
  Serial.println("ESP32 BT ready");

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(LED, OUTPUT);

  pinMode(SENSLED, OUTPUT);
}

//--------------------------------------------=-==- LOOP -==-=---------------------------------------------------------
void loop() {


  // Lees berichten van Bluetooth
  if (SerialBT.available()) {
    String line = SerialBT.readStringUntil('\n');
    line.trim();
    handleCommand(line);
    Serial.println(line);
  }

  if (running == 1) {
    //============================== RUNNING ==============================

    //safpreviousMillis = 0;

    sensorRead();

    float pos = 0;
    for (int i = 0; i < 8; i++) {
      pos += norm[i] * (-3.5 + i);  // integer * int8_t
    }

    pos = (float)pos / (float)1000;

    if(norm[0] > (tot*0.9)){
      pos = -4.5;
    }
    else if(norm[7] > (tot*0.9)){
      pos = 4.5;
    }
    
    if (tot < 5000){
      /* proportioneel regelen */
      output = pos * Kp;

      /* integrerend regelen */
      output += Ki * pos;
    }
    else {
      output = 0;
    }

    output = clamp(output, -1, 1);

    int leftmotor = v * clamp((1 - output), 0, 1);
    int rightmotor = v * clamp((1 + output), 0, 1);

    analogWrite(PWMA, leftmotor);
    analogWrite(PWMB, rightmotor);

    /*
    currentMillis = millis();

    if (currentMillis - previousMillis > 100) {
      previousMillis = currentMillis;
      Serial.printf("%d,%d,%d,%d,%d,%d,%d,%d\n", norm[0], norm[1], norm[2], norm[3], norm[4], norm[5], norm[6], norm[7]);
      Serial.printf("totaal = %d\n", tot);
      Serial.printf("position = %.3f\n", pos);
      Serial.printf("output = %.3f, R=%d-L=%d\n", output, rightmotor, leftmotor);
    }
    
    */

  } else {
    //============================== STOPPED ==============================

    currentMillis = millis();

    digitalWrite(SENSLED, LOW);

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, 0);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, 0);

    safpreviousMillis = 0;

    if (currentMillis - previousMillis > 500) {
      previousMillis = currentMillis;
      if (led == false) {
        digitalWrite(LED, HIGH);
        led = true;
      } else {
        digitalWrite(LED, LOW);
        led = false;
      }
    }
  }
}

//--------------------------------------------=-==- FUNCTIES -==-=---------------------------------------------------------

//============================== ANAOLOGE INGANGEN LEZEN & normalizeren ==============================
void sensorRead() {
  tot = 0;
  for (int i = 0; i < 8; i++) {
    sens[i] = analogRead(sensPins[i]);
    norm[i] = map(sens[i], wit[i], zwart[i], 0, 1000);
    tot += norm[i];
  }
}


//============================== BT LEZEN ==============================
void handleCommand(const String &cmd) {
  if (cmd.startsWith("CMD:")) {
    String c = cmd.substring(4);
    if (c == "RUN") {
      running = true;
      SerialBT.println("STATUS:RUNNING");
      SerialBT.printf("PID:Kp=%.2f,Ki=%.2f\n", Kp, Ki);

      digitalWrite(LED, HIGH);
      digitalWrite(SENSLED, HIGH);

      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);

      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, HIGH);

    } else if (c == "STOP") {
      running = false;
      SerialBT.println("STATUS:STOPPED");
    }
  } else if (cmd.startsWith("PAR:")) {
    String c = cmd.substring(4);
    if (c.startsWith("SPEED=")) {
      parseAndSetSpeed(cmd.substring(4));

    } else if (c.startsWith("CAL_WHITE")) {
      // Kalibratie WIT ==============================
      digitalWrite(SENSLED, HIGH);
      delay(100);

      sensorRead();
      for (int i = 0; i < 8; i++) {
        wit[i] = sens[i];
      }
      SerialBT.println("STATUS:CALIBRATED_WHITE");

      digitalWrite(SENSLED, LOW);

    } else if (c.startsWith("CAL_BLACK")) {
      // Kalibratie ZWART ==============================
      digitalWrite(SENSLED, HIGH);
      delay(100);

      sensorRead();
      for (int i = 0; i < 8; i++) {
        zwart[i] = sens[i];
      }
      SerialBT.println("STATUS:CALIBRATED_BLACK");

      digitalWrite(SENSLED, LOW);
    }
  } else if (cmd.startsWith("PID:")) {
    parseAndSetPID(cmd.substring(4));
    SerialBT.printf("PID:Kp=%.4f,Ki=%.4f\n", Kp, Ki);
  }
}

//============================== SNELHEID LEZEN ==============================
void parseAndSetSpeed(String body) {
  int idx;
  idx = body.indexOf("SPEED=");
  if (idx >= 0) v = map(body.substring(idx + 6, body.indexOf(',', idx) > 0 ? body.indexOf(',', idx) : body.length()).toInt(), 0, 100, 0, 255);
}

//============================== PID LEZEN ==============================
void parseAndSetPID(String body) {
  int idx;
  idx = body.indexOf("Kp=");
  if (idx >= 0) Kp = body.substring(idx + 3, body.indexOf(',', idx) > 0 ? body.indexOf(',', idx) : body.length()).toFloat();
  idx = body.indexOf("Ki=");
  if (idx >= 0) Ki = body.substring(idx + 3, body.indexOf(',', idx) > 0 ? body.indexOf(',', idx) : body.length()).toFloat();
}


float clamp(float value, float min, float max) {
  if (value > max) {
    value = max;
  } else if (value < min) {
    value = min;
  }
  return value;
}
