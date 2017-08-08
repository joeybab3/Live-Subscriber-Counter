//By Joey Babcock
//Find diagram and more here: http://www.joeybabcock.me/blog/projects/arduino-esp8266-live-subscriber-display/

#include <YoutubeApi.h>
#include <SPI.h>
#include "LedMatrix.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define NUMBER_OF_DEVICES 2
#define CS_PIN D4
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CS_PIN);

char ssid[] = "SSID_HERE"; // your network SSID (name)
char password[] = "PASSWORD"; // your network key
#define API_KEY "find on https://console.developers.google.com/" // your google apps API Token
#define CHANNEL_ID "UC7SHEJBjzSz7Xm9M4d93mOQ" // makes up the url of channel(Find yours here: 
WiFiClientSecure client;
YoutubeApi api(API_KEY, client);
int api_mtbs = 60000; //mean time between api requests
long api_lasttime;   //last time api request has been done
long subs = 0;

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
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
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  ledMatrix.init();
  ledMatrix.setIntensity(10); // range is 0-15
  getStats();
}

void loop() {
  scroll();
  ArduinoOTA.handle();
  if (millis() > api_lasttime + api_mtbs)  {
    if(api.getChannelStatistics(CHANNEL_ID))
    {
      getStats();
    }
    api_lasttime = millis();
  }
}

void scroll()
{
  ledMatrix.clear();
  ledMatrix.scrollTextLeft();
  ledMatrix.drawText();
  ledMatrix.commit();
  delay(100);
}

void getStats() {
  ledMatrix.setText(String(api.channelStats.subscriberCount));
}

