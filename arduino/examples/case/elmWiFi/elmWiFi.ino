#include <WiFi.h>
#include <ELMduino.h>
#include <GPSport.h>
#include <NMEAGPS.h>

#define LED_BUILTIN 2

#define GPSSerial Serial2
#define GPSECHO false

typedef union{
  float num;
  byte bin[4];
} binFloat;

const char* ssid = "WIFI_OBDII";
IPAddress server(192, 168, 0, 10);
WiFiClient client;

static NMEAGPS gps;
static gps_fix fix;

binFloat latitude;
binFloat longitude;
binFloat speed;
binFloat heading;
binFloat rpm;

void stop(){
  while(true){
    delay(100);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}

void sendData(){
  digitalWrite(LED_BUILTIN, HIGH); 
  Serial.write(0x7f);
  Serial.write(latitude.bin, 4);
  Serial.write(longitude.bin, 4);
  Serial.write(speed.bin, 4);
  Serial.write(heading.bin, 4);
  Serial.write(rpm.bin, 4);
  Serial.write(0x01);
  digitalWrite(LED_BUILTIN, LOW); 
}

float getRPM(){
  client.write("01 0C\r");
  String res = client.readString();

  char *pos = strstr(res.c_str(), "41 0C");
  if(!pos) return -1.0;

  int sum = 0;
  for(int i=0; i<5; i++){
    if(i==2) continue;
    sum <<= 4;
    sum += (pos[6+i]-'0');
  }
  
  return sum * 0.25;
}


void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  gpsPort.begin(9600);

  delay(1000);

  // FIX FOR ESP32 BOARDS!
  // https://github.com/SlashDevin/NeoGPS/issues/110
  gpsPort.println(F("$PUBX,40,VTG,0,0,0,0*5E")); //VTG OFF
  delay(100);
  gpsPort.println(F("$PUBX,40,GGA,0,0,0,0*5A")); //GGA OFF
  delay(100);
  gpsPort.println(F("$PUBX,40,GSA,0,0,0,0*4E")); //GSA OFF
  delay(100);
  gpsPort.println(F("$PUBX,40,GSV,0,0,0,0*59")); //GSV OFF
  delay(100);
  gpsPort.println(F("$PUBX,40,GLL,0,0,0,0*5C")); //GLL OFF
  delay(100);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, NULL);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  while(!client.connect(server, 35000));
  digitalWrite(LED_BUILTIN, LOW); 

  client.write("AT SP 0\r");
}

void loop(){
  while(gps.available(gpsPort)){
    fix = gps.read();

    if(fix.valid.location){
      latitude.num = fix.latitude();
      longitude.num = fix.longitude();
      speed.num = fix.speed_kph();
      heading.num = fix.heading();
      rpm.num = getRPM();

      sendData();
    }else{
      latitude.num = -9999.9;
      longitude.num = -9999.9;
      speed.num = -1.0;
      heading.num = -1.0;
      rpm.num = getRPM();

      sendData();
    }
  }
}
