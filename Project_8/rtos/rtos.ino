// Required Libraries
// FreeRTOS
// Adafruit BNO055 -> requires Adafruit Sensors lib

#include <Arduino_FreeRTOS.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <queue.h>
#include <limits.h>

// Configuration
const int IMU_SENSOR_ID = 55;
const int IMU_READ_INTERVAL_MS = 200;
const unsigned long SERIAL_BAUD = 115200;
const UBaseType_t MEASUREMENT_QUEUE_SIZE = 3;
const int SERIAL_WRITE_BUFFER_SIZE = 255;

// BNO055 Absolute Orientation Sensor
Adafruit_BNO055 bno = Adafruit_BNO055(IMU_SENSOR_ID, 0x28);

// Handles reading from the IMU
void TaskIMU( void *pvParameters );
// Handles serial communication
void TaskSerialWrite( void *pvParameters );
// Tasks shall not return or exit, so loop infinitely
void ErrorLoop();
// function to handle accurate waiting and millis() overflow
inline unsigned long CalculateWaitTargetWaitForOverflow(
  const unsigned long readInterval, 
  unsigned long & lastWaitTarget, 
  unsigned long & loopCount, 
  unsigned long & rolloverOffset);

// buffer to hold the printed string
char stringbuf[SERIAL_WRITE_BUFFER_SIZE];

// Holds a raw measurement from the IMU.
sensors_event_t data;

// Holds data necessary to output
const int NUM_MEASUREMENTS = 3;
struct Measurement
{
  float data[NUM_MEASUREMENTS];
};
 
// IMU data
QueueHandle_t dataQ;

// the setup function runs once when you press reset or power the board
void setup() {
  // Note NOT statically allocated.
  dataQ = xQueueCreate(MEASUREMENT_QUEUE_SIZE, sizeof(Measurement));

  Serial.begin(SERIAL_BAUD);

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskIMU,  
    "IMURead",
    128,  // Stack size
    NULL,
    1,  // priority
    NULL);

  xTaskCreate(
    TaskSerialWrite,
    "SerialWrite",
    128, // This stack size can be checked & adjusted by reading Highwater
    NULL,
    2,  // priority
    NULL);

  // Task scheduler is automatically started
}

void loop()
{
  // Empty. Things are done in Tasks.
}

// IMU Task queries the sensor and queues the measurements
void TaskIMU(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  if (!bno.begin())
  {
    // no sensor detected, loop forever
    Serial.println("No Sensor Detected");
    ErrorLoop();
  }

  // Hold the measurement from the IMU
  Measurement meas;

  // number of measurements taken
  unsigned long measurementCount = 0;
  // time of the last wait
  unsigned long lastWaitTarget = 0;
  // when a rollover occurs, this offset is used to 
  // calculate the correct time to wait to
  unsigned long rolloverOffset = 0;

  while(true)
  {
    // Get imu reading
    bno.getEvent(&data, Adafruit_BNO055::VECTOR_EULER);
    meas.data[0] = data.orientation.roll;
    meas.data[1] = data.orientation.pitch;
    meas.data[2] = data.orientation.heading;
  
    // put item on queue, do not block if full
    xQueueSend(dataQ, &meas, 0);

    // Calculate the next time to wait until
    unsigned long waitTarget = CalculateWaitTargetWaitForOverflow(
      IMU_READ_INTERVAL_MS, // Duration of each tick
      lastWaitTarget, // Last time we waited to
      measurementCount, // Number of previous waits
      rolloverOffset);// Offset introduced by rollovers
    // wait until time to take the next measurement. Can't use the
    // standard Free RTOS wait here because the lowest time setable
    // in the arduino version is 15ms and the max IMU frequency is 100Hz.
    while(millis() < waitTarget);
    lastWaitTarget = waitTarget;
    ++measurementCount;
  }
}

// Reads measurements from the queue and write them off
void TaskSerialWrite(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  // local measurement to hold the data from the queue
  Measurement meas;

  while(true)
  {
    // Get the latest measurements from the queue, block until not-empty
    if(xQueueReceive(dataQ, &meas, portMAX_DELAY) == pdTRUE)
    {
      for(int i = 0; i < NUM_MEASUREMENTS; ++i)
      {
        Serial.print(meas.data[i]);
        Serial.print(",");
      }
      Serial.print("\n");
    }
  }
}


// If a task errors out this will hold it in an Error state
void ErrorLoop()
{
  Serial.println("TASK ERROR");
  while(true) 
  { // wait for one second
      vTaskDelay( 1000 / portTICK_PERIOD_MS );
  }
}

// Calculate a time to wait to based on the input period. 
// Waiting via measuring the time taken, or a constant wait induces delays
// on each iteration that add up over time. Calculating targets to wait to
// via a counter and a period removes this drift, but overflow of the counters
// must be handled.
// Note that if an overflow is going to occur, this function will wait until it does.
inline unsigned long CalculateWaitTargetWaitForOverflow(
  const unsigned long readInterval, 
  unsigned long & lastWaitTarget, 
  unsigned long & loopCount, 
  unsigned long & rolloverOffset)
{
    unsigned long waitTarget = loopCount * readInterval + rolloverOffset;
    // rollover will occur at ~50 days of uptime
    if(waitTarget < lastWaitTarget)
    {
      lastWaitTarget = 0;
      // if the imu read interval is not a perfect divisor, some millis will be lost
      rolloverOffset = ULONG_MAX % readInterval;
      // calculate the wait target after the rollover
      waitTarget = loopCount * readInterval - rolloverOffset;
      // wait until the rollover
      while(millis() > 0);
    }
    return waitTarget;
}
