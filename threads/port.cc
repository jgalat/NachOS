#include "copyright.h"
#include "synch.h"
#include "port.h"
#include "system.h"



Port::Port(const char *debugName)
{
  name = debugName;
  msgs = new List<int>;
  lock = new Lock("portLock");
  Receivers = new Condition("PortReceivers", lock);
  Senders = new Condition ("PortSenders", lock);
}

Port::~Port()
{
  delete lock;
  delete msgs;
  delete Receivers;
  delete Senders;
  
}

void Port::Send(int msg)
{
  lock->Acquire();
  msgs->Append(msg);
  Receivers->Signal();
  Senders->Wait();
  lock->Release();
}

void Port::Receive(int *msg)
{
  ASSERT(msg != NULL);
  
  lock->Acquire();
  while(msgs->IsEmpty())
    Receivers->Wait();
  *msg = msgs->Remove();
  Senders->Signal();
  lock->Release();
}
