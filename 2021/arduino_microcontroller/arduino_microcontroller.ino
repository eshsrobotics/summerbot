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
const int PWM_INPUT_HORIZONTAL = 8;
const int PWM_INPUT_VERTICAL = 11;

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
}

// reads the pin, which has a PWM signal, scale it down from a number between 1000 and 200, and then turns it into a value between 0-99.
float findCommandParameter(int inputPin) {
  // We're expecting a pulse width between 1/1000Hz = 1000μs and 1/2000Hz =
  // 500μs.
  unsigned long pulseWidthMicroSeconds = pulseIn(inputPin, HIGH);

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
  const int PWM_MIN_FREQUENCY_HZ = 1000;
  const int PWM_MAX_FREQUENCY_HZ = 2000;
  float u = float(frequencyHz - PWM_MIN_FREQUENCY_HZ) / (PWM_MAX_FREQUENCY_HZ - PWM_MIN_FREQUENCY_HZ);

  // TODO: Add joystick deadzone around u==0.5 (theoretical dead center.)

  // Use the previous parameter of interpolation to calculate the rate at
  // which the robot will drive or turn.
  const int COMMAND_PARAMETER_MIN = 0;
  const int COMMAND_PARAMETER_MAX = 99;
  float commandParameter = COMMAND_PARAMETER_MIN + u * (COMMAND_PARAMETER_MAX - COMMAND_PARAMETER_MIN);
  return commandParameter;
}

// The loop routine runs over and over again forever.
void loop() {

  switch(state) {
    case INITIAL_STATE:
      // We ALWAYS receive a PWM signal from the FS-iA6B receiver, whether
      // the human is toching the transmitter or not.  Only the pulse width
      // can tell us this information.
      currentTurnParameter = int(findCommandParameter(PWM_INPUT_HORIZONTAL));
      currentDriveParameter = int(findCommandParameter(PWM_INPUT_VERTICAL));
      char buffer[100];
      snprintf(buffer, 100, "Current teleop parameters: D%02d, T%02d", currentDriveParameter, currentTurnParameter);
      Serial.println(buffer);

      if (currentDriveParameter != 50 || currentTurnParameter != 50) {
        // The user hit the joystick.
        Serial.println("Entered read_teleop");
        state = READ_TELEOP;
      }
      break;
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
  // delay(1000);
}
