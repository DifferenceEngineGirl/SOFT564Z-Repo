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

Servo dipper;

int motors;
volatile int stopFlag;
volatile int moveFlag;
volatile int forFlag;
volatile int backFlag;

int sensorData;
int count;

void halt(){
  stopFlag = HIGH;
  moveFlag = LOW;
  forFlag = LOW;
  backFlag = LOW;
  
  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, HIGH);

  motors = 0;
}

void forward() {
  stopFlag = LOW;
  moveFlag = LOW;
  forFlag = HIGH;
  backFlag = LOW;
  
  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, LOW);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, LOW);

  motors = 1;
}

void backward() {
  stopFlag = LOW;
  moveFlag = LOW;
  forFlag = LOW;
  backFlag = HIGH;
  
  digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, LOW);
  digitalWrite(motorBPin2, HIGH);

  motors = 1;
}

void leftTurn(){
  digitalWrite(motorAPin1, HIGH);
  digitalWrite(motorAPin2, LOW);

  digitalWrite(motorBPin1, LOW);
  digitalWrite(motorBPin2, HIGH);

  motors = 1;

  delay(turn90);

  halt();
}

void rightTurn(){
  digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, HIGH);

  digitalWrite(motorBPin1, HIGH);
  digitalWrite(motorBPin2, LOW);

  motors = 1;

  delay(turn90);

  halt();
}


float getDistance(){
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

void takeSample(){
  dipper.write(90);
  delay(1000);
  //read moisture sensor
  sensorData = analogRead(sensorPin);
  dipper.write(0);
  delay(1000);
  count++;
}

void autoMove(){
  stopFlag = LOW;
  moveFlag = HIGH;
  forFlag = LOW;
  backFlag = LOW;
  
  unsigned long start = millis();
  if((getDistance() > minDist)||
     (stopFlag == LOW)){
    forward();
  } else {
    rightTurn();
  }
  while(motors){
    if((millis() - start >= sideTime) || 
       (getDistance() <= minDist) ||
       (stopFlag == LOW)){
      halt();
    }
  }
  if(getDistance() <= minDist){
    //avoid
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

  i2cSetup();

  count = 0;
  
  halt();
}

void loop() {

  if(moveFlag == HIGH){
    autoMove();
  }else if(forFlag == HIGH){
    forward();
  }else if(backFlag == HIGH){
    backward();
  }else if(stopFlag == HIGH){
    halt();
  }


    
  delay(500);







  
  /*for(int i=0; i<4; i++){
  forward(2000);
  rightTurn();
  //take sample
  }
  halt();
  delay(5000);*/
}
