#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <ArduinoJson.h>
#include <NMEAGPS.h>
#include <GPSport.h>

#define USERNAME "Pipas"
#define PASSWORD "1234"

#define VEHICLE_ID 2

#define SENSOR_LAT_ID        6
#define SENSOR_LON_ID        7
#define SENSOR_SPEED_ID      8
#define SENSOR_HEADING_ID    9
#define SENSOR_FRONT_DIST_ID 10
#define SENSOR_SIDE_DIST_ID  11
//#define SENSOR_ECU_SPEED_ID  12

#define PIN_FRONT_SIGNAL  12
#define PIN_FRONT_CONTROL 13

#define PIN_SIDE_ECHO 26
#define PIN_SIDE_TRIG 27

//const char* ssid = "Sensor";
//const char* password = "ExVe2021";

const char* ssid = "Forte";
const char* password = "l0G53r!Run";

const char* host = "https://rabranteshome.dynip.sapo.pt/";

const char* rootCA = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

const char* clientCert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFPTCCBCWgAwIBAgISBH6JFfju67D3aGgy3Lg7nXivMA0GCSqGSIb3DQEBCwUA\n" \
"MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n" \
"EwJSMzAeFw0yMTExMTIxMzE5MTZaFw0yMjAyMTAxMzE5MTVaMCYxJDAiBgNVBAMT\n" \
"G3JhYnJhbnRlc2hvbWUuZHluaXAuc2Fwby5wdDCCASIwDQYJKoZIhvcNAQEBBQAD\n" \
"ggEPADCCAQoCggEBAOnxzKE8vDppUEDkB8YqTlsVA55ihSaCLZ//lYcvycU/btVA\n" \
"k5MMn+KI5ZfJuOLf9JZ6oUh5OhFjNRaHgOalMxw2s3uOnDOGIKvpGCgX9xH3dH2i\n" \
"4Wikq/3ZRXveEORV2as2O3NlTmA1zh61iqMyKGFf/by7bURQPf+SzG5rTg+CHdMi\n" \
"erzabnIEJeDRikVK13wpmxMjcU4bAyBSWdZGgqRCx3OcrYojbo1qAfYh2MJepigl\n" \
"43E6x5eBVUNWzPQIm87Kc7C3sK6pjWdTq39++7TrYT0DwqYJsHeR/BSRzemEITG0\n" \
"lfBAqNJxbr1Fi5tJXa3yvOB6Fii97CAVnpQDrv8CAwEAAaOCAlcwggJTMA4GA1Ud\n" \
"DwEB/wQEAwIFoDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0T\n" \
"AQH/BAIwADAdBgNVHQ4EFgQU1mXj7sKfGQ/1Cdykd6Ti3cJiebowHwYDVR0jBBgw\n" \
"FoAUFC6zF7dYVsuuUAlA5h+vnYsUwsYwVQYIKwYBBQUHAQEESTBHMCEGCCsGAQUF\n" \
"BzABhhVodHRwOi8vcjMuby5sZW5jci5vcmcwIgYIKwYBBQUHMAKGFmh0dHA6Ly9y\n" \
"My5pLmxlbmNyLm9yZy8wJgYDVR0RBB8wHYIbcmFicmFudGVzaG9tZS5keW5pcC5z\n" \
"YXBvLnB0MEwGA1UdIARFMEMwCAYGZ4EMAQIBMDcGCysGAQQBgt8TAQEBMCgwJgYI\n" \
"KwYBBQUHAgEWGmh0dHA6Ly9jcHMubGV0c2VuY3J5cHQub3JnMIIBBQYKKwYBBAHW\n" \
"eQIEAgSB9gSB8wDxAHYA36Veq2iCTx9sre64X04+WurNohKkal6OOxLAIERcKnMA\n" \
"AAF9FIRVWAAABAMARzBFAiBtyPzmv8erHqOTir3WVJ5778YUSkvw8h4FAfarVpb3\n" \
"3QIhAJsHwfDjSovPTg7dYY+9j9DYLaFTO7FiHeNNkF0jIL90AHcAKXm+8J45OSHw\n" \
"VnOfY6V35b5XfZxgCvj5TV0mXCVdx4QAAAF9FIRVSgAABAMASDBGAiEAroznpVW2\n" \
"viVFpjw7J2Ujv+NG4t8WfuPtUSCaQoSlNgcCIQDublrCK3mTr9/BjpeovEzcky7t\n" \
"sTJPdjRBIR7iA4/o2TANBgkqhkiG9w0BAQsFAAOCAQEAQd10fFK1ZuGwffofa35f\n" \
"M4444J7OcDZjCQF+utwu1BW7iFPZ63Hq3g8O9ao+KB5bMSjWgzDGfm9Avs+B8VkG\n" \
"VfkyAxTqIsCIBdO+lfhiCQKoay3WhU/KbswhoLA4gUoY6opqrAqxHnCY714vdTyE\n" \
"+ArjWOl23gekVRNlv8FqzJ8ah0rIv9+yVI6i3pAjQcFoKr8qHF1dONwvl4xUj3O2\n" \
"c3YNuYQdMWV0hjGzoy/ZpBtOl0//yJ5OrrfidAxUu+ipVaeE+zlO9am7ERa+iYyW\n" \
"DDM1lARF69HkvTKLp7KgiiFQ1J9vSZwnNz3yJuhhPNiwWPeZrVEt6IzwU7QrPe7N\n" \
"Mg==\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure client;

