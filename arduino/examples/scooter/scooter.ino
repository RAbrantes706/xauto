#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <NMEAGPS.h>
#include <GPSport.h>

#define PIN_FRONT 5
#define PIN_SIDE  4
#define PIN_FRONT_CONTROL 14
#define PIN_SIDE_CONTROL  12

const char* ssid = "Sensor";
const char* password = "ExVe2021";

const char* host = 
  "http://rabranteshome.dynip.sapo.pt/api/vehicles/3/measurements";

WiFiClient client;

static NMEAGPS gps;
static gps_fix fix;

//Take a measurement whith the ultrasonic device
float* measure(){
  float *dist = (float *)(calloc(2, sizeof(float)));

  digitalWrite(PIN_FRONT_CONTROL, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_FRONT_CONTROL, LOW);

  dist[0] = pulseIn(PIN_FRONT, HIGH) * 25.4 / 147.0 / 1000;

  delay(100);
  digitalWrite(PIN_SIDE_CONTROL, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_SIDE_CONTROL, LOW);

  dist[1] = pulseIn(PIN_SIDE, HIGH) * 25.4 / 147.0 / 1000;

  for(int i=0;i<2;i++){
    dist[i] = (dist[i]>6.0)?0.0:dist[i];
  }

  return dist;
}

//Send data
void send(float front, float side, float lat, float lon, float speed, 
    float heading){
  if(!client.status() != WL_CONNECTED){
    digitalWrite(LED_BUILTIN, HIGH);
    HTTPClient http;

    String payload = "{\"sensors\":[3,4,5,6,7,8],\"values\":";
    payload += "[" + String(front) + "," + String(side) + ",";
    payload += String(lat) + "," + String(lon) + ",";
    payload += String(speed) + "," + String(heading) + "]}";

    http.begin(client, host);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Content-Length", ""+payload.length());

    int httpCode = http.POST(payload);

    http.end();
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void send(float front, float side){
  if(!client.status() != WL_CONNECTED){
    digitalWrite(LED_BUILTIN, HIGH);
    HTTPClient http;

    String payload = "{\"sensors\":[3,4],\"values\":";
    payload += "[" + String(front) + "," + String(side) + "]}";

    http.begin(client, host);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Content-Length", ""+payload.length());

    int httpCode = http.POST(payload);

    http.end();
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_FRONT_CONTROL, OUTPUT);
  pinMode(PIN_SIDE_CONTROL, OUTPUT);
  pinMode(PIN_FRONT, INPUT);
  pinMode(PIN_SIDE, INPUT);

  gpsPort.begin(9600);
  Serial.begin(9600);

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
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  Serial.print("Connected to router at IP ");
  Serial.println(WiFi.localIP());

  //Init sonars
  digitalWrite(PIN_FRONT_CONTROL, LOW);
  delay(250);
  digitalWrite(PIN_FRONT_CONTROL, HIGH);
  delay(100);
  digitalWrite(PIN_FRONT_CONTROL, LOW);

  delay(500);

  digitalWrite(PIN_SIDE_CONTROL, LOW);
  delay(250);
  digitalWrite(PIN_SIDE_CONTROL, HIGH);
  delay(100);
  digitalWrite(PIN_SIDE_CONTROL, LOW);

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  while(gps.available(gpsPort)){
    fix = gps.read();

    float *dist = measure();
    if(fix.valid.location){
      send(dist[0], dist[1], fix.latitude(), fix.longitude(), fix.speed(),
          fix.heading());
    }else{
      send(dist[0], dist[1]);
    }

    free(dist);
  }
}

