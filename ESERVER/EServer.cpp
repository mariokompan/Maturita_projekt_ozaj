#include "EServer.h"

EServer::EServer(): _lWritten(0) { 
  _local_IP = new IPAddress(192,168,12,1);
  _gateway = new IPAddress(192,168,12,1);
  _subnet = new IPAddress(255,255,255,0);
  serverSerial = new SoftwareSerial(13, 12);
  serverSerial->begin(115200);
  serverSerial->setTimeout(10);
  Serial.println("Idem vytvorit _sdCard");
  _sdCard = new ESD();
  Serial.println("Vytvoril som _sdCard");
  _buffer = new EStack(MAX_BUFFER_SIZE);
  _sendingBuffer = new EStack(MAX_SEND_BUFFER_SIZE);
  Serial.println("Vytvoril som _buffer EStack");
  _packetParams = new EStack(MAX_PACKET_PARAM);
}

void EServer::mainThread() {
  unsigned char cycleCounter = 0;
  while(true) {
    switch (cycleCounter) {
      case 0 :       
        checkPackets();         
        break;
      case 1 : {       
        doStuffPacket();
        break;
      }
      case 2 : {
        sendPackets();
        break;
      }
      default: 
        break;
    }
    cycleCounter++;
    if (cycleCounter % TASK_COUNT == 0) cycleCounter = 0;   
  }
}

void EServer::checkPackets() {
  int packetSize = UDP->parsePacket();
  if (packetSize) {
    Serial.print("Received packet! Size: ");
    Serial.println(packetSize); 
    int len = UDP->read(packet, 255);
    if (len > 0)
    {
      packet[len] = '\0';
    }
    String packetFill;
    for( int i = 0; i < 255; i++) {
      packetFill += packet[i];
      if (packet[i] == '@') {
        break;
      }
    }
    Serial.println("Pridal som packet");
    Serial.println(packetFill);

    Serial.print("Pocet packetov v zasobniku : ");
    

    _buffer->push(packetFill);
    Serial.println(_buffer->getSize());
  }
}

void EServer::writeToCard(EDevice* tempDevice) {
  Serial.print("Pocet parametrov v zasobniku : ");
  Serial.println(_packetParams->getSize());
  String fileName = tempDevice->getFileName();
  String idDevice = String(tempDevice->getId());
  Serial.println(idDevice);
  Serial.println(fileName);
  
  if (tryFindDevice(idDevice.toInt()) != nullptr) {
    _sdCard->writeData(fileName, _packetParams); 
    Serial.println("Subor zapisany");
  } else {
    Serial.println("Zariadenie " + idDevice + " nieje autorizovane a teda nema moznost zapisu na sd kartu.");
  }
  _packetParams->clear();
    Serial.print("Pocet parametrov v zasobniku : ");
    Serial.println(_packetParams->getSize());
}

void EServer::writeToDataToSendFile(){
//  _sdCard->writeData(, _packetParams);
  Serial.println("Subor zapisany do dat na odoslanie");
}

void EServer::continueRecievePacket() {
  Serial.println("Hello world");
}

void EServer::printDevices() {
  for (int i = 0; i < _lWritten; i++) {
    Serial.print(_devices[i]->getId());
    Serial.print(" ");
    Serial.println(_devices[i]->getRoom());
  }
}

bool EServer::tryAddDevice(EDevice* dev) {
  if (tryFindDevice(dev->getId()) == nullptr && _lWritten - 1 < DEV_COUNT) {
    EStack tempLast(1);
    int pos = -1;
    _sdCard->getData(dev->getLastSentFileName(),tempLast, pos);
    String vLastSent = "0";
    if (tempLast.isEmpty()) {
      String tempdata = "0";
      tempLast.push(tempdata);
      _sdCard->writeData(dev->getLastSentFileName(), &tempLast);
      _sdCard->closeFile();
    } else {
      vLastSent = tempLast.pop();
    }
    dev->setLastSentPosition(_sdCard->getLastSentPosition(dev->getFileName(),vLastSent.toInt()));
    dev->setLIStored(_sdCard->getLIStored(dev->getFileName(), dev->getLSPos()));
    dev->setLastIDSent(vLastSent);
    _devices[_lWritten] = dev;
  
    _lWritten++;
    return true;
  }
  delete dev;
  return false;
}
EDevice* EServer::tryFindDevice(int id){
  for(int i = 0; i < _lWritten; i++){
    if (_devices[i]->getId() == id)  {
      return _devices[i]; 
    }
  }
  return nullptr;
}

void EServer::parseParams() {
  String packetData = _packetContent;
  String packetUnit = "";

  for (char v : packetData) {
    if (v == '$') {
      _packetParams->push(packetUnit);
      packetUnit = "";
    } else {
      packetUnit += v;
    }
  }
  _packetContent = "";
}

bool EServer::trySendDataToSerial() {
  if (sState == isReady){
      String bufferData = _sendingBuffer->pop();
      serverSerial->println(bufferData);
      Serial.println("Bolo poslan??: " + bufferData);
      return true;
  }
  return false;
}

