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
const char WIFI_NAME[] = "SeriesOfTubesGuest";
const char WIFI_PASS[] = "19386592";

// Display Variable
SSD1306Wire display(0x3c, D3, D4);
// Temperature and Humidity Sensor
DHTesp dht;

const int PRINT_BUFF_SIZE = 80;
char printbuff[PRINT_BUFF_SIZE];

const int FRAME_RATE = 60;
const int FRAME_PERIOD = 1000 / FRAME_RATE;

const char TSPEAK_WRITE_KEY[] = "S687MH9LQ0GGPPPC";
const char TSPEAK_WRITE_URL_FMT[] = "http://api.thingspeak.com/update?api_key=%s&field1=%f&field2=%f";
const int URLBUF_SIZE = sizeof(TSPEAK_WRITE_URL_FMT) + 80;
char urlbuf[URLBUF_SIZE];
const char TSPEAK[] = "https://api.thingspeak.com/  ";
unsigned long TSPEAK_INTERVAL_MS = 20000;

unsigned long timeOfLastUpdate = 0;

const int NUM_FIELDS = 2;
int currentFieldNum = 1;


void writeTSpeakField(int field1Val, float field2Val)
{
    if (WiFi.status() == WL_CONNECTED) 
    { //Check WiFi connection status

      WiFiClient client;
      HTTPClient http;  //Declare an object of class HTTPClient
      
      snprintf(urlbuf, URLBUF_SIZE, TSPEAK_WRITE_URL_FMT, TSPEAK_WRITE_KEY, field1Val, field2Val);

      http.begin(client, urlbuf);  //Specify request destination
      int httpCode = http.GET();                                  //Send the request
   
      if (httpCode > 0) { //Check the returning code
   
        String payload = http.getString();   //Get the request response payload
        Serial.println(payload);             //Print the response payload
   
      }
      
      http.end();   //Close connection
  }
  else
  {
    Serial.println("Wifi Not Connected");
  }
}


void tspeakUpdate(float tempC, float humidP)
{
  writeTSpeakField(1, tempC);
  writeTSpeakField(2, humidP);
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
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  display.clear();
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();


  snprintf(printbuff, PRINT_BUFF_SIZE, "Temp C: %f\n Humidity %: %f", temperature, humidity);
  display.drawString(0, 0, printbuff);

  unsigned long currentTime = millis();
  if(currentTime + TSPEAK_INTERVAL_MS > timeOfLastUpdate){
    timeOfLastUpdate = currentTime;
    Serial.println("Updating Server...");
    tspeakUpdate(temperature, humidity);
  }
  
  display.display();

  delay(FRAME_PERIOD);

}