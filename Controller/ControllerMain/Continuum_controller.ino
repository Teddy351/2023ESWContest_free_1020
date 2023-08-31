#include <WiFi.h>
#include <WiFiUdp.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define LXO 32
#define LYO 32
#define RXO 96
#define RYO 32

#define LX1 7
#define LX2 57
#define LY1 7
#define LY2 57

#define RX1 71
#define RX2 121
#define RY1 7
#define RY2 57

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiUDP udp;

struct MAP {
  bool N = 0, S = 0, E = 0, W = 0, B = 0, SW = 0;
};
MAP L;
MAP R;

const char *networkName = "Continuum";
const char *networkPswd = "44084408";
const char *udpAddress = "192.168.4.1";
const int udpPort = 3333;

boolean connected = false;

byte page = 1, _page = 0;

byte Lx = 32, Ly = 32, Rx = 96, Ry = 32;
bool Rf, Lf;
bool pre_RSW = 0, pre_LSW = 0, pre_RB = 0, pre_LB = 0;
byte control = 0x00;

void setup() {
  DP_SETTING();
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    control = Serial.read();
    if (control >> 6) {
      R.B = control >> 5 & 0x01;
      R.SW = control >> 4 & 0x01;
      R.N = control >> 3 & 0x01;
      R.S = control >> 2 & 0x01;
      R.W = control >> 1 & 0x01;
      R.E = control & 0x01;
    } else {
      L.B = control >> 5 & 0x01;
      L.SW = control >> 4 & 0x01;
      L.N = control >> 3 & 0x01;
      L.S = control >> 2 & 0x01;
      L.W = control >> 1 & 0x01;
      L.E = control & 0x01;
    }
  }
  if (page != _page) {
    switch (page) {
      case 1:
        drawMap();
        break;

      default:
        break;
    }
    display.display();
  } else {
    switch (page) {
      case 1:
        if (R.N || R.W || R.E || R.S || L.N || L.W || L.S || L.E || R.SW || L.SW || R.B || L.B) {
          display.drawLine(LXO, LYO, Lx, Ly, BLACK);
          display.drawLine(RXO, RYO, Rx, Ry, BLACK);

          Lx = constrain((L.W == 1) ? Lx-- : Lx, LX1 + 1, LX2 - 2);
          Lx = constrain((L.E == 1) ? Lx++ : Lx, LX1 + 1, LX2 - 2);

          Ly = constrain((L.N == 1) ? Ly-- : Ly, LY1 + 1, LY2 - 2);
          Ly = constrain((L.S == 1) ? Ly++ : Ly, LY1 + 1, LY2 - 2);

          Rx = constrain((R.W == 1) ? Rx-- : Rx, RX1 + 1, RX2 - 2);
          Rx = constrain((R.E == 1) ? Rx++ : Rx, RX1 + 1, RX2 - 2);

          Ry = constrain((R.N == 1) ? Ry-- : Ry, RY1 + 1, RY2 - 2);
          Ry = constrain((R.S == 1) ? Ry++ : Ry, RY1 + 1, RY2 - 2);

          if (R.SW == 1 && pre_RSW == 0) {
            udp.beginPacket(udpAddress, udpPort);
            udp.write(0xf1);
            udp.write(Lx);
            udp.write(Ly);
            udp.write(Rx);
            udp.write(Ry);
            udp.write(0xf0);
            udp.endPacket();
          }
          pre_RSW = R.SW;

          if (L.SW == 1 && pre_LSW == 0) {
            Lx = LXO;
            Ly = LYO;
            Rx = RXO;
            Ry = RYO;
            udp.beginPacket(udpAddress, udpPort);
            udp.write(0xf1);
            udp.write(Lx);
            udp.write(Ly);
            udp.write(Rx);
            udp.write(Ry);
            udp.write(0xf0);
            udp.endPacket();
          }
          pre_LSW = L.SW;

          if (R.B == 1 && pre_RB == 0) {
            static bool Rf = (Rf == 1) ? 0 : 1;
            if (Rf) {
              udp.beginPacket(udpAddress, udpPort);
              udp.write(0xf1);
              udp.write(Lx);
              udp.write(Ly);
              udp.write(Rx);
              udp.write(Ry);
              udp.write(0xf1);
              udp.endPacket();
            } else {
              udp.beginPacket(udpAddress, udpPort);
              udp.write(0xf1);
              udp.write(Lx);
              udp.write(Ly);
              udp.write(Rx);
              udp.write(Ry);
              udp.write(0xf0);
              udp.endPacket();
            }
          }
          pre_RB = R.B;
          if (L.B == 1 && pre_LB == 0) {
            static bool Lf = (Lf == 1) ? 0 : 1;
            if (Lf) {
              udp.beginPacket(udpAddress, udpPort);
              udp.write(0xf1);
              udp.write(Lx);
              udp.write(Ly);
              udp.write(Rx);
              udp.write(Ry);
              udp.write(0xf2);
              udp.endPacket();
            } else {
              udp.beginPacket(udpAddress, udpPort);
              udp.write(0xf1);
              udp.write(Lx);
              udp.write(Ly);
              udp.write(Rx);
              udp.write(Ry);
              udp.write(0xf0);
              udp.endPacket();
            }
          }
          pre_LB = L.B;
          if (R.N || R.W || R.E || R.S || L.N || L.W || L.S || L.E || L.SW) {
            delay(10);
            display.drawLine(LXO, LYO, Lx, Ly, WHITE);
            display.drawLine(RXO, RYO, Rx, Ry, WHITE);

            display.display();
          }
        }
        break;
      default:
        break;
    }
  }
  _page = page;
}

void drawMap() {
  display.drawRect(LX1, LY1, LX2 - LX1, LY2 - LY1, WHITE);
  display.drawRect(RX1, RY1, RX2 - RX1, RY2 - RY1, WHITE);
}

void DP_SETTING() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;)
      ;
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();
  Show("Loading.", 0, 27);
  display.display();
  delay(800);

  connectToWiFi(networkName, networkPswd);
  Show("Loading..", 0, 27);
  display.display();
  delay(500);
  Show("Loading...", 0, 27);
  display.display();
  delay(1000);

  Clear();
  Show("Done!", 0, 27);
  display.display();
  delay(3000);
  Clear();
}

void connectToWiFi(const char *ssid, const char *pwd) {
  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, pwd);
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      udp.begin(WiFi.localIP(), udpPort);
      connected = true;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      connected = false;
      ESP.restart();
      break;
    default: break;
  }
}

void Show(String sentance, int x, int y) {
  display.setCursor(x, y);
  display.println(sentance);
}

void Clear() {
  display.clearDisplay();
  display.display();
}

void Erase(int x, int y, int w, int h) {
  display.fillRect(x, y, w, y, BLACK);
}
