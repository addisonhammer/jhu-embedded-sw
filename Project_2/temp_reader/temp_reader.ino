#include <millisDelay.h> // Helpers for Timer-Based RR Loop
#include <BufferedOutput.h> // Helper for non-blocking debug output
#include <loopTimer.h>  // Debug output for loop execution time
#include <DHT.h>  // Library with DHT Sensor Helpers
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

createBufferedOutput(bufferedOut, 80, DROP_UNTIL_EMPTY);

void setup() {
  // Start Serial connection for debugging
  Serial.begin(115200);
  bufferedOut.connect(Serial);  // connect buffered stream to Serial
  // Start I2C Slave Connection, for data transmit interrupt
  Wire.begin(SLAVE_ADDRESS); // this will begin I2C Connection with 0x40 address
  Wire.onRequest(sendDataInterrupt); // sendData is funtion called when Pi requests data

  // Initialize DHT11 device.
  dht.begin();

  // Use the onboard LED to indicate an interrupt has been received
  pinMode(LEDPIN, OUTPUT);

  // Start Timers
  ledDelay.start(ledDelayMS);
  readDelay.start(readDelayMS);
}

void loop() {
  bufferedOut.nextByteOut(); // call at least once per loop to release chars
  loopTimer.check(bufferedOut);
  // Get temperature event and convert it
  readTemp();
}

void readTemp() {
  // Read DHT Temp when sensor is ready (1Hz for DHT11)
  if (readDelay.justFinished()) {
    readDelay.repeat();
    bufferedOut.print("Reading Probe Temp: ");
    float tempF = dht.readTemperature(true);
    bufferedOut.println(tempF);
    // Buffer the temperature as a String for I2C comms
    dtostrf(tempF, 3, 2, tempBuffer);
  }
}

void sendDataInterrupt() {
  // Turn on the LED while running, and print execution time
  unsigned long start = micros();
  digitalWrite(LEDPIN, HIGH);
  bufferedOut.print("Data Request Interrupt Received: ");
  bufferedOut.println(tempBuffer);
  Wire.write(tempBuffer); // return data to PI
  digitalWrite(LEDPIN, LOW);
  bufferedOut.print("  Interrupt Duration (uS): ");
  bufferedOut.println(micros() - start);
}
