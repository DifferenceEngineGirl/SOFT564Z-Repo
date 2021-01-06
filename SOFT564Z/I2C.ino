#include <Wire.h>

void i2cSetup(void){
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(command); // register event
  Serial.begin(9600);           // start serial for output
}

void command(int comNo){

  switch(comNo){
    case 0x01: 
      //Call do square function
      break;
    case 0x02: 
      //Call forward function
      break;
    case 0x03: 
      //Call backward function
      break;
    case 0x04: 
      //Call right turn function
      break;
    case 0x05: 
      //Call left turn function
      break;
    case 0x06: 
      //Call stop function
      break;
    case 0x07: 
      takeSample();
      break;
    default:
      //error
      break;
  }
}
