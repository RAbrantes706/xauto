#include <BluetoothSerial.h>
#include <ELMduino.h>
#include <GPSport.h>
#include <NMEAGPS.h>

#define LED_BUILTIN 13

typedef union{
  float num;
  byte bin[4];
} binFloat;

BluetoothSerial SerialBT;
ELM327 elm;

static NMEAGPS gps;
static gps_fix fix;

binFloat latitude;
binFloat longitude;
binFloat speed;
binFloat heading;
binFloat rpm;

void stop(){
  while(true){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(500);
  }
}

void sendData(){
  Serial2.write(latitude.bin, 4);
  Serial2.write(longitude.bin, 4);
  Serial2.write(speed.bin, 4);
  Serial2.write(heading.bin, 4);
  Serial2.write(rpm.bin, 4);
}

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  Serial2.begin(115200);
  gpsPort.begin(9600);
  delay(1000);

  SerialBT.setPin("1234");
  if (!SerialBT.connect("OBDII")){
    Serial.println("Could not connect to OBDII");
    stop();
  }

  if (!elm.begin(SerialBT, true, 2000)){
    Serial.println("Could not start ELM327");
    stop();
  }

  Serial.println("Successfully connected to ELM327!");

  latitude.num = 1.0;
  longitude.num = 2.0;
  speed.num = 3.0;
  heading.num = 4.0;
  rpm.num = 5.0;
}

void loop(){
  while(gps.available(gpsPort)){
    fix = gps.read();

    if(fix.valid.location){
      float rpmVal = elm.rpm();

      latitude.num = fix.latitude();
      longitude.num = fix.longitude();
      speed.num = fix.speed();
      heading.num = fix.heading();
      rpm.num = (elm.nb_rx_state == ELM_SUCCESS)?rpmVal:-1.0;

      if(elm.nb_rx_state != ELM_SUCCESS){
        elm.printError();
      }

      sendData();
    }

  }
}

