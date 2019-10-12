#ifndef Mqtt485_h
#define Mqtt485_h

#include "Arduino.h"
#include "Simple485.h"
#include <LinkedList.h>

class Mqtt485
{
  public:
    Mqtt485() {};
    Mqtt485(Simple485 * rs485, uint8_t gateaddr, void(*onMessage)(String topic, uint8_t len, uint8_t * bytes));
    void publish(String topic, uint8_t len, uint8_t * bytes);
    void publish(String topic, String message);
    void subscribe(String topic);
    void unsubscribe(String topic);
    void receive(uint8_t len, uint8_t * bytes);
  private:
    Simple485 * rs485;
    uint8_t gateaddr = 0;
    void(*onMessage)(String topic, uint8_t len, uint8_t * bytes) = nullptr;
    LinkedList<String> subscribed = LinkedList<String>();
};

bool strbytescmp(String str, uint8_t * bytes, uint8_t len);

#endif
