#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

const char* ssid     = "CSW-Guest";
const char* password = "Critical98";
const char* serverName = "http://10.8.8.25:8888/";

HTTPClient http;
WiFiClient client;

#define DHTPIN D2
#define DHTTYPE DHT11
#define soundPin A0

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  sensor_t sensor;
  dht.begin();
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void loop() {
  delay(delayMS);

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    Serial.print("\n\n\n");

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    Serial.println("POST REQUEST");
    String httpRequestData = "{\n\"measurement\": \"temp\",\n";
    httpRequestData += "\"equip\": \"wifi\",\n";
    httpRequestData += "\"value\": ";
    httpRequestData += event.temperature;
    httpRequestData += "\n";
    httpRequestData += "}";

    Serial.println(httpRequestData);

    int httpCode = http.POST(httpRequestData);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
  }

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    Serial.print("\n\n\n");

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    Serial.println("POST REQUEST");
    String httpRequestData = "{\n\"measurement\": \"hum\",\n";
    httpRequestData += "\"equip\": \"wifi\",\n";
    httpRequestData += "\"value\": ";
    httpRequestData += event.relative_humidity;
    httpRequestData += "\n";
    httpRequestData += "}";

    Serial.println(httpRequestData);

    int httpCode = http.POST(httpRequestData);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
  }

  long sound_level = 0;
  long ruido_db = 0;

  sound_level = analogRead(soundPin);
  ruido_db= 20*log(sound_level)-60;
  Serial.println(ruido_db);

  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/json");

  Serial.println("POST REQUEST");
  String httpRequestData = "{\n\"measurement\": \"noise\",\n";
  httpRequestData += "\"equip\": \"wifi\",\n";
  httpRequestData += "\"value\": ";
  httpRequestData += ruido_db;
  httpRequestData += "\n";
  httpRequestData += "}";

  Serial.println(httpRequestData);

  int httpCode = http.POST(httpRequestData);
  String payload = http.getString();
  Serial.println(httpCode);
  Serial.println(payload);
  http.end();


}
