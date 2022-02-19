#include "EServer.h"

EServer* server;

void setup() {
  Serial.begin(57600);
  server = new EServer();
  Serial.println("Vytvoril som Localip gateway a subnet");
  server->startAccessPoint();
  Serial.println("Spustil som Access Point");
}
 
void loop() {    
  //Serial.println("Spustam hlavne vlakno");
  server->mainThread();
}
