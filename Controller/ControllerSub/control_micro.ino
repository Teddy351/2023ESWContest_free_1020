#include <Btn.h>

Btn RY(A0);
Btn RX(A1);
Btn LY(A2);
Btn LX(A3);

Btn LSW(2, 1);
Btn RSW(3, 1);

Btn LB(4, 1);
Btn RB(5, 1);

#define Uart Serial1

void setup() {
  Serial.begin(115200);
  Uart.begin(9600);

  Serial.println("Start");
}

bool RN = 0, RW = 0, RS = 0, RE = 0;
bool LN = 0, LW = 0, LS = 0, LE = 0;

bool Rbtn = 0, Lbtn = 0, Rsw = 0, Lsw = 0;

byte R_byte = 0x40, L_byte = 0x00;
byte R_c = 0x40, L_c = 0x00;

void loop() {
  Btn_Working();
  if (R_byte != R_c || L_byte != L_c) {
    Uart.write(L_byte);
    Uart.write(R_byte);
  }
  R_c = R_byte;
  L_c = L_byte;
}
void Btn_Working() {
  switch (RB.State()) {
    case FALL:
      R_byte |= 0x20;
      break;
    case RISE:
      R_byte &= ~0x20;
      break;
    default:
      break;
  }
  switch (LB.State()) {
    case FALL:
      L_byte |= 0x20;
      break;
    case RISE:
      L_byte &= ~0x20;
      break;

    default:
      break;
  }
  switch (RSW.State()) {
    case FALL:
      R_byte |= 0x10;
      break;
    case RISE:
      R_byte &= ~0x10;
      break;

    default:
      break;
  }
  switch (LSW.State()) {
    case FALL:
      L_byte |= 0x10;
      break;
    case RISE:
      L_byte &= ~0x10;
      break;

    default:
      break;
  }

  switch (LX.Joy_X()) {
    case 0x00:
      L_byte |= 0x02;
      break;
    case 0x01:
      L_byte &= ~0x03;
      break;
    case 0x02:
      L_byte |= 0x01;
      break;
  }
  switch (LY.Joy_Y()) {
    case 0x02:
      L_byte |= 0x08;
      break;
    case 0x01:
      L_byte &= ~0x0C;
      break;
    case 0x00:
      L_byte |= 0x04;
      break;
  }

  switch (RX.Joy_X()) {
    case 0x00:
      R_byte |= 0x02;
      break;
    case 0x01:
      R_byte &= ~0x03;
      break;
    case 0x02:
      R_byte |= 0x01;
      break;
  }
  switch (RY.Joy_Y()) {
    case 0x02:
      R_byte |= 0x08;
      break;
    case 0x01:
      R_byte &= ~0x0C;
      break;
    case 0x00:
      R_byte |= 0x04;
      break;
  }
}
