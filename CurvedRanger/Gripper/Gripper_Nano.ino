#include <Wire.h>

#define IN1 3
#define IN2 4
#define IIC_ADDR 0x03  //addres

#define STOP 0x00
#define RUN 0x01
#define REVERSE 0x02

void setup() {
  Serial.begin(115200);
  Serial.println("Serial Start");

  Wire.begin(IIC_ADDR);
  Wire.onReceive(receiveGripper);
}

void loop() {
}

void receiveGripper() {
  while (Wire.available()) {
    if (Wire.read() == STOP) { 
      analogWrite(IN1, 0);
      analogWrite(IN2, 0);
    } else if (Wire.read() == RUN) {
      analogWrite(IN1, 255);
      analogWrite(IN2, 0);
    } else if (Wire.read() == REVERSE) {
      analogWrite(IN1, 0);
      analogWrite(IN2, 255);
    }
  }
}
