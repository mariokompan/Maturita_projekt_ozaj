#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WifiUdp.h>
#include "EStack.h"

#define STASSID "EServer"
#define STAPSK  "AhojMajo"
#define PORT 4210

#define DHTPIN D2 
#define DHTTYPE DHT22

#define ROOM_NAME "Spalna"
#define ID_SENSOR "54321"
#define IP_ADDRESS "192.168.12.1"

#define SLEEP_TIME 2e6

#define AUTH_TYPE "1"
#define DATA_PACKET "2"

DHT dht(DHTPIN, DHTTYPE);
WiFiUDP udp;

  float humidity;
  float temperature;
  float vABat;
  float vOutBattery;
  float vInBattery;
  unsigned int cPacket = 0;

EStack* dataPacket = new EStack(15);

void createData(String data[], unsigned char count) {
  for (int i = 0; i < count; i++) {
    dataPacket->push(data[i]);
  }
  //Serial.println("Vytvoril som packetData");
}

void sendPacket() {
  String data;
  udp.beginPacket("192.168.12.1", PORT);
  while(!dataPacket->isEmpty()) {
    data += dataPacket->pop();
    data += "$";
  }
  data+= "@";
  //Serial.print("Posielam Packet : ");
  //Serial.println(data);
  udp.write(data.c_str());
  udp.endPacket();
  dataPacket->clear();
}

void getBatteryCapacity() {
  vABat = analogRead(A0);
  vOutBattery = (vABat * 3.30) / 1024.00;  
  vInBattery = vOutBattery / (1000.00/(10000.00+1000.00));
}

void setup() {
  pinMode(A0,INPUT);
  Serial.begin(115200); 
  udp.begin(PORT);
  dht.begin();
  
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  String data[] = {AUTH_TYPE, ID_SENSOR, IP_ADDRESS, ROOM_NAME};

  createData(data,4);
  sendPacket();

}

void loop() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C "));

  String tempS = String(temperature);
  String tempH = String(humidity);
  String fileName = "/";
  fileName += ROOM_NAME;
  fileName += ID_SENSOR;
  fileName += ".txt";
  getBatteryCapacity();
  String cBatt = String(vInBattery);
  cPacket++;
  String packetC = String(cPacket);
  String data[] = {DATA_PACKET, fileName , ID_SENSOR, ID_SENSOR, tempS, tempH, cBatt, packetC};

  createData(data, 8);
  sendPacket();
  delay(10);
  ESP.deepSleep(SLEEP_TIME); 
  //ESP.deepSleep(2e6);


}
