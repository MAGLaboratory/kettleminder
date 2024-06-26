#include <Arduino.h>
#include <U8g2lib.h>
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

WiFiServer server(80);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 12, /* data=*/ 14, /* reset=*/ U8X8_PIN_NONE);   // ESP32 Thing, pure SW emulated I2C

#define C_NUM_TBI (8U)
#define C_NUM_NUM_TBI (5U)

union topBarItem_t
{
  struct
  {
      uint8_t trip: 1;
      uint8_t tripD: 1;
      uint8_t sel: 1;
      uint8_t dcon: 1;
  } v;
  uint8_t b;
};

topBarItem_t topBarItem[C_NUM_TBI];

void setup() {
  Serial.begin(115200U);
  u8g2.begin();
  u8g2.setFont(u8g2_font_7x14_tf);
  u8g2.drawStr(3U, 13U, "Booting...");
  u8g2.drawStr(3U, 13U + 16U, "Connect to ");
  u8g2.drawStr(3U, 13U + 16U * 2U, "KettleMinderSetup");
  u8g2.drawStr(3u, 13U + 16U * 3U, "If stuck for 3 sec");
  u8g2.sendBuffer();
  for (uint8_t i = 0; i < C_NUM_TBI; i++)
  {
    topBarItem[i].v.trip = 1U;
  }

  WiFiManager wifiManager;
  wifiManager.autoConnect("KettleMinderSetup");

  Serial.println("Connected.");
}

void TBITripTog()
{
  for (uint8_t i = 0; i < C_NUM_TBI; i++)
  {
    if (topBarItem[i].v.trip == 0)
    {
      topBarItem[i].v.tripD = 0;
    }
    else
    {
      topBarItem[i].v.tripD ^= 1U;
    }
  }
}

void drawTopBar()
{
  char str[] = " ";
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_7x14_tf);
  for (uint8_t i = 0; i < C_NUM_NUM_TBI; i++)
  {
    str[0] = i + '1';
    if (topBarItem[i].v.tripD != 0)
    {
      u8g2.setDrawColor(1);
      u8g2.drawBox(i * 14, 0, 13, 16);
      u8g2.setDrawColor(0);
      u8g2.drawStr(3U + i * 14U, 13U, str);
    }
    else
    {
      u8g2.setDrawColor(1);
      u8g2.drawStr(3U + i * 14U, 13U, str);
    }
    if (topBarItem[i].v.dcon != 0)
    {
      u8g2.drawHLine(1 + i*14, 7, 11);
    }
    if (topBarItem[i].v.sel != 0)
    {
      u8g2.drawBox(i*14, 14, 13, 2);
    }
  }
  for (uint8_t i = C_NUM_NUM_TBI; i < C_NUM_TBI; i++)
  {
    switch (i)
    {
      case C_NUM_NUM_TBI:
        str[0] = 'F';
        break;
      case C_NUM_NUM_TBI+1U:
        str[0] = 'C';
        break;
      case C_NUM_NUM_TBI+2U:
        str[0] = 'S';
        break;
    } 
    if (topBarItem[i].v.tripD != 0)
    {
      u8g2.setDrawColor(1);
      u8g2.drawBox(86U + (i-C_NUM_NUM_TBI) * 14U, 0, 13U, 16U);
      u8g2.setDrawColor(0);
      u8g2.drawStr(89U + (i-C_NUM_NUM_TBI) * 14U, 13U, str);
    }
    else
    {
      u8g2.setDrawColor(1);
      u8g2.drawStr(89U + (i-C_NUM_NUM_TBI) * 14, 13, str);
    }
    if (topBarItem[i].v.dcon != 0)
    {
      u8g2.drawHLine(87U + (i-C_NUM_NUM_TBI)*14, 7, 11);
    }
    if (topBarItem[i].v.sel != 0)
    {
      u8g2.drawBox(86U + (i-C_NUM_NUM_TBI)*14, 14, 13, 2);
    }    
  }
}

void loop() {
  EVERY_N_MILLISECONDS(5000)
  {
    for (uint8_t i = 0; i < C_NUM_TBI; i++)
    {
      topBarItem[i].v.dcon ^= 1U;
    }
  }
  EVERY_N_MILLISECONDS(3000)
  {
    for (uint8_t i = 0; i < C_NUM_TBI; i++)
    {
      topBarItem[i].v.sel ^= 1U;
    }
  }
  EVERY_N_MILLISECONDS(1000)
  {
    u8g2.clearBuffer();
    TBITripTog();
    drawTopBar();
    u8g2.sendBuffer();
  }
}
