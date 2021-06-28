/*******************************************************************************
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTY AND SUPPORT
 * IS APPLICABLE TO THIS SOFTWARE IN ANY FORM. CYTRON TECHNOLOGIES SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR CONSEQUENTIAL
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 ********************************************************************************
 * DESCRIPTION:
 *
 * This example shows how to drive a motor using the PWM and DIR pins.
 * This example only shows how to drive a single motor for simplicity.
 * For dual channel motor driver, both channel work the same way.
 * 
 * 
 * CONNECTIONS:
 * 
 * Arduino D3  - Motor Driver PWM Input
 * Arduino D4  - Motor Driver DIR Input
 * Arduino GND - Motor Driver GND
 *
 *
 * AUTHOR   : Kong Wai Weng
 * COMPANY  : Cytron Technologies Sdn Bhd
 * WEBSITE  : www.cytron.io
 * EMAIL    : support@cytron.io
 *
 *******************************************************************************/

 #include "CytronMotorDriver.h"

/* ===== PWM PINS ===== */

// Going to the Cytron Motor Driver.
const int PWM_OUTPUT_PIN_1 = 2;
const int PWM_OUTPUT_PIN_2 = 5;

const int DIR_OUTPUT_PIN_1 = 3;
const int DIR_OUTPUT_PIN_2 = 6;

// Inputs are coming from the reciever (fly sky something). 
const int PWM_INPUT_PIN_1 = 8;
const int PWM_INPUT_PIN_2 = 11;

// ==================== */

bool teleopState = false;

// Configure the motor driver.
// CytronMD motor(PWM_DIR, 12, 13);  // PWM = Pin 3, DIR = Pin 4.
CytronMD motor1(PWM_DIR, PWM_OUTPUT_PIN_1, DIR_OUTPUT_PIN_1);
CytronMD motor2(PWM_DIR, PWM_OUTPUT_PIN_2, DIR_OUTPUT_PIN_2);


enum ROBOT_STATE {
  // Start of the loop nothing happens
  // PARENT STATE: NONE | CHILD STATES: READ_TELEOP, READ_SERIAL
  INITIAL_STATE = 0,
  // Read from fly sky reciever and parsing commands
  // PARENT STATE: INITIAL_STATE | CHILD STATES: RUN_COMMAND
  READ_TELEOP = 1,
  // Reads and parses serial commands from the raspberry pi i.e. also check vision
  // PARENT STATE: INITIAL_STATE | CHILD STATES: RUN_COMMAND
  READ_SERIAL = 2,
  // Take input commands and output to motors
  // PARENT STATE: READ_TELEOP, READ_SERIAL | CHILD STATES: INITIAL_STATE
  RUN_COMMAND = 3;
};

// The setup routine runs once when you press reset.
void setup() {
  Serial.begin(9600);
}

// The loop routine runs over and over again forever.
void loop() {

  teleopState = false; // we want to have it default to no 
  if (digitalRead(PWM_INPUT_PIN_1) || digitalRead(PWM_INPUT_PIN_2)) {
    // we recieved a teleoperated signal
    teleopState = true;

  }

  motor.setSpeed(128);  // Run forward at 50% speed.
  Serial.println("128");
  delay(1000);
  
  motor.setSpeed(255);  // Run forward at full speed.
  Serial.println("255");
  delay(1000);

  motor.setSpeed(0);    // Stop.
  Serial.println("0");
  delay(1000);

  motor.setSpeed(-128);  // Run backward at 50% speed.
  Serial.println("-128");
  delay(1000);
  
  motor.setSpeed(-255);  // Run backward at full speed.
  Serial.println("-255");
  delay(1000);

  motor.setSpeed(0);    // Stop.
  Serial.println("0");
  delay(1000);
}
