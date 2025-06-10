#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "Danial"
#define STAPSK "tanoswasright"
#endif

// Motor control pins (emulated L9110)
struct MotorPins { int pwmPin, dirPin; } motors[4] = {
  {12, 14}, {13, 15}, {3, 1}, {4, 5}
};

const int SPEED = 180; 

// #include <math.h>
// const int x_out = 1; /* connect x_out of module */
// const int y_out = 3; /* connect y_out of module */
// const int z_out = 16; /* connect z_out of module */


const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

// Minimal frontend with JS to capture keyboard inputs and POST in background
const String postForms = "<html>\
  <head>\
    <title>Drone Controller</title>\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
    <style>\
      body { background-color: #cccccc; font-family: Arial, sans-serif; color: #000088; text-align: center; padding: 20px; }\
      button { width: 60px; height: 60px; font-size: 20px; margin: 5px; border-radius: 10px; }\
      .control-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; justify-items: center; max-width: 200px; margin: 20px auto; }\
      .group { margin-bottom: 20px; }\
    </style>\
    <script>\
      let interval = null;\
      function sendCommand(cmd) {\
        fetch('/controlls/', {\
          method: 'POST',\
          headers: { 'Content-Type': 'text/plain' },\
          body: cmd\
        }).catch(() => alert('Failed to send command'))\
      }\
      function start() {\
        if (!interval) {\
          interval = setInterval(() => { sendCommand('heartbeat'); }, 2000);\
        }\
      }\
      function stop() {\
        clearInterval(interval);\
        interval = null;\
      }\
    </script>\
  </head>\
  <body>\
    <h2>Drone Controller</h2>\
    <p>Control the drone using buttons</p>\
    <div class=\"group\">\
      <div class=\"control-grid\">\
        <div></div><button onclick=\"sendCommand('w')\">W</button><div></div>\
        <button onclick=\"sendCommand('a')\">A</button><button onclick=\"sendCommand('s')\">S</button><button onclick=\"sendCommand('d')\">D</button>\
      </div>\
    </div>\
    <div class=\"group\">\
      <div class=\"control-grid\">\
        <button onclick=\"sendCommand('z')\">Z</button>\
        <div></div>\
        <button onclick=\"sendCommand('x')\">X</button>\
      </div>\
    </div>\
    <div class=\"group\">\
      <button onclick=\"start()\" style=\"background-color:green;color:white;\">Start</button>\
      <button onclick=\"stop()\" style=\"background-color:red;color:white;\">Stop</button>\
    </div>\
  </body>\
</html>";


void handleRoot() { 
  server.send(200, "text/html", postForms); 
}

void handleControlls() {
  if (server.method() != HTTP_POST) { 
    server.send(405, "text/plain", "Method Not Allowed"); 
    return;
  }

  String command = server.arg("plain");
  // Serial.print("Received Command: ");
  // Serial.println(command);  // This simulates drone control

  // You could expand this block to interpret and act on the command
  // Example:
  // if (command == "w") { moveForward(); } 
  if (command == "z") { moveUp(); } 
 
  server.send(200, "text/plain", "OK"); 
}

void moveUp() {
  // Serial.print("Received Command: ");
  // Serial.println("moveUp");  

  for (int i=0; i<4; i++) {
    // pinMode(motors[i].pwmPin, OUTPUT);
    // pinMode(motors[i].dirPin, OUTPUT);

  // analogWrite(13, 0);                  
  analogWrite(motors[i].pwmPin, 255);             
  analogWrite(motors[i].dirPin, SPEED);            
  // analogWrite(14, 0);    
  }


  delay(5000);
  for (int i=0; i<4; i++) {
    analogWrite(motors[i].pwmPin, 0);
    analogWrite(motors[i].dirPin, 0);
  }
}

// Removed handleForm (unused for this project)

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) { 
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  // Serial.begin(115200);

  WiFi.begin(ssid, password);
  // Serial.println("");

  for (int i=0; i<4; i++) {
    pinMode(motors[i].pwmPin, OUTPUT);
    pinMode(motors[i].dirPin, OUTPUT);
  }

  delay(500);
  for (int i=0; i<4; i++) {
    analogWrite(motors[i].pwmPin, 0);
    analogWrite(motors[i].dirPin, 0);
  }
  delay(500);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }

  // Serial.println("");
  // Serial.print("Connected to ");
  // Serial.println(ssid);
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());

  // if (MDNS.begin("esp8266")) {
  //   Serial.println("MDNS responder started");
  // }

  server.on("/", handleRoot);
  server.on("/controlls/", handleControlls);
  server.onNotFound(handleNotFound);
  server.begin();
  // Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
