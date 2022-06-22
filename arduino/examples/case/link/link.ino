#include "XautoConnection.h"

typedef union{
  float num;
  byte bin[4];
} binFloat;

#define USERNAME "Example"
#define PASSWORD "eXaMpLe"

#define VEHICLE_ID 3

#define SENSOR_LAT_ID      6
#define SENSOR_LON_ID      7
#define SENSOR_SPEED_ID    8
#define SENSOR_HEADING_ID  9
#define SENSOR_RPM_ID     10

const char* ssid = "Sensor";
const char* password = "ExVe2021";

const char* host = "https://abrantes.sytes.net/";
const char* fingerprint =
"99 E6 30 17 01 10 18 0C EF 59 17 FB 1B 5B 1F FD F7 ED DD EA";

XautoConnection* xauto;
int sensorIDs[] = {SENSOR_LAT_ID, SENSOR_LON_ID, SENSOR_SPEED_ID,
  SENSOR_HEADING_ID, SENSOR_RPM_ID};

String values[5];

binFloat latitude;
binFloat longitude;
binFloat speed;
binFloat heading;
binFloat rpm;

int readFloat(binFloat *number){
  int readBytes = 0;
  for(int i=0;i<4;i++){
    int r = Serial.read();
    if(r==-1) return readBytes;
    number->bin[i] = (byte)(r);
    readBytes++;
  }
  return readBytes;
}

bool readData(){
  int r = -1;
  do{ // -1 means no more available, 0x7f is start of message byte
    r = Serial.read(); //flush other characters until EOM or start byte
    if(r==-1) return false;
  }while(r!=127);

  readFloat(&latitude);
  readFloat(&longitude);
  readFloat(&speed);
  readFloat(&heading);
  readFloat(&rpm);

  r = Serial.read();
  if(r!=1) return false;
  return true;
}

bool anyValid(){
  for(int i=0; i<5; i++){
    if(!values[i].equals("")) return true;
  }
  return false;
}

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
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
  digitalWrite(LED_BUILTIN, HIGH); 

  Serial.print("Connected to router at IP: ");
  Serial.println(WiFi.localIP());

  xauto = new XautoConnection(host, fingerprint, USERNAME, PASSWORD, VEHICLE_ID,
    sensorIDs, 5);
}

void loop(){
  while(Serial.available()){
    if(readData()){
      values[0] = (latitude.num<-1000.0)?"":String(latitude.num, 6);
      values[1] = (longitude.num<-1000.0)?"":String(longitude.num, 6);
      values[2] = (speed.num<0.0)?"":String(speed.num, 1);
      values[3] = (heading.num<0.0)?"":String(heading.num, 0);
      values[4] = (rpm.num<0.0)?"":String(rpm.num, 1);

      if(!anyValid()){
        Serial.flush();
        continue;
      }
      
      digitalWrite(LED_BUILTIN, LOW); 
      xauto->send(values);
      digitalWrite(LED_BUILTIN, HIGH); 
    }
  }
  delay(133);
}

