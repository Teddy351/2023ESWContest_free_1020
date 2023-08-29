//LIBRAY--------------------------
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
//LIBRAY--------------------------

//DEFINE--------------------------
#define LED_BUILTIN 2

#define RXO 96
#define RYO 32
#define LXO 32
#define LYO 32
//DEFINE--------------------------

//FUNCTION------------------------
void easyPull(int section, int incrementAngle, int card);
void pullIncrement(int section, double incrementAngle, int card);
//FUNCTION------------------------

//CLASS---------------------------
WiFiUDP Udp;
//CLASS---------------------------

//STRUCT--------------------------
struct Sheet {
  byte address;
  int M[3] = { 0, 0, 0 };
  int M_pre[3] = { 0, 0, 0 };
};
Sheet Sec1;
Sheet Sec2;
Sheet Sec3;

Sheet Section[3] = { Sec1, Sec2, Sec3 };
//STRUCT--------------------------

//VARIABLE------------------------
const char *ssid = "Continuum";
const char *password = "44084408";
byte packetBuffer[6];
unsigned int localPort = 3333;

double _angleArray[8] = {
  0,
};
float _cardTuner = 0.65;
int _sectionTuner = 0;
int _sections = 2;
//VARIABLE------------------------

void setup() {
  Serial.begin(115200);
  Wire.begin();
  WiFi.softAP(ssid, password);
  Udp.begin(localPort);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
}

void loop() {
  int packetSize = Udp.parsePacket();

  if (packetSize) {

    int len = Udp.read(packetBuffer, 6);

    if (len > 0) packetBuffer[len] = 0;

    float Rangle, Langle;

    if (packetBuffer[1] != 0 && packetBuffer[3] != 0) {
      if (packetBuffer[1] >= LXO) {
        if (packetBuffer[2] >= LYO) {
          //1
          Langle = 90 + atan2(packetBuffer[2] - LYO, packetBuffer[1] - LXO) * 180 / PI;
        } else {
          //2
          Langle = 90 + atan2(packetBuffer[2] - LYO, packetBuffer[1] - LXO) * 180 / PI;
        }
      } else {
        if (packetBuffer[2] >= LYO) {
          //3
          Langle = 90 + atan2(packetBuffer[2] - LYO, packetBuffer[1] - LXO) * 180 / PI;
        } else {
          //4
          Langle = 450 + atan2(packetBuffer[2] - LYO, packetBuffer[1] - LXO) * 180 / PI;
        }
      }

      if (packetBuffer[3] >= RXO) {
        if (packetBuffer[4] >= RYO) {
          //1
          Rangle = 90 + atan2(packetBuffer[4] - RYO, packetBuffer[3] - RXO) * 180 / PI;
        } else {
          //2
          Rangle = 90 + atan2(packetBuffer[4] - RYO, packetBuffer[3] - RXO) * 180 / PI;
        }
      } else {
        if (packetBuffer[4] >= RYO) {
          //3
          Rangle = 90 + atan2(packetBuffer[4] - RYO, packetBuffer[3] - RXO) * 180 / PI;
        } else {
          //4
          Rangle = 450 + atan2(packetBuffer[4] - RYO, packetBuffer[3] - RXO) * 180 / PI;
        }
      }

      byte L_dis = sqrt(sq(packetBuffer[1] - LXO) + sq(packetBuffer[2] - LYO));
      byte R_dis = sqrt(sq(packetBuffer[3] - RXO) + sq(packetBuffer[4] - RYO));

      L_dis = map(L_dis, 0, 25 * sqrt(2), 0, 90);
      R_dis = map(R_dis, 0, 25 * sqrt(2), 0, 90);

      easyPull(1, L_dis, Langle);
      easyPull(2, R_dis, Rangle);

      for (byte i = 4; i < 8; i++) {
        _angleArray[i] = map(_angleArray[i], 0, 200, 0, 170);
      }

      for (byte a = 0; a < 8; a++) {
        Serial.print(_angleArray[a]);
        Serial.print(" ");
      }
      Serial.println();

      Wire.beginTransmission(0x01);
      Wire.write((byte)_angleArray[0]);
      Wire.write((byte)_angleArray[1]);
      Wire.write((byte)_angleArray[2]);
      Wire.write((byte)_angleArray[3]);
      Wire.endTransmission(true);

      Wire.beginTransmission(0x02);
      Wire.write((byte)_angleArray[4]);
      Wire.write((byte)_angleArray[5]);
      Wire.write((byte)_angleArray[6]);
      Wire.write((byte)_angleArray[7]);
      Wire.endTransmission(true);

      Wire.beginTransmission(0x03);
      Wire.write(packetBuffer[5] & 0x0f);
      Wire.endTransmission(true);

      for (byte a = 0; a < 8; a++) {
        _angleArray[a] = 0;
      }
    }
  }
  if (Serial.available() > 0) {
    int sec = Serial.parseInt();
    int A = Serial.parseInt();
    int B = Serial.parseInt();
    int C = Serial.parseInt();
    int D = Serial.parseInt();
    if (sec == 1) {
      Wire.beginTransmission(0x01);
      Wire.write(A);
      Wire.write(B);
      Wire.write(C);
      Wire.write(D);
      Wire.endTransmission(true);
    } else {
      Wire.beginTransmission(0x02);
      Wire.write(A);
      Wire.write(B);
      Wire.write(C);
      Wire.write(D);
      Wire.endTransmission(true);
    }
  }
}

