#define BOARD_PROC_ESP8266

#ifndef XautoConnection_h
#define XautoConnection_h

#ifdef BOARD_PROC_ESP32
#define _BOARDDEFINED_

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#endif

#ifdef BOARD_PROC_ESP8266
#define _BOARDDEFINED_

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#endif 

#ifndef _BOARDDEFINED_ 
#error "Board processor must be defined: BOARD_PROC_ESP32 or BOARD_PROC_ESP8266"
#endif

class XautoConnection{
  public:
    XautoConnection(String XautoHost, const char* rootCA, String uname,
        String pass, int vehicleId, int sensorIDArray[], int numSensors);
    int send(String* values);
  private:
    String host;
    WiFiClientSecure client;

    String username;
    String password;
    String accessToken;

    int vehicle;
    int sensorNum;
    int* sensorIDs;

    String logIn();
};

#endif
