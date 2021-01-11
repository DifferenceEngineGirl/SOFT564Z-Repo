//Include the servo library
#include <Servo.h>

//Pin Definitions
#define echoPin 2
#define trigPin 3

#define motorAPin1 6
#define motorAPin2 7

#define motorBPin1 4
#define motorBPin2 5

#define servoPin 9

#define sensorPin A3

//Time for a 90 turn
#define turn90 375
//Time fo one side of the square
#define sideTime 2000

//Minimum distance for the ultrasonic
#define minDist 10.0

//Forward and Backward values for the direction flag
#define FOR 1
#define BACK 0

//Assign a name to the servo
Servo dipper;

//Flags
int motors;
volatile int stopFlag;
volatile int moveFlag;
volatile int dirFlag;
volatile int leftFlag;
volatile int rightFlag;
volatile int sampleFlag;

//Variable to hold the reading from the moisture sensor
int sensorData;
//Variable to hold the number of readings taken since the last restart of the arduino
int count;

//Function to get the distance from an object with the ultrasonic sensor
float getDistance() {
  // ---------------------------------------------------------------- //
  // Arduino Ultrasonic Sensor HC-SR04
  // Re-written by Arbi Abdul Jabbaar
  // Using Arduino IDE 1.8.7
  // Using HC-SR04 Module
  // Tested on 17 September 2019
  // ---------------------------------------------------------------- //

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

//Stop function
void halt() {

  //Set all motor pins to high
  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, HIGH);

  //Set the motor flag low
  motors = 0;
}

//90deg right turn function
void rightTurn() {
  //Set the left motor to run forward, and the right to run backwards
  digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, LOW);

  motors = 1;

  //Wait for the time it takes to turn 90deg
  delay(turn90);

  //Stop the motors
  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, HIGH);

  //Set the right turn flag low
  rightFlag = LOW;
}

//Forward function
void forward() {
  //If it's about to bump into something, turn right
  if (getDistance() <= minDist) {
    rightTurn();
  // If not, set both motors to run forwards
  } else {
    digitalWrite(motorAPin1, HIGH);
    digitalWrite(motorAPin2, LOW);

    digitalWrite(motorBPin1, HIGH);
    digitalWrite(motorBPin2, LOW);

    //Set motors flag high
    motors = 1;
  }
}

//Backwards function
void backward() {
  //Set both motors to run backwards
  digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, LOW);
  digitalWrite(motorBPin2, HIGH);

  //Set motors flag high
  motors = 1;
}

//90deg left turn function
void leftTurn() {
  //Set the left motor to run backwards and the right to run forwards
  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, LOW);

  digitalWrite(motorBPin1, LOW);
  digitalWrite(motorBPin2, HIGH);

  //Set motors flag high
  motors = 1;

  //Wait for the time it takes to turn 90deg
  delay(turn90);

  //Stop the motors
  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, HIGH);

  //Set the left turn flag low
  leftFlag = LOW;
}

void takeSample() {
  //Stop the motors
  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, HIGH);

  //Turn the servo 90deg
  dipper.write(90);
  //Wait 1s
  delay(1000);
  //Read moisture sensor
  sensorData = analogRead(sensorPin);
  //Return the servo to its start position
  dipper.write(0);
  //Wait 1s
  delay(1000);
  //Increment count
  count++;
  //Set the sample flag low
  sampleFlag = LOW;
}

//Autonomous movement function
void autoMove() {
  //Save the start time
  unsigned long start = millis();
  //If there are no immediate obstacles and no stop signal has been sent
  if ((getDistance() > minDist) ||
      (stopFlag == LOW)) {
    //Start moving forwards
    forward();
  //Otherwise
  } else {
    //Turn right
    rightTurn();
  }
  //While the motors are on
  while (motors) {
    //If the length of time for a side has passed, there is an obstacle, or the stop signal has been sent 
    if ((millis() - start >= sideTime) ||
        (getDistance() <= minDist) ||
        (stopFlag == HIGH)) {
      //Stop
      halt();
    }
    //Otherwise keep going
  }
  //If there is an obstacle
  if (getDistance() <= minDist) {
    //Right turn to avoid
    rightTurn();
  //Otherwise
  } else {
    //Take a sample, then make a right turn
    takeSample();
    rightTurn();
  }
}

void setup() {
  // put your setup code here, to run once:
  //Set the motors as outputs
  pinMode(motorAPin1, OUTPUT);
  pinMode(motorAPin2, OUTPUT);

  pinMode(motorBPin1, OUTPUT);
  pinMode(motorBPin2, OUTPUT);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  //Attach the servo to the relevant pin, and set it at its rest position
  dipper.attach(servoPin);
  dipper.write(0);

  //Run the I2C setup
  i2cSetup();

  //Initialise the counter at 0
  count = 0;

  //Initialise the stop flag high
  stopFlag = HIGH;

  //Make sure the buggy is not moving
  halt();
}

void loop() {

  //Run functions if the relevant flag is high
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
}
