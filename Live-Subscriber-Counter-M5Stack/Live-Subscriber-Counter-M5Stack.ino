//By Joey Babcock
//Find diagram and more here: https://www.joeybabcock.me/blog/projects/arduino-esp8266-live-subscriber-display/

#include <M5Stack.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#include <YoutubeApi.h>
#include <SPI.h>
#include "LedMatrix.h"

#include <WiFiClientSecure.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define API_KEY "find on https://console.developers.google.com/" // your google apps API Token
#define CHANNEL_ID "UC7SHEJBjzSz7Xm9M4d93mOQ" // makes up the url of channel(Find yours here: 
WiFiClientSecure client;
YoutubeApi api(API_KEY, client);
int api_mtbs = 10000; //time between api requests
long api_lasttime;   //last time api request has been done
long subs = 0;

void setup() {

    // initialize the M5Stack object
    m5.begin();
    m5.lcd.setBrightness(100);
    m5.Lcd.setFont();
    m5.Lcd.fillScreen(BLACK);
    m5.Lcd.setCursor(10, 10);
    m5.Lcd.setTextColor(WHITE);
    m5.Lcd.setTextSize(2);
  
    // We start by connecting to a WiFi network
    int count = 0;
    Serial.println("Connecting to wifi...");
    m5.Lcd.println("Connecting to WiFi...");
    wifiMulti.addAP("ssid1", "password");
    wifiMulti.addAP("ssid2", "password");
    wifiMulti.addAP("freewifi", "");
    while(wifiMulti.run() != WL_CONNECTED) {
      if(count < 20)
      {
        delay(500);
        Serial.print(".");
        
        m5.Lcd.printf(".");
      }
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    m5.Lcd.println("Wifi connection successful with IP: ");
    m5.Lcd.println(WiFi.localIP());
   
    m5.Lcd.fillScreen(BLACK);
  ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  getStats();
}

void loop() {
  ArduinoOTA.handle();
  if (millis() > api_lasttime + api_mtbs)  {
    if(api.getChannelStatistics(CHANNEL_ID))
    {
      getStats();
    }
    api_lasttime = millis();
  }
}

void scroll(const char* msg)
{
  m5.Lcd.fillScreen(BLACK);
  m5.Lcd.setTextColor(WHITE);
  m5.Lcd.setCursor(10, 10);
  m5.Lcd.setTextSize(8);
  M5.Lcd.printf(msg);
  Serial.println(msg);
}

void getStats() {
  String stringOne = String(api.channelStats.subscriberCount);
  char charBuf[50];
  stringOne.toCharArray(charBuf, 50);
  scroll(charBuf);
}

