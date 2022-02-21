#include "EDevice.h"

EDevice::EDevice(int id, String ip, String room): _id(id), _ipaddr(ip), _room(room) {_active = true;}
EDevice::EDevice(): _active(true){};

void EDevice::setSending(bool sending) {
  _sending = sending;
}
void EDevice::setFName(){
  _fileName = "/";
  _fileName += _id+_room;
  _lastSentFileName = _fileName;
  _fileName+= ".txt";
  _lastSentFileName += "LASTSENT.txt";
}
bool EDevice::replyRequest() {
}

EDevice::~EDevice() {}
