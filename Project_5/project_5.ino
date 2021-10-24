#include <millisDelay.h>

#include <OLEDDisplay.h>
#include <OLEDDisplayFonts.h>
#include <OLEDDisplayUi.h>
#include <SSD1306Wire.h>
#include "DHTesp.h"
#include <stdlib.h>
#include <cstdio>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WIFI Auth
const char WIFI_NAME[] = "{your SSID}";
const char WIFI_PASS[] = "{your password}";

// Display Variable
SSD1306Wire display(0x3c, D3, D4);

// Temperature and Humidity Sensor
DHTesp dht;
const int DHT_UPDATE_PERIOD = 1000;
millisDelay DHTreadDelay;

const int PRINT_BUFF_SIZE = 80;
char FormattedTemperature[PRINT_BUFF_SIZE];
char FormattedHumidity[PRINT_BUFF_SIZE];

const int FRAME_RATE = 60;
const int FRAME_PERIOD = 1000 / FRAME_RATE;
millisDelay OLEDframeDelay;

const char TSPEAK_WRITE_KEY[] = "S687MH9LQ0GGPPPC";
const char TSPEAK_WRITE_URL_FMT[] = "http://api.thingspeak.com/update?api_key=%s&field1=%f&field2=%f";
const int URLBUF_SIZE = sizeof(TSPEAK_WRITE_URL_FMT) + 80;
char urlbuf[URLBUF_SIZE];
const char TSPEAK[] = "https://api.thingspeak.com/  ";
unsigned long TSPEAK_INTERVAL_MS = 20000;
millisDelay thingSpeakDelay;
millisDelay thingSpeakInidicatorDelay;
bool tspeakIsUpdating = false;

const int NUM_FIELDS = 2;
int currentFieldNum = 1;

float temperature = 0.0;
float humidity = 0.0;

void writeTSpeakField(float field1Val, float field2Val) {
  WiFiClient client;
  HTTPClient http;  // Declare an object of class HTTPClient
  snprintf(urlbuf, URLBUF_SIZE, TSPEAK_WRITE_URL_FMT, TSPEAK_WRITE_KEY, field1Val, field2Val);
  http.begin(client, urlbuf);  // Specify request destination
  Serial.print("ThingSpeak request: ");
  Serial.println(urlbuf);  // Print the request payload
  int httpCode = http.GET();  // Send the request
  if (httpCode != HTTP_CODE_OK) { // Check the returning code
    Serial.print("ThingSpeak HTTP Error: ");
    Serial.print(httpCode);
    Serial.print(" ");
    Serial.println(http.errorToString(httpCode));
  }
  else {
  String payload = http.getString();  // Get the request response payload
  Serial.print("Success! New Record ID: ");
  Serial.println(payload); // Print the response payload
  }
http.end();  // Close connection
}

void setup()
{
  // Setup debugging prints
  Serial.begin(115200);

  // Setup Display
  display.init();
  display.clear();
  display.setColor(WHITE);
  display.setFont(ArialMT_Plain_10);
  display.display();

  // Setup DHT11 sensor
  dht.setup(D6, DHTesp::DHT11);

  // Setup Wifi
  Serial.println("Connecting to WiFi.");
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start Timers
  DHTreadDelay.start(DHT_UPDATE_PERIOD);
  OLEDframeDelay.start(FRAME_PERIOD);
  thingSpeakDelay.start(TSPEAK_INTERVAL_MS);
}

void loop() {
  if (DHTreadDelay.justFinished()) {
    update_temp();
  }
  if (OLEDframeDelay.justFinished()) {
    OLEDframeDelay.repeat();
    update_oled();
  }
  if (thingSpeakDelay.justFinished()) {
    thingSpeakDelay.repeat();
    thingSpeakInidicatorDelay.start(1000);
    tspeakIsUpdating = true;
    if (WiFi.status() != WL_CONNECTED) { //Check WiFi connection status
      Serial.println("Wifi Not Connected");
      return;
    }
    writeTSpeakField(temperature, humidity);
  }
  if (thingSpeakInidicatorDelay.justFinished()) {
    tspeakIsUpdating = false;
  }
}

void update_temp() {
  temperature = dht.toFahrenheit(dht.getTemperature());
  humidity = dht.getHumidity();
}

void update_oled() {
  display.clear();
  drawIndoor(&display, temperature, humidity);
  if (tspeakIsUpdating) {
    drawUpdating(&display);
  }
  display.display();
}

void drawIndoor(SSD1306Wire * display, float temperature, float humidity) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 0, "DHT22 Indoor Sensor");
  display->setFont(ArialMT_Plain_16);
  dtostrf(temperature, 4, 1, FormattedTemperature);
  display->drawString(64, 12, "Temp: " + String(FormattedTemperature) + ("°F"));
  dtostrf(humidity, 4, 1, FormattedHumidity);
  display->drawString(64, 30, "Humidity: " + String(FormattedHumidity) + "%");
}

void drawUpdating(SSD1306Wire * display) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 48, "Updating Thingspeak...");
}