void easyPull(int section, int incrementAngle, int card) {
  float angle = ((card % 90) * M_PI) / 180;
  if (card % 90 == 0) {
    pullIncrement(section, incrementAngle, card / 90);
  } else {
    for (int i = 0; i < section - 1; i++) {
      _angleArray[4 * i + (card / 90)] += incrementAngle * (tan(angle) + 2 * (180 / incrementAngle) - 2) / (1 + tan(angle));
      _angleArray[4 * i + ((card / 90 + 1) % 4)] += incrementAngle * (2 * (180 / incrementAngle) - 1 - (tan(angle) + 2 * (180 / incrementAngle) - 2) / (1 + tan(angle)));
      _angleArray[4 * i + (card / 90)] *= 0.5;
      _angleArray[4 * i + ((card / 90 + 1) % 4)] *= 0.5;
    }
    for (int i = section - 1; i < _sections; i++) {
      float var = (incrementAngle * section) / (i + 1);
      _angleArray[4 * i + (card / 90)] += var * (tan(angle) + 2 * (180 / incrementAngle) - 2) / (1 + tan(angle));
      _angleArray[4 * i + ((card / 90 + 1) % 4)] += var * (2 * (180 / incrementAngle) - 1 - (tan(angle) + 2 * (180 / incrementAngle) - 2) / (1 + tan(angle)));
    }
    //_angleArray[4 * (section - 1) + (card / 90)] += incrementAngle * (tan(angle) + 2 * (180 / incrementAngle) - 2) / (1 + tan(angle));
    //_angleArray[4 * (section - 1) + ((card / 90 + 1) % 4)] += incrementAngle * (2 * (180 / incrementAngle) - 1 - (tan(angle) + 2 * (180 / incrementAngle) - 2) / (1 + tan(angle)));
  }
}
void pullIncrement(int section, double incrementAngle, int card) {
  for (int i = 0; i < section - 1; i++) {
    _angleArray[4 * i + card] += incrementAngle;
    _angleArray[4 * i + ((card + 3) % 4)] += _cardTuner * (incrementAngle * _sectionTuner);
    _angleArray[4 * i + ((card + 1) % 4)] += _cardTuner * (incrementAngle * _sectionTuner);
  }
  for (int i = section - 1; i < _sections; i++) {
    _angleArray[4 * i + card] += (incrementAngle * section) / (i + 1) ;
    _angleArray[4 * i + ((card + 3) % 4)] += _cardTuner * ((incrementAngle * section) / (i + 1));
    _angleArray[4 * i + ((card + 1) % 4)] += _cardTuner * ((incrementAngle * section) / (i + 1));
  }
}