void EServer::sendPackets() {
  String lastActIDSent[_lWritten] = {}; 
  for (int i = 0; i < _lWritten; i++) {
    if (_sendingBuffer->isFull()) {break;}
    if (_devices[i]->getLIStored() == _devices[i]->getLastIDSent()){continue;}
    
    lastActIDSent[i] = _devices[i]->getLastIDSent();
    int tempPos = _devices[i]->getLSPos();
    _sdCard->getData(_devices[i]->getFileName(),*_sendingBuffer, _devices[i]->getLSPos());
    tempPos = _devices[i]->getLSPos() - tempPos;
    lastActIDSent[i] = String(lastActIDSent[i].toInt() + tempPos);
  }
  while (!_sendingBuffer->isEmpty()) {
    contactServer();
    listenFromServer();
    if (sState == isReady) {
        Serial.println("Posielam na server zo s??boru na odoslanie");
        trySendDataToSerial(); 
        sState = isNotReady;
    } 
    else if (sState == isNotReady){
      Serial.println("Server neodpovedal alebo ma plny buffer");
      return;
    }
    delay(10);
  }       
  /* for (int i = 0; i < _lWritten; i++) {
    _sdCard->replaceLISentFile(_devices[i]->getLastSentFileName(), lastActIDSent[i]);
    Serial.println(lastActIDSent[i]);
  }*/
}

void EServer::doStuffPacket() {
  if (_buffer->isEmpty() && _packetContent.isEmpty()) {
    return;
  }
  if (_packetContent.isEmpty()) {  
    _packetContent = _buffer->pop();
  }
  char packetType = (_packetContent[0]);
  
  if (!_packetContent.isEmpty()){
    switch (packetType) {
      case '1' : {
        parseParams();
        if (_packetParams->isEmpty()) {
          break;
        }      
       _packetParams->pop();
        _packetContent = "";
        EDevice* newDevice = new EDevice();        
        String id = _packetParams->pop();
  
        newDevice->setId(id.toInt());
        newDevice->setIp(_packetParams->pop());   
        newDevice->setRoom(_packetParams->pop());
        newDevice->setFName();
        _sdCard->createFile(newDevice->getFileName());

        newDevice->vypisInfo();
       
        if (tryAddDevice(newDevice)) {
          Serial.println("Zariadenie bolo pridane");
          printDevices();
        } else {
          Serial.println("Zariadenie sa nepodarilo pridat");
        }
        break;
      }
      case '2' : {
        parseParams();
        _packetParams->pop();
        _packetParams->pop();
        int findedId = _packetParams->pop().toInt();
        
        EDevice* findedDev = tryFindDevice(findedId);
        Serial.println(findedDev->getId());
        Serial.println("");
        Serial.println(findedDev->getFileName());
        Serial.println("");
        Serial.println(findedDev->getRoom());
        Serial.println("");
        findedDev->vypisInfo();
        if (findedDev != nullptr) {
          findedDev->incLIS();
          _packetParams->pushFront(findedDev->getLIStored());
          findedDev->vypisInfo();
          writeToCard(findedDev);
          findedDev->vypisInfo();
        }
        else{
          _packetParams->clear();
        }
        break;
     }
      default:
        break;
    }
  }
}


/*Ak je hodnota v packetSent X tak neposlal packet 
 * Ak je hodnota v packetSent Y tak odoslal packet
 * Ak je hodnota v responseSender X tak neposlal packet
 * Ak je hodnota v responseSender D tak poslal dotazovaci
 * Ak je hodnota v responseSender T tak poslal datovy packet
 */

void EServer::listenFromServer() {
  if (serverSerial->available() && (serverSerial->readString()).indexOf("YesImHere")) {
    sState = isReady;
  }
  else if (serverSerial->available() && (serverSerial->readString()).indexOf("PosielamIDaPOS")){
      EDevice* dev;
      dev = tryFindDevice(serverSerial->readString().toInt());
      if (dev != nullptr){
        String tempIDLastSent = serverSerial->readString();
        dev->setLastSentPosition(tempIdLastSent.toInt());
        _sdCard->replaceLISentFile(dev->getLastSentFileName(), tempIDLastSent);
      }    
  }
}

bool EServer::contactServer() {
  serverSerial->println("HelloMyFriend");
}

void EServer::startAccessPoint() {
  Serial.println("Starting access point...");
  wifi = new WiFiClass();
  UDP = new WiFiUDP();
  Serial.println("New ESP prebehlo");
  wifi->softAPConfig(*_local_IP, *_gateway, *_subnet);
  Serial.println("SOFTAPCONFIG prebehol");
  wifi->softAP(AP_SSID, AP_PASS);
  Serial.println(wifi->localIP());

  UDP->begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);
}

EServer::~EServer() {
  for (int i = 0; i < DEV_COUNT; i++) {
      delete _devices[i];
  }
  delete _buffer, _packetParams;
}
