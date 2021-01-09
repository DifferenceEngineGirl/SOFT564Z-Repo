#include <Wire.h>

void i2cSetup(void){
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(command);      // register event
  Wire.onRequest(dataReq);
  Serial.begin(9600);           // start serial for output
  Serial.println("Initialised");
}

void command(int numByte){

  unsigned char comNo;
  
  while(Wire.available()){
    comNo = Wire.read();
  }
  
  Serial.println(comNo, HEX);

  switch(comNo){
    case 0x01: 
      //Call do square function
      moveFlag = HIGH;
      stopFlag = LOW;
      break;
    case 0x02: 
      //Call forward function
      dirFlag = FOR;
      moveFlag = LOW;
      stopFlag = LOW;
      break;
    case 0x03: 
      //Call backward function
      dirFlag = BACK;
      moveFlag = LOW;
      stopFlag = LOW;
      break;
    case 0x04: 
      //Call right turn function
      rightFlag = HIGH;
      break;
    case 0x05: 
      //Call left turn function
      leftFlag = HIGH;
      break;
    case 0x06: 
      //Call stop function
      stopFlag = HIGH;
      moveFlag = LOW;
      break;
    case 0x07: 
      sampleFlag = HIGH;
      break;
    default:
      //error
      break;
  }
}

void dataReq(){
  String data;
  data += count;
  data += " = ";
  data += sensorData;
  data += '\n';
  Serial.println(data);
  Wire.write(data.c_str());
}
