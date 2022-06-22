#include "XautoConnection.h"

#define USERNAME "Example"
#define PASSWORD "eXaMpLe"

#define VEHICLE_ID 1

#define SENSOR_TEST_ID 1

const char* ssid = "Sensor";
const char* password = "ExVe2021";

const char* host = "https://abrantes.sytes.net/";
const char* fingerprint =
"99 E6 30 17 01 10 18 0C EF 59 17 FB 1B 5B 1F FD F7 ED DD EA";

XautoConnection* xauto;
int sensorIDs[] = {SENSOR_TEST_ID};
String values[1];
bool p;

void setup(){
  Serial.begin(9600);
  delay(1000);

  while(!Serial);
  Serial.println("Starting test!\n");

  Serial.println(); Serial.println();
  Serial.print("Connecting to "); Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  Serial.print("Connected to router at IP: ");
  Serial.println(WiFi.localIP());

  xauto = new XautoConnection(host, fingerprint, USERNAME, PASSWORD, VEHICLE_ID,
    sensorIDs, 1);

  p = false;
}

void loop(){
  delay(2000);
  values[0] = (p=!p)?"Hello":"World";
  xauto->send(values);
}
