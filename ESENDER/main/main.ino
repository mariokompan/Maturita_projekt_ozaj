#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "EStack.h"

#define MAX_BUFFER_SIZE 30

SoftwareSerial ServerSerial(14, 13); //D7, D8 || RX, TX
WiFiClient SenderClient;
PubSubClient client(SenderClient);

char msgmqtt[50];

EStack* eBuffer;

void reconnectmqttserver() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  String MQTT_DATA = "";
  for (int i=0;i<length;i++) {
    MQTT_DATA += (char)payload[i];
  }
}

void setup() {
  
  ServerSerial.begin(115200);
  ServerSerial.setTimeout(10);
  Serial.begin(115200);
  eBuffer = new EStack(MAX_BUFFER_SIZE);
  /*WiFi.disconnect();
  delay(3000);
  Serial.println("START");
   WiFi.begin("matej","password");
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print("..");

  }
  Serial.println("Connected");
  Serial.println("Your IP is");
  Serial.println((WiFi.localIP().toString()));
  client.setServer("192.168.2.99", 1883);
  client.setCallback(callback);*/
}

void loop()
{
  //if(ServerSerial.available() > 0){
    String temp = ServerSerial.readString();
    if (temp.length() > 0) {
      if (temp.indexOf("HelloMyFriend")){
        ServerSerial.println("YesImHere"); //ak je dostupny server
        Serial.println(temp);
      }
      if (temp.indexOf(".txt")){
        ServerSerial.println("PosielamIDaPOS"); //ak poslal na server
        Serial.println(temp);
      }
    }
    /*
    else if ((ServerSerial.readString()).concat("^")){
      digitalWrite(14, HIGH);
      Serial.print("Dostal som udaje: ");
      Serial.println(ServerSerial.readString());
    }
    else{
    }*/
    //Serial.println(temp);
  
    /*if (!client.connected()) {
      reconnectmqttserver();
    }
    client.loop();
    snprintf (msgmqtt, 50, "%f","");
    client.publish("DHT21-Temperature", msgmqtt);
    snprintf (msgmqtt, 50, "%f","");
    client.publish("DHT21-Humidity", msgmqtt);
    delay(10000);*/
}
