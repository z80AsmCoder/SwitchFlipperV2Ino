/*
 * Modified by Ashley to add /flip/12
  Arduino Yún Bridge example

  This example for the YunShield/Yún shows how 
  to use the Bridge library to access the digital and
  analog pins on the board through REST calls.
  It demonstrates how you can create your own API when
  using REST style calls through the browser.

  Possible commands created in this shetch:

  "/arduino/digital/13"     -> digitalRead(13)
  "/arduino/digital/13/1"   -> digitalWrite(13, HIGH)
  "/arduino/analog/2/123"   -> analogWrite(2, 123)
  "/arduino/analog/2"       -> analogRead(2)
  "/arduino/mode/13/input"  -> pinMode(13, INPUT)
  "/arduino/mode/13/output" -> pinMode(13, OUTPUT)

  This example code is part of the public domain

  http://www.arduino.cc/en/Tutorial/Bridge

*/

#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

// Listen to the default port 5555, the Yún webserver
// will forward there all the HTTP requests you send
BridgeServer server;

//Pins to be used in the Mobile App.---Used only for getting state
int arPins[] = {12, 11, 10, 9, 8};
 
void setup() {
  Serial.begin(9600);
  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  
  //Using all Digital Pins as output
  for(int i = 0; i <=12; i++){
    pinMode(i, OUTPUT);
  }

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
 // server.listenOnLocalhost();
  server.begin();
}

void loop() {
  // Get clients coming from server
  BridgeClient client = server.accept();

  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50); // Poll every 50ms
}

void process(BridgeClient client) {
  // read the command
  String command = client.readStringUntil('/');
  Serial.println(command);
  // is "digital" command?
  if (command == "digital") {
    digitalCommand(client);
    getState(client);
  }

  // is "analog" command?
  if (command == "analog") {
    analogCommand(client);
  }

  if(command == "state") {
    Serial.println("in it");
    getState(client);
  }

  if(command == "flip"){
    flip(client);
  }
  
}

void digitalCommand(BridgeClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  } else {
    value = digitalRead(pin);
  }


  /*  
  // Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);
  */
  
  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));

  //getState(client);
}

void analogCommand(BridgeClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  if (client.read() == '/') {
    // Read value and execute command
    value = client.parseInt();
    analogWrite(pin, value);

    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  } else {
    // Read analog pin
    value = analogRead(pin);

  
    // Send feedback to client
    client.print(F("Pin A"));
    client.print(pin);
    client.print(F(" reads analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  }
}

void getState(BridgeClient client){

  String json = "{";
  int arraysize = sizeof(arPins)/sizeof(arPins[0]); //1 int is 2Bytes. sizeof() returns number of bytes in array
   
  for(int i = 0; i<arraysize; i++){
    json += "\"D" + (String) arPins[i] + "\":" + digitalRead(arPins[i]);
    if(i != (arraysize - 1)){
      json += ",";
    }
  }
  json += "}";
  client.print(json);
}

void flip(BridgeClient client){
  int pin;
  pin = client.parseInt(); //read pin number from uri
  digitalWrite(pin, !digitalRead(pin));
  client.print(digitalRead(pin));
}
