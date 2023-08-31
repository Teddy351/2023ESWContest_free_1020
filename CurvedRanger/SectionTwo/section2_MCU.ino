#include <Step.h>
#include <Wire.h>

#define STEP1 2
#define DIR1 3

#define STEP2 4
#define DIR2 5

#define STEP3 6
#define DIR3 7

#define STEP4 8
#define DIR4 9

#define IIC_ADDR 0x02

Step_M A(DIR1, STEP1);
Step_M B(DIR2, STEP2);
Step_M C(DIR3, STEP3);
Step_M D(DIR4, STEP4);

byte buf[4];

void setup() {
  Serial.begin(115200);
  Serial.println("Serial Check");

  A.setDuration(2500);
  B.setDuration(2500);
  C.setDuration(2500);
  D.setDuration(2500);

  Wire.begin(IIC_ADDR);
  Wire.onReceive(Que);
}

void loop() {
  if (Serial.available()) {
    int a = Serial.parseInt();
    A.setStep(a);
  }

  if (A.Whether() || B.Whether() || C.Whether() || D.Whether()) {
    A.Move();
    B.Move();
    C.Move();
    D.Move();
  }
}

void Que() {
  byte cnt = 0;
  while (Wire.available()) {
    buf[cnt++] = Wire.read();
  }

  A.setStep(A.preStep(angle2step(buf[0])));
  B.setStep(B.preStep(angle2step(buf[1])));
  C.setStep(C.preStep(angle2step(buf[2])));
  D.setStep(D.preStep(angle2step(buf[3])));
}

int angle2step(long angle) {
  return (float)angle / 0.05;
}
