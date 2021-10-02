#include <Servo.h>
#include <millisDelay.h>
#include <loopTimer.h>  // Debug output for loop execution time
#include <BufferedOutput.h> // Helper for non-blocking debug output
#include <Wire.h>  // Library which contains functions to have I2C Communication

// I2C Interrupt Configuration
#define SLAVE_ADDRESS 0x40 // Define the I2C address to Communicate
#define I2C_LEN 8

// Timers for RR Interrupts
millisDelay escWriteDelay;
millisDelay tachReadDelay;
millisDelay tachPrintDelay;

// ESC Configuration
Servo ESC; // create servo object to control the ESC
static const uint8_t ESC_PIN = 9;
static const int ESC_ARM_DELAY = 8000;
static const int ESC_WRITE_DELAY = 1000;
uint8_t escSpeed = 0;

// Tach Read Configuration
static const uint8_t IR_RECV_PWR_PIN = 2;
static const uint8_t IR_RECV_READ_PIN = A0;
static const int IR_RECV_READ_DELAY = 0;
static const int THRESHOLD = 800;
bool lastDetected = false;
unsigned long microsDetected;
unsigned long bladeMicros;

// Compute RPM Configuration
static const int TACH_PRINT_DELAY = 1000;
static const long NUM_BLADES = 2;
static const unsigned long MICROS_PER_MIN = 60000000;
unsigned long instantRPM;

createBufferedOutput(bufferedOut, 80, DROP_UNTIL_EMPTY);

void setupEsc() {
  bufferedOut.println("Arming ESC, please wait...");
  ESC.attach(ESC_PIN, 1000, 2000);
  ESC.write(0);  // Send the zero signal to the ESC
  delay(ESC_ARM_DELAY);
  bufferedOut.println("Armed!");
}

void setupIR() {
  pinMode(IR_RECV_PWR_PIN, OUTPUT);
  digitalWrite(IR_RECV_PWR_PIN, HIGH);
}

void sendDataISR() {
  char rpmStr[I2C_LEN];
  sprintf(rpmStr, "%i", instantRPM);
  Serial.print("Data Request Interrupt Received: ");
  Serial.println(rpmStr);
  Wire.write(rpmStr); // return data to PI
}

void handleCommandISR(int len) {
  if (len <= 1) {
    return;
  }
  Serial.print("Command Interrupt Received: ");
  long command = Wire.parseInt();
  Serial.println(command);
  handleSpeedCommand(command);
  Wire.read();
}

void setup() {
  Serial.begin(115200);
  bufferedOut.connect(Serial);  // connect buffered stream to Serial

  // Start I2C Slave Connection, for data transmit interrupt
  Wire.begin(SLAVE_ADDRESS); // this will begin I2C Connection with 0x40 address
  Wire.onRequest(sendDataISR); // sendData is funtion called when Pi requests data
  Wire.onReceive(handleCommandISR); // sendData is funtion called when Pi requests data

  setupEsc();
  setupIR();

  // Start Timers
  escWriteDelay.start(ESC_WRITE_DELAY);
  tachReadDelay.start(IR_RECV_READ_DELAY);
  tachPrintDelay.start(TACH_PRINT_DELAY);
}

void loop() {
  bufferedOut.nextByteOut();
  loopTimer.check(bufferedOut);
  checkSpeedCommand();
  checkESDwrite();
  checkTachRead();
  checkTachPrint();
}

void checkESDwrite() {
  if (escWriteDelay.justFinished()) {
    escWriteDelay.repeat();
    ESC.write(escSpeed); // Send the signal to the ESC
    // Serial.println("ESC Write.");
  }
}

void checkSpeedCommand() {
  if (Serial.available() > 0) {
    long input = Serial.parseInt();
    handleSpeedCommand(input);
  }
}

void handleSpeedCommand(long input) {
  if ((input >= 1300) && (input <= 4000)) {
    escSpeed = (uint8_t)map(input, 1340, 4786, 15, 100);
    bufferedOut.print("Setting Speed to: ");
    bufferedOut.println(escSpeed);
  }
  else if (input == -1) {
    escSpeed = 0;
    bufferedOut.print("Stopping Motors...");
  }
}

void checkTachRead() {
  if (tachReadDelay.justFinished()) {
    tachReadDelay.repeat();
    int val = analogRead(IR_RECV_READ_PIN);
    bool detected = (val < THRESHOLD);
    if (!lastDetected and detected) {
      bladeMicros = micros() - microsDetected;
      microsDetected = micros();
    }
    lastDetected = detected;
  }
}

void checkTachPrint() {
  if (tachPrintDelay.justFinished()) {
    tachPrintDelay.repeat();
    // Serial.print("uS per Blade:");
    // Serial.println(bladeMicros);
    noInterrupts();
    instantRPM = (MICROS_PER_MIN / (bladeMicros * NUM_BLADES));
    interrupts();
    if (instantRPM > 5000) {
      return;
    }
    bufferedOut.print("Current RPM: ");
    bufferedOut.println(instantRPM);
  }
}
