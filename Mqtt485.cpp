#include "Mqtt485.h"

Mqtt485::Mqtt485(Simple485 * _rs485, uint8_t _gateaddr, void(*_onMessage)(String topic, uint8_t len, uint8_t * bytes)) {
  rs485 = _rs485;
  gateaddr = _gateaddr;
  onMessage = _onMessage;
}

void Mqtt485::subscribe(String topic) {
  rs485->send(gateaddr, "mqsub" + topic);
  for (uint8_t i = 0; i < subscribed.size(); i++) {
    if (subscribed.get(i).equals(topic)) return;
  }
  subscribed.add(topic);
}

void Mqtt485::unsubscribe(String topic) {
  rs485->send(gateaddr, "mquns" + topic);
  for (uint8_t i = 0; i < subscribed.size(); i++) {
    if (subscribed.get(i).equals(topic)) {
      subscribed.remove(i);
      i--;
    }
  }
}

void Mqtt485::publish(String topic, uint8_t len, uint8_t * bytes) {
  uint8_t * buff = new uint8_t[len + 5 + topic.length() + 1];
  String p = "mqpub" + topic;
  p.getBytes(buff, p.length()+1);
  memcpy(buff + p.length() + 1, bytes, len);
  rs485->send(gateaddr, len + 5 + topic.length() + 1, buff);
  delete [] buff;
}

void Mqtt485::publish(String topic, String message) {
  uint8_t * buff = new uint8_t[message.length() + 1];
  message.getBytes(buff, message.length()+1);
  publish(topic, message.length()+1, buff);
}

void Mqtt485::receive(uint8_t len, uint8_t * buff) {
  if (strbytescmp("mqrcv", buff, len)) {              // receiving mqtt message
    Serial.println("receiving mqrcv");
    uint8_t i = 5;
    String topic = "";
    while (i < len) {
      if (buff[i] != 0) topic += (char)buff[i];
      else break;
      i++;
    }
    uint8_t * payload = new uint8_t[len - i];
    memcpy(payload, buff + i + 1, len - i);
    onMessage(topic, len - i, payload);
  } else if (strbytescmp("mqini", buff, len)) {
    Serial.println("receiving mqini");
    for (uint8_t i = 0; i < subscribed.size(); i++) {
      rs485->send(gateaddr, "mqsub" + subscribed.get(i));
    }
  }
}

bool strbytescmp(String str, uint8_t * bytes, uint8_t len) {
  if (str.length()+1 > len) return false;
  uint8_t * buff = new uint8_t[str.length() + 1];
  str.getBytes(buff, str.length()+1);
  bool c = memcmp(buff, bytes, str.length());
  delete [] buff;
  return c == 0;
}
