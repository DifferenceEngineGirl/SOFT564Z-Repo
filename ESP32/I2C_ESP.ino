char sensorData[10];

void sendCom(int com){
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(com);           // sends command 
  Wire.endTransmission();    // stop transmitting
}

void reqData(){
  Wire.requestFrom(4, 10);
  
  int x = 0;
  
  while (Wire.available()) { // slave may send less than requested
      char c = Wire.read();
      if(c != '\n'){
        sensorData[x] = c;    // receive a byte as character
        x++;
      }else{
        break;
      }
  }
  dataList.push_back(sensorData);
  dataList.unique();
}
