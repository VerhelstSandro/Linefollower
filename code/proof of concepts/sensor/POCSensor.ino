/*
//--------------------------------------------=-==- POC Sensor -==-=---------------------------------------------------------
*/

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

unsigned long currentMillis;
unsigned long previousMillis;

int sensPins[8] = { SENS1, SENS2, SENS3, SENS4, SENS5, SENS6, SENS7, SENS8 };
int sens[8];

//--------------------------------------------=-==- SETUP -==-=---------------------------------------------------------
void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);

  pinMode(SENSLED, OUTPUT);
}

//--------------------------------------------=-==- LOOP -==-=---------------------------------------------------------
void loop() {

  currentMillis = millis();

  digitalWrite(LED, HIGH);
  digitalWrite(SENSLED, HIGH);

  sensorRead();

  if (currentMillis - previousMillis > 100) {
    previousMillis = currentMillis;

    Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n%d,%d,%d,%d,%d,%d,%d,%d\n", sens[0], sens[1], sens[2], sens[3], sens[4], sens[5], sens[6], sens[7]);
  }
}

//--------------------------------------------=-==- FUNCTIES -==-=---------------------------------------------------------

//============================== ANAOLOGE INGANGEN LEZEN ==============================
void sensorRead() {
  for (int i = 0; i < 8; i++) {
    sens[i] = analogRead(sensPins[i]);
  }
}
