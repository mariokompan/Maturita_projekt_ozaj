//#include "EServer.h"
#include "ESD.h"
#include "EStack.h"

//EServer* server;

void setup() {
  Serial.begin(57600);
  ESD* sd = new ESD();
  EStack stack = EStack(3);
  String v = "/skuska.txt";
  String b = "aasd";
  for (int i = 0; i < 20; i++) {
    stack.push(b);
    sd->writeData(v, &stack);
  }
  
  sd->closeFile();
  sd->getData(v,stack);
  Serial.print("Zasobnik velkost  = ");
  
  Serial.println(stack.getSize());
  String temp;
  for (int i = 0; i < stack.getSize(); i++) {
    temp = stack.pop();
    Serial.println(temp);
  }
  //server = new EServer();
  //Serial.println("Vytvoril som Localip gateway a subnet");
  //server->startAccessPoint();
  //Serial.println("Spustil som Access Point");
}
 
void loop() {    
  //Serial.println("Spustam hlavne vlakno");
  //server->mainThread();
}
