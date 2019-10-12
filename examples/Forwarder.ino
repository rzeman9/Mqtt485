#include "Simple485.h"
#include <AltSoftSerial.h>
#include "Mqtt485.h"

// AltSoftSerial
// Arduino Uno
// TX 9 RX 8

#define BAUDRATE 9600
#define REDEPIN 7

#define GATEADDR 27

Simple485 rs485;
Mqtt485 mqtt485;
AltSoftSerial altSerial;

String s = "";
String topic = "";
uint8_t addr = 0;

void onMessage(String topic, uint8_t len, uint8_t * bytes) {
  Serial.println("Received " + String(len+1) + " bytes in topic " + topic + ":");
  Serial.write(bytes, len);
  Serial.println("");
  delete [] bytes;  
}

void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial);
  altSerial.begin(BAUDRATE);

  randomSeed(analogRead(0));
  addr = random(1, 256);
  rs485 = Simple485(&altSerial, addr, REDEPIN);

  mqtt485 = Mqtt485(&rs485, GATEADDR, &onMessage);
  
  
  Serial.println("Ready. Addr " + String(addr));
}

void loop() {
  rs485.loop();

  // Serial to Mqtt485
  char c;
  while (Serial.available() > 0) {
    c = Serial.read();
    if (c == 10) {
      if (s.startsWith("subscribe")) {
        Serial.println("Subscribing " + s.substring(10));
        mqtt485.subscribe(s.substring(10));
      } else if (s.startsWith("topic")) {
        topic = s.substring(6);
        Serial.println("Changing topic to " + topic);
      } else {
        Serial.println("Publishing " + s);
        mqtt485.publish(topic, s);
      }
      s = "";
    } else {
      s += c;
    }
  }

  // RS485 to Mqtt485
  while (rs485.received() > 0) {
    Message m = rs485.read();
    mqtt485.receive(m.len, m.bytes);
  }
  
}
