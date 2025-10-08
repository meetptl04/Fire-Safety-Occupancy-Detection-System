#include <SoftwareSerial.h>
      // Bluetooth serial pins
SoftwareSerial BTSerial(12, 4); // TX, RX

// Smoke sensors
#define SMOKE1_PIN A0
#define SMOKE2_PIN A1

// Buzzer
#define BUZZER_PIN 5

// IR sensors
#define IR1_PIN 11 // Floor 1 Entry
#define IR2_PIN 10 // Floor 1 Exit
#define IR3_PIN 2  // Floor 2 Entry
#define IR4_PIN 3  // Floor 2 Exit

// LEDs
#define LED1_F1 13
#define LED2_F1 8
#define LED1_F2 6
#define LED2_F2 7

const int SMOKE_THRESHOLD = 260;
int peopleCountF1 = 0;
int peopleCountF2 = 0;

bool IR1_triggered = false, IR2_triggered = false;
bool IR3_triggered = false, IR4_triggered = false;

unsigned long lastBTMessageTime = 0;
const unsigned long BT_COOLDOWN = 5000;

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);

  pinMode(SMOKE1_PIN, INPUT);
  pinMode(SMOKE2_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(IR3_PIN, INPUT);
  pinMode(IR4_PIN, INPUT);

  pinMode(LED1_F1, OUTPUT);
  pinMode(LED2_F1, OUTPUT);
  pinMode(LED1_F2, OUTPUT);
  pinMode(LED2_F2, OUTPUT);
}

void loop() {
  handleSmokeDetection();
  handlePeopleCounter();
}

void handleSmokeDetection() {
  int smokeF1 = analogRead(SMOKE1_PIN);
  int smokeF2 = analogRead(SMOKE2_PIN);
  Serial.print("1: ");
  Serial.println(smokeF1);
  Serial.print("2: ");
  Serial.println(smokeF2);

  if (smokeF1 > SMOKE_THRESHOLD || smokeF2 > SMOKE_THRESHOLD) {
    digitalWrite(BUZZER_PIN, HIGH);

    // Send alerts regardless of cooldown
    if (smokeF1 > SMOKE_THRESHOLD) {
      BTSerial.println("ALERT: Smoke detected on Floor 1!");
      BTSerial.print("People on Floor 1: ");
      BTSerial.println(peopleCountF1);
    }
    if (smokeF2 > SMOKE_THRESHOLD) {
      BTSerial.println("ALERT: Smoke detected on Floor 2!");
      BTSerial.print("People on Floor 2: ");
      BTSerial.println(peopleCountF2);
    }

    lastBTMessageTime = millis(); // reset cooldown after emergency
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void handlePeopleCounter() {
  unsigned long currentTime = millis();

  // Floor 1 Entry
  if (digitalRead(IR1_PIN) == LOW && !IR1_triggered) {
    IR1_triggered = true;
    delay(50);
  }
  if (IR1_triggered && digitalRead(IR2_PIN) == LOW) {
    peopleCountF1++;
    Serial.print("Floor 1 - Person Entered. Count: ");
    Serial.println(peopleCountF1);

    if (currentTime - lastBTMessageTime >= BT_COOLDOWN) {
      BTSerial.print("Floor 1 - Person Entered. Count: ");
      BTSerial.println(peopleCountF1);
      lastBTMessageTime = currentTime;
    }

    updateLEDs(peopleCountF1, 1);
    IR1_triggered = IR2_triggered = false;
    delay(500);
  }

  // Floor 1 Exit
  if (digitalRead(IR2_PIN) == LOW && !IR2_triggered) {
    IR2_triggered = true;
    delay(50);
  }
  if (IR2_triggered && digitalRead(IR1_PIN) == LOW) {
    if (peopleCountF1 > 0) peopleCountF1--;
    Serial.print("Floor 1 - Person Exited. Count: ");
    Serial.println(peopleCountF1);

    if (currentTime - lastBTMessageTime >= BT_COOLDOWN) {
      BTSerial.print("Floor 1 - Person Exited. Count: ");
      BTSerial.println(peopleCountF1);
      lastBTMessageTime = currentTime;
    }

    updateLEDs(peopleCountF1, 1);
    IR1_triggered = IR2_triggered = false;
    delay(500);
  }

  // Floor 2 Entry
  if (digitalRead(IR3_PIN) == LOW && !IR3_triggered) {
    IR3_triggered = true;
    delay(50);
  }
  if (IR3_triggered && digitalRead(IR4_PIN) == LOW) {
    peopleCountF2++;
    Serial.print("Floor 2 - Person Entered. Count: ");
    Serial.println(peopleCountF2);

    if (currentTime - lastBTMessageTime >= BT_COOLDOWN) {
      BTSerial.print("Floor 2 - Person Entered. Count: ");
      BTSerial.println(peopleCountF2);
      lastBTMessageTime = currentTime;
    }

    updateLEDs(peopleCountF2, 2);
    IR3_triggered = IR4_triggered = false;
    delay(500);
  }

  // Floor 2 Exit
  if (digitalRead(IR4_PIN) == LOW && !IR4_triggered) {
    IR4_triggered = true;
    delay(50);
  }
  if (IR4_triggered && digitalRead(IR3_PIN) == LOW) {
    if (peopleCountF2 > 0) peopleCountF2--;
    Serial.print("Floor 2 - Person Exited. Count: ");
    Serial.println(peopleCountF2);

    if (currentTime - lastBTMessageTime >= BT_COOLDOWN) {
      BTSerial.print("Floor 2 - Person Exited. Count: ");
      BTSerial.println(peopleCountF2);
      lastBTMessageTime = currentTime;
    }

    updateLEDs(peopleCountF2, 2);
    IR3_triggered = IR4_triggered = false;
    delay(500);
  }
}

void updateLEDs(int count, int floor) {
  int led1 = (floor == 1) ? LED1_F1 : LED1_F2;
  int led2 = (floor == 1) ? LED2_F1 : LED2_F2;

  if (count >= 1 && count <= 5) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
  } else if (count >= 6) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  } else {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }
  Serial.print("LEDs updated for Floor ");
  Serial.println(floor);
}
return;

 
