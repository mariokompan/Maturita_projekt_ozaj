#pragma once

#define LOGX(x) Serial.print(x + String(", "))

class StackItem{
    String _data;
    StackItem* _next;

  public:
    StackItem(String data) : _data(data), _next(nullptr) {}
    void setNext(StackItem* next) {
      _next = next;
    }
    String& getData() {return _data;}
    StackItem* getNext() {return _next;}
    ~StackItem() {
      _next = nullptr;
    }
   

  };


class EStack{

  StackItem* _first;
  StackItem* _last;
  unsigned int _max_size;
  unsigned int _size;
  public:

  EStack(unsigned int max_size): _max_size(max_size), _first(nullptr), _last(nullptr), _size(0) {};

  void vypisZasobnika(){
    StackItem* var = _first;
    while(var != nullptr){
     LOGX(var->getData());
     var = var->getNext();
    }
  }

  unsigned int getSize() {return _size;}
  unsigned int getMaxSize() {return _max_size;}

  bool isEmpty() {return _size==0 ? 1 : 0;}
  bool isFull() {return _size == _max_size ? 1 : 0;}


  bool pushFront(String data){
    if (_size +1 > _max_size){
      return false;
    }
    StackItem* temp = new StackItem(data);
    if (!_first){
      _last = temp;
    }
    else{
      temp->setNext(_first);
    }
    _first = temp;
    _size++;
    return true;
  }
  
  bool push(String data) { // push back
    if (_size+1 > _max_size) {
      return false;
    }
    StackItem* temp = new StackItem(data);
    if (!_first) {
      _first = temp;
    }
    else {
    _last->setNext(temp);
    }
    _last = temp;
    _size++;
    return true;
  }

  String pop() {
    if (!_first) {
      return "";
    }
    String firstData = _first->getData();
    StackItem* temp = _first;
    _first = _first->getNext();
    delete temp;
    _size--;
    return firstData;
  }

  String accessData() {
    return _first->getData();
  }

  void clear() {
    while (_first) {
      StackItem* var = _first;
      _first = _first->getNext();
      _size--;
      delete var;
    }
  }

  ~EStack() {
    clear();
    _first = nullptr;
    _last = nullptr;
  }
};