static NMEAGPS gps;
static gps_fix fix;

String token;

float *distances;

void measure(float *dist){
  digitalWrite(PIN_FRONT_CONTROL, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_FRONT_CONTROL, LOW);

  dist[0] = pulseIn(PIN_FRONT_SIGNAL, HIGH) * 1.72789e-3;

  delay(100);

  digitalWrite(PIN_SIDE_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_SIDE_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_SIDE_TRIG, LOW);

  dist[1] = pulseIn(PIN_SIDE_ECHO, HIGH) / 2.0 / 2910;
}

String logIn(String name, String password){
  HTTPClient http;

  char url[256];
  strcpy(url, host);
  strcat(url, "api/login/");

  StaticJsonDocument<256> credJson;
  credJson["name"] = name;
  credJson["password"] = password;

  String payload = "";
  serializeJson(credJson, payload);

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", ""+payload.length());

  Serial.println();
  Serial.println(payload);

  int httpCode = http.POST(payload);
  if(httpCode != HTTP_CODE_OK){
    Serial.print(httpCode);
    Serial.println(": Could not log in, invalid credentials!");
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

int send(float* dist){
  digitalWrite(LED_BUILTIN, HIGH);
  HTTPClient http;

  char url[256];
  strcpy(url, host);
  strcat(url, "api/vehicles/");
  strcat(url, String(VEHICLE_ID).c_str());
  strcat(url, "/measurements");

  StaticJsonDocument<1024> dataJson;
  JsonArray sensors = dataJson.createNestedArray("sensors");
  sensors.add(SENSOR_FRONT_DIST_ID);
  sensors.add(SENSOR_SIDE_DIST_ID);

  JsonArray values = dataJson.createNestedArray("values");
  values.add(dist[0]);
  values.add(dist[1]);

  String payload = "";
  serializeJson(dataJson, payload);

  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " + token);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", ""+payload.length());

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

int send(float latitude, float longitude, float speed, float heading, 
    float* dist){

  digitalWrite(LED_BUILTIN, HIGH);
  HTTPClient http;

  char url[256];
  strcpy(url, host);
  strcat(url, "api/vehicles/");
  strcat(url, String(VEHICLE_ID).c_str());
  strcat(url, "/measurements");

  StaticJsonDocument<1024> dataJson;
  JsonArray sensors = dataJson.createNestedArray("sensors");
  sensors.add(SENSOR_LAT_ID);
  sensors.add(SENSOR_LON_ID);
  sensors.add(SENSOR_SPEED_ID);
  sensors.add(SENSOR_HEADING_ID);
  sensors.add(SENSOR_FRONT_DIST_ID);
  sensors.add(SENSOR_SIDE_DIST_ID);

  JsonArray values = dataJson.createNestedArray("values");
  values.add(latitude);
  values.add(longitude);
  values.add(speed);
  values.add(heading);
  values.add(dist[0]);
  values.add(dist[1]);

  String payload = "";
  serializeJson(dataJson, payload);

  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " + token);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", ""+payload.length());

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

  pinMode(PIN_FRONT_CONTROL, OUTPUT);
  pinMode(PIN_FRONT_SIGNAL,  INPUT);

  pinMode(PIN_SIDE_TRIG, OUTPUT);
  pinMode(PIN_SIDE_ECHO, INPUT);

  //gpsPort.begin(9600);
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
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setCACert(rootCA);
  client.setCertificate(clientCert);

  token = logIn(USERNAME, PASSWORD);

  if(token.equals("")){
    /* If invalid credentials lock the program */
    Serial.println("Error while authenticating, locking program.");
    while(true){
      delay(200);
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  }
  Serial.println("Authentication successful.");
  Serial.println(token);

  //Init sonars
  digitalWrite(PIN_FRONT_CONTROL, LOW);
  delay(250);
  digitalWrite(PIN_FRONT_CONTROL, HIGH);
  delay(100);
  digitalWrite(PIN_FRONT_CONTROL, LOW);

  digitalWrite(PIN_SIDE_TRIG, LOW);

  digitalWrite(LED_BUILTIN, LOW);

  distances = (float *)(calloc(2, sizeof(float)));
  Serial.println("Initialized sensors, allocated space for variables");
}

void loop() {
  while(gps.available(gpsPort)){
    fix = gps.read();
    measure(distances);

    if(fix.valid.location){
      send(fix.latitude(), fix.longitude(), 1.60934*fix.speed(), 
          fix.heading(), distances);
    }else{
      send(distances);
    }
  }
}
