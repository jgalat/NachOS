#ifndef PORT_H
#define PORT_H


#include "copyright.h"
#include "thread.h"
#include "list.h"

class Lock;
class Condition;

class Port {
  public:
    Port(const char *debugName);
    ~Port();
    void Send(int msg);
    void Receive(int *msg);

  
  private:
  const char *name;
  List<int> *msgs;
  Condition *Receivers, *Senders;
  Lock *lock;
};



#endif // PORT_H
