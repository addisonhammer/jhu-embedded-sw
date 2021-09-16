#include <millisDelay.h>
#include <loopTimer.h>
#include <DHT.h>
#include <Wire.h>  // Library which contains functions to have I2C Communication
#define SLAVE_ADDRESS 0x40 // Define the I2C address to Communicate to Uno

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define LEDPIN 13     // Onboard Indicator LED
#define DHTTYPE DHT11

DHT dht = DHT(DHTPIN, DHTTYPE);
char tempBuffer[8];

const uint32_t readDelayMS = 1000;
const uint32_t ledDelayMS = 100;
millisDelay ledDelay;
millisDelay readDelay;

void setup() {
  // Start Serial connection for debugging
  Serial.begin(9600);
  // Start I2C Slave Connection, for data transmit interrupt
  Wire.begin(SLAVE_ADDRESS); // this will begin I2C Connection with 0x40 address
  Wire.onRequest(sendData); // sendData is funtion called when Pi requests data

  // Initialize DHT11 device.
  dht.begin();

  // Use the onboard LED to indicate an interrupt has been received
  pinMode(LEDPIN, OUTPUT);

  // Start Timers
  ledDelay.start(ledDelayMS);
  readDelay.start(readDelayMS);
}

void loop() {
  loopTimer.check(Serial);
  // Get temperature event and convert it
  readTemp();
  resetLED();
}

void readTemp() {
  if (readDelay.justFinished()) {
    readDelay.repeat();
    Serial.print("Reading Probe Temp: ");
    float tempF = dht.readTemperature(true);
    Serial.println(tempF);
    dtostrf(tempF, 3, 2, tempBuffer);
  }
}

void resetLED() {
  if (ledDelay.justFinished()) {
    ledDelay.repeat();
    digitalWrite(LEDPIN, LOW);
  }
}

void sendData() {
  digitalWrite(LEDPIN, HIGH);
  Serial.print("Data Request Received: ");
  Serial.println(tempBuffer);
  Wire.write(tempBuffer); // return data to PI
}
