#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <NMEAGPS.h>
#include <GPSport.h>

#define USERNAME "Teste"
#define PASSWORD "3148509"

#define VEHICLE_ID 3

#define SENSOR_LAT_ID     6
#define SENSOR_LON_ID     7
#define SENSOR_SPEED_ID   8
#define SENSOR_HEADING_ID 9
#define SENSOR_ECU_RPM    10

//const char* ssid = "Sensor";
//const char* password = "ExVe2021";

const char* ssid = "Forte";
const char* password = "l0G53r!Run";

const char* host = "https://abrantes.sytes.net:443/";

const char* rootCA =
"-----BEGIN CERTIFICATE-----\r\n"\
"MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw\r\n"\
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\r\n"\
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw\r\n"\
"WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\r\n"\
"RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\r\n"\
"AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP\r\n"\
"R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx\r\n"\
"sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm\r\n"\
"NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg\r\n"\
"Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG\r\n"\
"/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC\r\n"\
"AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB\r\n"\
"Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA\r\n"\
"FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw\r\n"\
"AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw\r\n"\
"Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB\r\n"\
"gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W\r\n"\
"PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl\r\n"\
"ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz\r\n"\
"CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm\r\n"\
"lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4\r\n"\
"avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2\r\n"\
"yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O\r\n"\
"yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids\r\n"\
"hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+\r\n"\
"HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv\r\n"\
"MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX\r\n"\
"nLRbwHOoq7hHwg==\r\n"\
"-----END CERTIFICATE-----\r\n";

WiFiClientSecure client;

static NMEAGPS gps;
static gps_fix fix;

String token;

int sensorIDs[] = {SENSOR_ECU_RPM, SENSOR_LAT_ID, SENSOR_LON_ID,
  SENSOR_SPEED_ID, SENSOR_HEADING_ID};
float* values;

String logIn(String name, String password){
  HTTPClient http;

  char url[256];
  strcpy(url, host);
  strcat(url, "api/login/");

  String payload = "{\"name\":\"" + name + "\",\"password\":\"" + password + \
    "\"}";

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length", ""+payload.length());

  Serial.println();
  Serial.println(payload);

  int httpCode = http.POST(payload);
  if(httpCode != HTTP_CODE_OK){
    delay(1000);
    Serial.print(httpCode);
    Serial.println(": Could not log in, invalid credentials!");
    Serial.println(http.getString());
    delay(1000);
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

int send(float* val, int length){
  digitalWrite(LED_BUILTIN, HIGH);
  HTTPClient http;

  char url[256];
  strcpy(url, host);
  strcat(url, "api/vehicles/");
  strcat(url, String(VEHICLE_ID).c_str());
  strcat(url, "/measurements");

  String payload, sensors, values;
  sensors = values = "[";
  for(int i=0; i<length; i++){
    sensors += String(sensorIDs[i]) + (i<length-1?",":"]");
    values += String(val[i]) + (i<length-1?",":"]");
  }
  payload = "{\"sensors\":" + sensors + ",\"values\":" + values + "}";

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

void lock(int time){
  while(true){
    delay(time);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  delay(1000);

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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setCACert(rootCA);
  token = logIn(USERNAME, PASSWORD);

  if(token.equals("")){
    /* If invalid credentials lock the program */
    Serial.println("Error while authenticating, locking program.");
    lock(200);
  }
  Serial.println("Authentication successful:");
  Serial.println(token);
  Serial.print("Logged in as:");
  Serial.println(USERNAME);

  Serial.println("\nStarting GPS");
  gpsPort.begin(9600);
}

void loop() {
  while(gps.available(gpsPort)){
    fix = gps.read();
    float rpm = 1.0;

    if(fix.valid.location){
      values = (float*)(malloc(5*sizeof(float)));
      values[0] = rpm;
      values[1] = fix.latitude();
      values[2] = fix.longitude();
      values[3] = 1.60934*fix.speed();
      values[4] = fix.heading();
      send(values, 5);
    }else{
      values = (float*)(malloc(sizeof(float)));
      values[0] = rpm;
      send(values, 1);
    }

    free(values);
  }
}

