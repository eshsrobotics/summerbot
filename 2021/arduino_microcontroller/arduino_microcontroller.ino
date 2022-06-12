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
//#include <cstring>

/* ===== PWM PINS ===== */

// Going to the Cytron Motor Driver.
const int PWM_OUTPUT_PIN_1 = 2;
const int PWM_OUTPUT_PIN_2 = 5;

const int DIR_OUTPUT_PIN_1 = 3;
const int DIR_OUTPUT_PIN_2 = 6;

// Inputs are coming from the reciever (fly sky something).
const int PWM_INPUT_HORIZONTAL = 8;  // Blue wire
const int PWM_INPUT_VERTICAL = 11;   // White wire


// ==================== */

// Configure the motor driver.
// CytronMD motor(PWM_DIR, 12, 13);  // PWM = Pin 3, DIR = Pin 4.
CytronMD motor1(PWM_DIR, PWM_OUTPUT_PIN_1, DIR_OUTPUT_PIN_1);
CytronMD motor2(PWM_DIR, PWM_OUTPUT_PIN_2, DIR_OUTPUT_PIN_2);


enum ROBOT_STATE {
  // Start of the loop; waiting for input.
  // PARENT STATE: NONE | CHILD STATES: READ_TELEOP, READ_SERIAL
  INITIAL_STATE = 0,

  // Read from fly sky reciever and generate command.
  // PARENT STATE: INITIAL_STATE | CHILD STATES: RUN_COMMAND
  READ_TELEOP = 1,

  // Reads and parses serial commands from the raspberry pi i.e. also check vision.
  // PARENT STATE: INITIAL_STATE | CHILD STATES: RUN_COMMAND
  READ_SERIAL = 2,

  // Take input commands (3-character strings) and output to motors.
  // The following commands are recognized:
  // * Txx: Turns clockwise at rate xx.
  //        xx == 0: Turn counterclockwise at full speed.
  //        xx == 50: Stop turning.
  //        xx == 99: Turn clockwise at full speed.
  // * Dxx: Drive forward at rate xx.
  //        xx == 0: Drive backwards at full speed.
  //        xx == 50: Stop driving.
  //        xx == 99: Drive forwards at full speed.
  // PARENT STATE: READ_TELEOP, READ_SERIAL | CHILD STATES: INITIAL_STATE
  RUN_COMMAND = 3
};

// Tracks current state, global because it needs to persist beyond the end of loop().
int state = INITIAL_STATE;


// How fast the robot is driving fowards and backwards, from 0 - 99
int currentDriveParameter = 50;

// How fast the robot is turning, from 0 - 99
int currentTurnParameter = 50;

// The setup routine runs once when you press reset.
void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(PWM_INPUT_HORIZONTAL, INPUT);
  pinMode(PWM_INPUT_VERTICAL, INPUT);
  pinMode(PWM_OUTPUT_PIN_1, OUTPUT);
  pinMode(PWM_OUTPUT_PIN_2, OUTPUT);
  pinMode(DIR_OUTPUT_PIN_1, OUTPUT);
  pinMode(DIR_OUTPUT_PIN_2, OUTPUT);
}

