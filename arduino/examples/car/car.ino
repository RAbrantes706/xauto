#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include <NMEAGPS.h>
#include <GPSport.h>

#define USERNAME "Pipas"
#define PASSWORD "1234"

#define VEHICLE_ID 1
#define SENSOR_TESTE 1

const char* ssid = "Sensor";
const char* password = "ExVe2021";

const char* host = "https://abrantes.sytes.net/";
const char* fingerprint =
"99 E6 30 17 01 10 18 0C EF 59 17 FB 1B 5B 1F FD F7 ED DD EA";

WiFiClientSecure client;

static NMEAGPS gps;
static gps_fix fix;

String token;

String value;
bool p;

String logIn(String name, String password){
  HTTPClient http;

  char url[256];
  strcpy(url, host);
  strcat(url, "api/login/");

  DynamicJsonDocument credJson(256);
  credJson["name"] = name;
  credJson["password"] = password;

  String payload = "";
  serializeJson(credJson, payload);

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(payload.length()));

  Serial.println(payload);

  int httpCode = http.POST(payload);
  if(httpCode != HTTP_CODE_OK){
    Serial.println("Could not log in, invalid credentials!");
    return "";
  }

  String response = http.getString();
  StaticJsonDocument<256> resJson;
  DeserializationError err = deserializeJson(resJson, response);

  if(err){
    Serial.print("Deserialization failed with code ");
    Serial.println(err.f_str());
    return "";
  }

  String token = resJson["accessToken"];
  http.end();
  return token;
}

int send(String val){
  digitalWrite(LED_BUILTIN, HIGH);
  HTTPClient http;

  char url[256];
  strcpy(url, host);
  strcat(url, "api/vehicles/");
  strcat(url, String(VEHICLE_ID).c_str());
  strcat(url, "/measurements");

  DynamicJsonDocument dataJson(1024);
  JsonArray sensors = dataJson.createNestedArray("sensors");
  sensors.add(SENSOR_TESTE);

  JsonArray values = dataJson.createNestedArray("values");
  values.add(val);

  String payload = "";
  serializeJson(dataJson, payload);
  Serial.println(payload);

  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " + token);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", String(payload.length()));

  int httpCode = http.POST(payload);
  if(httpCode == 403){
    Serial.println("Session Expired, attempting to log in...");
    token = logIn(USERNAME, PASSWORD);
    return -1;
  }else if(httpCode != 201){
    Serial.print("Did not create reading:");
    Serial.println(" " + String(httpCode));
    Serial.println(http.getString());
    return -1;
  }

  http.end();
  digitalWrite(LED_BUILTIN, LOW);
  return 0;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  gpsPort.begin(115200);
  Serial.begin(115200);

  while(!Serial);
  Serial.println("Starting test!\n");

  Serial.println(); Serial.println();
  Serial.print("Connecting to "); Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  digitalWrite(LED_BUILTIN, HIGH);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setFingerprint(fingerprint);
  //client.setInsecure();
  token = logIn(USERNAME, PASSWORD);

  if(token.equals("")){
    /* If invalid credentials lock the program */
    Serial.println("Error while authenticating, locking program.");
    while(true){
      delay(200);
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  }

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  delay(2000);
  if(p = !p){
    value = "Hello";
  }else{
    value = "World";
  }
  send(value);
}
