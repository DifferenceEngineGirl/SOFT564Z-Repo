//Based off inbuilt arduino example code

//Include the I2C library
#include <Wire.h>

//Setup I2C
void i2cSetup(void){
  Wire.begin(4);                // Join i2c bus with address #4
  Wire.onReceive(command);      // Register command event
  Wire.onRequest(dataReq);      // Register request event
}

//Command event
void command(int numByte){

  //Create variable to hold the command number
  unsigned char comNo;

  //Read the command number from the I2C
  while(Wire.available()){
    comNo = Wire.read();
  }

  //Switch to the appropriate case based on the command number supplied
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

//Request event
void dataReq(){
  //Create string to hold the data to be sent
  String data;
  //Add the desired data to the string
  data += count;
  data += " = ";
  data += sensorData;
  data += '\n';
  //Send the data across the I2C to the esp32.
  Wire.write(data.c_str());
}