// reads the pin, which has a PWM signal, scale it down from a number between 1000 and 2000, and then turns it into a value between 0-99.
float findCommandParameter(int inputPin) {
  // We're expecting a pulse width between 1000μs and 2000μs.
  unsigned long pulseWidthMicroSeconds = pulseIn(inputPin, HIGH);

  if (pulseWidthMicroSeconds == 0) {
    // No complete pulse detected within the time period.
    // Returning the most neutral number possible.
    return 50;
  }

  // An integer division should be fine for the frequency -- we don't need
  // fine granularity here.
  int frequencyHz = 1e6 / pulseWidthMicroSeconds;

  // Convert the input frequency into a parameter of interpolation.  0 <= u <=
  // 1, *if* the PWM signal corresponds to the pulse width above.
  //
  // - If the frequency is less than PWM_MIN_FREQUENCY_HZ, then u will be
  //   less than 0.
  //
  // - If the frequency is great than PWM_MAX_PULSE_WIDTH_US, then u will be
  //   greater than 1.
  //
  // It is important that we work in the non-linear frequency domain here.
  // We'll get subtle errors if we work in the time domain
  // ("PWM_MIN_PULSE_WIDTH_US").
  int PWM_MIN_PULSE_WIDTH_US = 1300;
  int PWM_MAX_PULSE_WIDTH_US = 2500;
  if (inputPin == PWM_INPUT_HORIZONTAL) {
    // Based on the observation that the horizontal channel (channel 1) behaves differently than the vertical channel (channel 2)
    PWM_MIN_PULSE_WIDTH_US = 1078;
    PWM_MAX_PULSE_WIDTH_US = 2253;
  }

  // dd joystick deadzone around u==0.5 (theoretical dead center.)
  // const unsigned long deadzoneThresholdMicroseconds = 100;
  // const unsigned long centerPulseWidthMicroseconds = (PWM_MAX_PULSE_WIDTH_US + PWM_MIN_PULSE_WIDTH_US) / 2;

  // if (pulseWidthMicroSeconds < centerPulseWidthMicroseconds + deadzoneThresholdMicroseconds && pulseWidthMicroSeconds > centerPulseWidthMicroseconds - deadzoneThresholdMicroseconds) {
  //   pulseWidthMicroSeconds = centerPulseWidthMicroseconds;
  // }

  float u = float(long(pulseWidthMicroSeconds) - PWM_MIN_PULSE_WIDTH_US) / (PWM_MAX_PULSE_WIDTH_US - PWM_MIN_PULSE_WIDTH_US);

  // This is a clamp which will make sure if the value of u (which determines motor speed) is a number that we cannot use (higher than 1 or lower than 0)
  // that it will be set to a value which we can use
  if (u < 0) {
    u = 0;
  }
  if (u > 1) {
    u = 1;
  }

  // Deadband calculation
  const float DEADZONE_THRESHOLD = 0.18;
  if (u < 0.5 + DEADZONE_THRESHOLD && u > 0.5 - DEADZONE_THRESHOLD) {
    u = 0.5;
  }

  // Use the previous parameter of interpolation to calculate the rate at
  // which the robot will drive or turn.
  const int COMMAND_PARAMETER_MIN = 0;
  const int COMMAND_PARAMETER_MAX = 99;
  // float commandParameter = map(frequencyHz, PWM_MIN_FREQUENCY_HZ, PWM_MAX_FREQUENCY_HZ, COMMAND_PARAMETER_MIN, COMMAND_PARAMETER_MAX);
  float commandParameter = COMMAND_PARAMETER_MIN + u * (COMMAND_PARAMETER_MAX - COMMAND_PARAMETER_MIN);

  char buffer[100];
  snprintf(buffer, 100, "Current teleop parameters: Freq: %d Hz, Pulse width: %ld us -> u=%.2f, param=%02f", frequencyHz, pulseWidthMicroSeconds, u, commandParameter);
  // Serial.println(buffer);


  return commandParameter;
}


// The loop routine runs over and over again forever.
void loop() {

  switch (state) {
    case INITIAL_STATE:
      {
        // We ALWAYS receive a PWM signal from the FS-iA6B receiver, whether
        // the human is touching the transmitter or not.  Only the pulse width
        // can tell us this information.
        int currentTurnParameter = int(findCommandParameter(PWM_INPUT_HORIZONTAL));
        int currentDriveParameter = int(findCommandParameter(PWM_INPUT_VERTICAL));

        char buffer[100];
        snprintf(buffer, 100, "Current teleop parameters: D%02f (%02d), T%02f (%02d)",
                 (float)currentDriveParameter,
                 currentDriveParameter,
                 (float)currentTurnParameter,
                 currentTurnParameter);
        Serial.println(buffer);

        if (currentDriveParameter != 50 || currentTurnParameter != 50) {
          // The user hit the joystick.
          // Serial.println("Entered read_teleop");
          // state = READ_TELEOP;
        }
        break;
      }
    case READ_TELEOP:
      break;
    case READ_SERIAL:
      break;
    case RUN_COMMAND:
      break;
  }

  // motor.setSpeed(128);  // Run forward at 50% speed.
  // Serial.println("128");
  // delay(1000);

  // motor.setSpeed(255);  // Run forward at full speed.
  // Serial.println("255");
  // delay(1000);

  // motor.setSpeed(0);    // Stop.
  // Serial.println("0");
  // delay(1000);

  // motor.setSpeed(-128);  // Run backward at 50% speed.
  // Serial.println("-128");
  // delay(1000);

  // motor.setSpeed(-255);  // Run backward at full speed.
  // Serial.println("-255");
  // delay(1000);

  // motor.setSpeed(0);    // Stop.
  // Serial.println("0");
  delay(400);
}