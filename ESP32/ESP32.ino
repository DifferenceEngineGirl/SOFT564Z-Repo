/*********
  Based upon code by Rui Santos
  Complete project details at https://randomnerdtutorials.com

  Altered by Alice Tuffen
*********/

// Load Wi-Fi library
#include <WiFi.h>
// Load wire library
#include <Wire.h>
// Load list library
#include <list>

#if 0
// Replace with your network credentials
const char* ssid = "Alice's phone";
const char* password = "tuffen12";
#else
// Replace with your network credentials
const char* ssid = "ESP-32";
const char* password = "espaccess";
#endif

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String moveState = "off";
String sensorState = "off";
String dataState = "off";
String forwardState = "off";
String backwardState = "off";

// Assign output variables to GPIO pins
const int output26 = 2;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//Create list to hold data
std::list<String> dataList;

hw_timer_t * readTimer = NULL;

volatile int reqFlag;

void setFlag() {
  reqFlag = HIGH;
}

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)

  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

#if 0
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
#else
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
#endif

  //prescale 80 for 1MHz clock
  readTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(readTimer, &setFlag, true);
  
  //Alarm every second
  timerAlarmWrite(readTimer, 1000000, true);
  timerAlarmEnable(readTimer);

  reqFlag = LOW;
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");

            // turns the GPIOs on and off
            if (header.indexOf("GET /Move") >= 0) {
              Serial.println("Buggy moving");
              moveState = "on";
              forwardState = "off";
              backwardState = "off";
              digitalWrite(output26, HIGH);
              sendCom(0x01); //send go command
            } else if (header.indexOf("GET /Stop") >= 0) {
              Serial.println("Stopped");
              moveState = "off";
              forwardState = "off";
              backwardState = "off";
              digitalWrite(output26, LOW);
              sendCom(0x06); //send stop command
            } else if (header.indexOf("GET /Forward") >= 0) {
              Serial.println("Buggy moving forward");
              forwardState = "on";
              moveState = "off";
              backwardState = "off";
              sendCom(0x02); //send forwards command
            } else if (header.indexOf("GET /Backward") >= 0) {
              Serial.println("Buggy moving backward");
              backwardState = "on";
              forwardState = "off";
              moveState = "off";
              sendCom(0x03); //send backwards command
            } else if (header.indexOf("GET /Right") >= 0) {
              Serial.println("Buggy turning right 90 degrees");
              sendCom(0x04); //send turn right command;
            } else if (header.indexOf("GET /Left") >= 0) {
              Serial.println("Buggy turning left 90 degrees");
              sendCom(0x05); //send turn left command
            } else if (header.indexOf("GET /SensorRead") >= 0) {
              Serial.println("Taking a sensor reading");
              sendCom(0x07); //send sensor read command;
              reqData();
            }

            if (header.indexOf("GET /DataGet") >= 0) {
              Serial.println("Data Fetched");
              dataList.unique();
              client.println("Content-type:text/plain");
              client.println("Content-Disposition: attachment; filename=\"data.txt\"");
              client.println("Connection: close");
              client.println();
              client.println("Data:\n");
              for (std::list<String>::iterator it = dataList.begin(); it != dataList.end(); ++it) {
                String sendData = *it;
                client.println(sendData);
                //dataList.pop_front();
              }
            } else {
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              // CSS to style the on/off buttons
              // Feel free to change the background-color and font-size attributes to fit your preferences
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println(".button2 {background-color: #555555;}</style></head>");

              // Web Page Heading
              client.println("<body><h1>ESP32 Web Server</h1>");

              // Display current state, and ON/OFF buttons for Move
              client.println("<p>Move - State " + moveState + "</p>");
              // If the moveState is off, it displays the ON button
              if (moveState == "off") {
                client.println("<p><a href=\"/Move\"><button class=\"button\">Move</button></a></p>");
              } else {
                client.println("<p><a href=\"/Stop\"><button class=\"button button2\">Halt</button></a></p>");
              }

              // Display current state, and ON/OFF buttons for Forward
              client.println("<p>Forwards - State " + forwardState + "</p>");
              // If the forwardState is off, it displays the ON button
              if (forwardState == "off") {
                client.println("<p><a href=\"/Forward\"><button class=\"button\">Move Forwards</button></a></p>");
              } else {
                client.println("<p><a href=\"/Stop\"><button class=\"button button2\">Halt</button></a></p>");
              }

              // Display current state, and ON/OFF buttons for Backward
              client.println("<p>Backwards - State " + backwardState + "</p>");
              // If the backwardState is off, it displays the ON button
              if (backwardState == "off") {
                client.println("<p><a href=\"/Backward\"><button class=\"button\">Move Backwards</button></a></p>");
              } else {
                client.println("<p><a href=\"/Stop\"><button class=\"button button2\">Halt</button></a></p>");
              }

              // Display current state, and ON/OFF buttons for Right Turn
              client.println("<p>Right Turn</p>");
              client.println("<p><a href=\"/Right\"><button class=\"button\">Turn Right 90</button></a></p>");

              // Display current state, and ON/OFF buttons for Left Turn
              client.println("<p>Left Turn</p>");
              client.println("<p><a href=\"/Left\"><button class=\"button\">Turn Left 90</button></a></p>");

              // Display current state, and ON/OFF buttons for Sensor Read
              client.println("<p>Sensor Read</p>");
              client.println("<p><a href=\"/SensorRead\"><button class=\"button\">Read</button></a></p>");

              // Display current state, and ON/OFF buttons for Get Data
              client.println("<p>Get Data</p>");
              client.println("<p><a href=\"/DataGet\"><button class=\"button\">Fetch</button></a></p>");

              // The HTTP response ends with another blank line
              client.println("</body></html>");
              client.println();
            }
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  if (reqFlag == HIGH) {
    reqFlag = LOW;
    reqData();
  }
}
