#include <Servo.h>

#define echoPin 2
#define trigPin 3

#define motorAPin1 6
#define motorAPin2 7

#define motorBPin1 4
#define motorBPin2 5

#define servoPin 9

#define sensorPin A3

#define turn90 375
#define sideTime 2000

#define minDist 10.0

#define FOR 1
#define BACK 0

Servo dipper;

int motors;
volatile int stopFlag;
volatile int moveFlag;
volatile int dirFlag;
volatile int leftFlag;
volatile int rightFlag;
volatile int sampleFlag;

int sensorData;
int count;

float getDistance() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  float distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  return distance;
}

void halt() {

  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, HIGH);

  motors = 0;
}

void forward() {

  if (getDistance() <= minDist) {
    rightTurn();
  } else {
    digitalWrite(motorAPin1, HIGH);
    digitalWrite(motorAPin2, LOW);

    digitalWrite(motorBPin1, HIGH);
    digitalWrite(motorBPin2, LOW);

    motors = 1;
  }
}

void backward() {
  
    digitalWrite(motorAPin1, LOW);
    digitalWrite(motorAPin2, HIGH);

    digitalWrite(motorBPin1, LOW);
    digitalWrite(motorBPin2, HIGH);

    motors = 1;
}

void leftTurn() {
  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, LOW);

  digitalWrite(motorBPin1, LOW);
  digitalWrite(motorBPin2, HIGH);

  motors = 1;

  delay(turn90);

  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, HIGH);

  leftFlag = LOW;
}

void rightTurn() {
  digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, LOW);

  motors = 1;

  delay(turn90);

  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, HIGH);

  rightFlag = LOW;
}


void takeSample() {

  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, HIGH);

  dipper.write(90);
  delay(1000);
  //read moisture sensor
  sensorData = analogRead(sensorPin);
  dipper.write(0);
  delay(1000);
  count++;
  sampleFlag = LOW;
}

void autoMove() {

  unsigned long start = millis();
  if (stopFlag == LOW) {
    forward();
  }
  while (motors) {
    if ((millis() - start >= sideTime) ||
        (getDistance() <= minDist) ||
        (stopFlag == HIGH)) {
      halt();
    }
  }
  if (getDistance() <= minDist) {
    //avoid
    rightTurn();
  } else {
    takeSample();
    rightTurn();
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(motorAPin1, OUTPUT);
  pinMode(motorAPin2, OUTPUT);

  pinMode(motorBPin1, OUTPUT);
  pinMode(motorBPin2, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  dipper.attach(servoPin);
  dipper.write(0);

  i2cSetup();

  count = 0;

  stopFlag = HIGH;

  halt();
}

void loop() {

  if (rightFlag == HIGH) {
    rightTurn();
  } else if (leftFlag == HIGH) {
    leftTurn();
  } else if (moveFlag == HIGH) {
    autoMove();
  } else if (sampleFlag == HIGH) {
    takeSample();
  } else if (stopFlag == LOW) {
    if (dirFlag == FOR) {
      forward();
    } else if (dirFlag == BACK) {
      backward();
    }
  } else {
    halt();
  }



  delay(500);
}
