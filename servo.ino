#include <Servo.h>
#include <LiquidCrystal.h>

// Define pins for ultrasonic sensors
const int triggerPinIn1 = 9;
const int echoPinIn1 = 10;
const int triggerPinIn2 = A0;
const int echoPinIn2 = A1;
const int triggerPinOut1 = A2;
const int echoPinOut1 = A3;
const int triggerPinOut2 = A4;
const int echoPinOut2 = A5;

// Define servo pin
const int servoPin = 8;

// Define LCD pins
const int rs = 2;
const int en = 3;
const int d4 = 4;
const int d5 = 5;
const int d6 = 6;
const int d7 = 7;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Servo myServo;

bool gateOpen = false;
unsigned long gateOpenTime = 0;
int availableSlots = 2;

void setup() {
  pinMode(triggerPinIn1, OUTPUT);
  pinMode(echoPinIn1, INPUT);
  pinMode(triggerPinIn2, OUTPUT);
  pinMode(echoPinIn2, INPUT);
  pinMode(triggerPinOut1, OUTPUT);
  pinMode(echoPinOut1, INPUT);
  pinMode(triggerPinOut2, OUTPUT);
  pinMode(echoPinOut2, INPUT);
  myServo.attach(servoPin);
  lcd.begin(16, 2);
}

void loop() {
  // Measure distances from all sensors
  int in1Distance = measureDistance(triggerPinIn1, echoPinIn1);
  int in2Distance = measureDistance(triggerPinIn2, echoPinIn2);
  int out1Distance = measureDistance(triggerPinOut1, echoPinOut1);
  int out2Distance = measureDistance(triggerPinOut2, echoPinOut2);

  // Update available slots count
  if (out1Distance <= 15 && out2Distance <= 15) {
    availableSlots = 0;
  } else if (out1Distance <= 15 || out2Distance <= 15) {
    availableSlots = 1;
  } else {
    availableSlots = 2;
  }

  // Check if parking is full
  if (availableSlots == 0) {
    lcd.clear();
    lcd.print("Parking Full");
    // Reset gate state and time if parking is full
    gateOpen = false;
    gateOpenTime = 0;
  } else {
    if (availableSlots == 1) {
      lcd.clear();
      lcd.print("1 Slot Empty");
    } else {
      lcd.clear();
      lcd.print("Parking Here");
    }
    
    // Check if any incoming sensor detects something and gate is not already open
    if ((in1Distance <= 15 || in2Distance <= 15) && !gateOpen) {
      lcd.clear();
      lcd.print("Gate Opening");
      myServo.write(90); // Open the gate
      gateOpen = true;
      gateOpenTime = millis();
    }
  }

  // Check if the gate has been open for 5 seconds
  if (gateOpen && millis() - gateOpenTime >= 5000) {
    lcd.clear();
    lcd.print("Gate Closing");
    myServo.write(0); // Close the gate
    gateOpen = false;
    delay(3000); // Delay for 3 seconds before closing
  }

  delay(1000); // Small delay between measurements
}

// Function to measure distance using ultrasonic sensor
int measureDistance(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  return pulseIn(echoPin, HIGH) * 0.034 / 2;
}
