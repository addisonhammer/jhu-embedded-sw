// Required Libraries
// FreeRTOS
// Adafruit BNO055

#include <Arduino_FreeRTOS.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <queue.h>

// Configuration
const int IMU_SENSOR_ID = 55;
const int IMU_READ_INTERVAL_MS = 15;
const unsigned long SERIAL_BAUD = 9600;
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

// buffer to hold the printed string
char stringbuf[SERIAL_WRITE_BUFFER_SIZE];

struct Measurement
{
  float x;
  float y;
  float z;
};
 
// IMU data
QueueHandle_t dataQ;

// the setup function runs once when you press reset or power the board
void setup() {
  dataQ = xQueueCreate(MEASUREMENT_QUEUE_SIZE, sizeof(Measurement));

  Serial.begin(SERIAL_BAUD);

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskIMU,  
    "IMURead",   // A name just for humans
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

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
sensors_event_t data;

void TaskIMU(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  if (!bno.begin())
  {
    // no sensor detected, loop forever
    Serial.println("No Sensor Detected");
    ErrorLoop();
  }

  Measurement meas;

  while(true)
  {
    // Get imu reading
    bno.getEvent(&data, Adafruit_BNO055::VECTOR_EULER);
    meas.x = data.orientation.x;
    meas.y = data.orientation.y;
    meas.z = data.orientation.z;
  
    // put item on queue, do not block if full
    xQueueSend(dataQ, &meas, 0);
    
    // wait until next measurement
    vTaskDelay(IMU_READ_INTERVAL_MS/portTICK_PERIOD_MS);
  }
}

void TaskSerialWrite(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize serial communication at 9600 bits per second:
  Serial.begin(SERIAL_BAUD);

  Measurement meas;
  
  // Get the latest measurement from the queue, block until not-empty
  while(xQueueReceive(dataQ, &meas, portMAX_DELAY ))
  {
    Serial.print(meas.x);
    Serial.print(",");
    Serial.print(meas.y);
    Serial.print(",");
    Serial.print(meas.z);
    Serial.print("\n");
  }
}

void ErrorLoop()
{
  Serial.println("TASK ERROR");
  while(true) 
  { // wait for one second
      vTaskDelay( 1000 / portTICK_PERIOD_MS );
  }
}
