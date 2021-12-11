#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Wire.h>

// Configuration
const int IMU_SENSOR_ID = 55;
const byte SLAVE_ADDRESS = 0x04;
const unsigned long SERIAL_BAUD = 115200;

// BNO055 Absolute Orientation Sensor
Adafruit_BNO055 bno = Adafruit_BNO055(IMU_SENSOR_ID, 0x28);

// Holds a raw measurement from the IMU.
sensors_event_t data;

// Holds data necessary to output
const int NUM_MEASUREMENTS = 3;
struct Measurement
{
  float data[NUM_MEASUREMENTS];
};
 
// Commands the program will respond to
enum CommandsEnum
{
  ORIENTATION = '!'
};
int NUM_COMMANDS = 1;
char COMMANDS[] = {ORIENTATION};

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("Starting...");

  if (!bno.begin()) {
    // no sensor detected, loop forever
    Serial.println("No Sensor Detected");
    while(1);
  }  
  Serial.println("Sensor started!");
  pinMode(LED_BUILTIN, OUTPUT);

  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(imuRequest);

  Serial.println("Started");
}

char wireData[100];
bool LED_VALUE = false;
void loop()
{
  delay(100);
  digitalWrite(LED_BUILTIN, (LED_VALUE = (LED_VALUE == LOW ? HIGH : LOW)));
  getIMU();
}

// IMU queries the sensor and queues the measurements
void getIMU() {
  // Hold the measurement from the IMU
  Measurement meas;

  // Get imu reading
  bno.getEvent(&data, Adafruit_BNO055::VECTOR_EULER);
  meas.data[0] = data.orientation.roll;
  meas.data[1] = data.orientation.pitch;
  meas.data[2] = data.orientation.heading;

  String imuData = "";
  for(int i = 0; i < NUM_MEASUREMENTS; ++i)
  {
    imuData = imuData + String(meas.data[i], 4);
    if(i < NUM_MEASUREMENTS - 1) imuData = imuData + ",";
  }
  imuData.toCharArray(wireData, imuData.length()+1);
}

void imuRequest() {
  Wire.write(wireData);
  Serial.print("Handled Request:");
  Serial.println(wireData);
}
