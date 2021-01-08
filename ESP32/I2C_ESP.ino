char sensorData[10];

void sendCom(int com){
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(com);           // sends command 
  Wire.endTransmission();    // stop transmitting
}

void reqData(){
  while (Wire.available()) { // slave may send less than requested
    for(int x = 0; x<10; x++){
      sensorData[x] = Wire.read();    // receive a byte as character
      //Serial.print(c);         // print the character
    }
  }
  dataList.push_back(String(sensorData));
}
