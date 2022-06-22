#include "XautoConnection.h"

XautoConnection::XautoConnection(String XautoHost, const char* rootCA,
    String uname, String pass, int vehicleId, int sensorIDArray[],
    int numSensors){

  host = String(XautoHost);

#ifdef BOARD_PROC_ESP32
  client.setCACert(rootCA);
#endif

#ifdef BOARD_PROC_ESP8266
  client.setFingerprint(rootCA);
#endif

  username = String(uname);
  password = String(pass);
  vehicle = vehicleId;

  accessToken = "";
  while(!accessToken.length()){
    accessToken = logIn();
    delay(1000);
  }

  sensorIDs = sensorIDArray;
  sensorNum = numSensors;
}

int XautoConnection::send(String values[]){
  HTTPClient http;

  char url[256];
  strcpy(url, host.c_str());
  strcat(url, "api/vehicles/");
  strcat(url, String(vehicle).c_str());
  strcat(url, "/measurements");

  String payload, sensors, vals;
  sensors = vals = "[";
  for(int i=0; i<sensorNum; i++){
    if(values[i].length()){
      sensors += String(sensorIDs[i]) + (i<sensorNum-1?",":"]");
      vals    += "\"" + String(values[i]) + "\"" + (i<sensorNum-1?",":"]");
    }
  }
  payload = "{\"sensors\":" + sensors + ",\"values\":" + vals + "}";

  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " + accessToken);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(payload.length()));

  int httpCode = http.POST(payload);
  if(httpCode == 403){
    Serial.println("Session Expired, attempting to log in...");
    accessToken = logIn();
    return -1;
  }else if(httpCode != 201){
    Serial.print("Did not create reading:");
    Serial.println(" " + String(httpCode));
    Serial.println(http.getString());
    return -1;
  }

  http.end();
  return 0;
}

String XautoConnection::logIn(){
  HTTPClient http;

  char url[256];
  strcpy(url, host.c_str());
  strcat(url, "api/login/");

  String payload = "{\"name\":\"" + username + "\",\"password\":\"" + \
    password + "\"}\n";

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(payload.length()));

  int httpCode = http.POST(payload);

  if(httpCode != HTTP_CODE_OK){
    Serial.println("Could not log in, invalid credentials!");
    return "";
  }

  String response = http.getString();
  String token = response.substring(16,response.length()-2);

  if(response.length()<15){
    Serial.println("The server did not return a JWT");
    return "";
  }

  http.end();
  return token;
}
