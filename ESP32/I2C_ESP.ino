char sensorData[10];

void sendCom(int com){
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(com);           // sends command 
  Wire.endTransmission();    // stop transmitting
}

void reqData(){
  while (Wire.available()) { // slave may send less than requested
    sensorData = Wire.read();    // receive a byte as character
    Serial.print(c);         // print the character
  }
}
