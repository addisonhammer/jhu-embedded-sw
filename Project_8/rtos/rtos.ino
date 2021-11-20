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
const unsigned long SERIAL_BAUD = 115200;
const UBaseType_t MEASUREMENT_QUEUE_SIZE = 3;
const UBaseType_t COMMAND_QUEUE_SIZE = 3;

// BNO055 Absolute Orientation Sensor
Adafruit_BNO055 bno = Adafruit_BNO055(IMU_SENSOR_ID, 0x28);

// Handles reading from the IMU
void TaskIMU( void *pvParameters );
// Handles serial communication
void TaskSerialWrite( void *pvParameters );
// Tasks shall not return or exit, so loop infinitely
void ErrorLoop();

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
// Commands
QueueHandle_t commandQ;

// Commands the program will respond to
enum CommandsEnum
{
  ORIENTATION = '!'
};
int NUM_COMMANDS = 1;
char COMMANDS[] = {ORIENTATION};

// the setup function runs once when you press reset or power the board
void setup() {
  // Note NOT statically allocated.
  dataQ = xQueueCreate(MEASUREMENT_QUEUE_SIZE, sizeof(Measurement));
  commandQ = xQueueCreate(COMMAND_QUEUE_SIZE, sizeof(char));

  Serial.begin(SERIAL_BAUD);
  pinMode(LED_BUILTIN, OUTPUT);

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

  char command;

  while(true)
  {
    if(xQueueReceive(commandQ, &command, portMAX_DELAY) == pdTRUE)
    {
      if(command == ORIENTATION)
      {
        // Get imu reading
        bno.getEvent(&data, Adafruit_BNO055::VECTOR_EULER);
        meas.data[0] = data.orientation.roll;
        meas.data[1] = data.orientation.pitch;
        meas.data[2] = data.orientation.heading;
      
        // put item on queue, do not block if full
        xQueueSend(dataQ, &meas, 0);
      }
    }
  }
}

// Reads measurements from the queue and write them off
void TaskSerialWrite(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  // local measurement to hold the data from the queue
  Measurement meas;
  
  static const int READ_ERR_VAL = -1;
  while(true)
  {
    int incoming = READ_ERR_VAL;

    // blink the onboard LED to signify waiting for commands
    unsigned long last_blink = 0;
    int LED_VALUE = LOW;
    while((incoming = Serial.read()) == READ_ERR_VAL)
    {
      unsigned long tme = millis();
      if(last_blink + 200 <= tme)
      {
        last_blink = tme;
        digitalWrite(LED_BUILTIN, (LED_VALUE = (LED_VALUE == LOW ? HIGH : LOW)));
      }
    }
    digitalWrite(LED_BUILTIN, HIGH);
    
    // Ignore non orientation commands
    bool validCommand = false;
    for(int i = 0; i < NUM_COMMANDS; ++i)
    {
      if((char)incoming == COMMANDS[i]) 
      { 
        validCommand = true;
        break;
      }
    }

    if(!validCommand) continue;

    // put item on queue, do not block if full
    xQueueSend(commandQ, &incoming, 0);
    
    // Get the latest measurements from the queue, block until not-empty
    if(xQueueReceive(dataQ, &meas, portMAX_DELAY) == pdTRUE)
    {
      for(int i = 0; i < NUM_MEASUREMENTS; ++i)
      {
        Serial.print(meas.data[i]);
        if(i < NUM_MEASUREMENTS - 1) Serial.print(",");
      }
      Serial.print("\n");
    }
    
  }
}


// If a task errors out this will hold it in an Error state
void ErrorLoop()
{
  Serial.println("TASK ERROR");

  digitalWrite(LED_BUILTIN, HIGH);
  
  while(true) 
  { // wait for one second
      vTaskDelay( 1000 / portTICK_PERIOD_MS );
  }
}
