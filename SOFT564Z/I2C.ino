#include <Wire.h>

void i2cSetup(void){
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(command);      // register event
  Wire.onRequest(dataReq);
  Serial.begin(9600);           // start serial for output
}

void command(int comNo){

  switch(comNo){
    case 0x01: 
      //Call do square function
      moveFlag = HIGH;
      forFlag = LOW;
      backFlag = LOW;
      stopFlag = LOW;
      break;
    case 0x02: 
      //Call forward function
      forFlag = HIGH;
      moveFlag = LOW;
      backFlag = LOW;
      stopFlag = LOW;
      break;
    case 0x03: 
      //Call backward function
      backFlag = HIGH;
      forFlag = LOW;
      moveFlag = LOW;
      stopFlag = LOW;
      break;
    case 0x04: 
      //Call right turn function
      rightTurn();
      break;
    case 0x05: 
      //Call left turn function
      leftTurn();
      break;
    case 0x06: 
      //Call stop function
      stopFlag = HIGH;
      forFlag = LOW;
      backFlag = LOW;
      moveFlag = LOW;
      break;
    case 0x07: 
      takeSample();
      break;
    default:
      //error
      break;
  }
}

void dataReq(){
  Wire.write(count + " = " + sensorData);
}
