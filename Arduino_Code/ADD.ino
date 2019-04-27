#include <Servo.h>

// declare pins for sensors
const int plankSensorIn = 1;
const int liftBallSensorIn = 2;
const int liftTopSensorIn = 3;      // sensor that detects lift has reached top
const int liftBottomSensorIn = 4;   // sensor that detects lift has reached bottom
const int cannonSensorIn = 5;
const int cannonTopSensorIn = 6;    // sensor that detects cannon has reached top
const int cannonBottomSensorIn = 7; // sensor that detects cannon has reached bottom

// declare actuator pins
const int plankServoPin = 8;
const int chestServoPin = 9;
const int liftMotorPinForward = 10;
const int liftMotorPinReverse = 11;
const int mastServoPin = 12;
const int cannonMotorPinForward = 13;
const int cannonMotorPinReverse = 14;

// declare FPGA output pins
const int outStateBit0 = 15;
const int outStateBit1 = 16;

// declare FPGA input pins
// const int state1 = 18;
// const int state2 = 19;
// const int state3 = 20;
// const int state4 = 21;
// const int state5 = 22;

// create servos
Servo plankServo;
Servo chestServo;
Servo mastServo;
Servo cannonServo;

// state is global - it identifies the current actions being undertaken
int bitwiseState;

void setup() {
  // declare pin modes for sensors
  for (int i = 1; i < 8; i++) { // set first 7 (sensor) pins as inputs
    pinMode(i, INPUT);
  }

  // declare state output pins
  plankServo.attach(plankServoPin);
  pinMode(liftMotorPinForward, OUTPUT);
  pinMode(liftMotorPinReverse, OUTPUT);
  chestServo.attach(chestServoPin);
  mastServo.attach(mastServoPin);
  pinMode(cannonMotorPinForward, OUTPUT);
  pinMode(cannonMotorPinReverse, OUTPUT);

  //declare FPGA pinmodes
  pinMode(outStateBit0, OUTPUT);
  pinMode(outStateBit1, OUTPUT);

  for (int i = 18; i < 23; i++) { // set the 5 states as inputs
    pinMode(i, INPUT);
  }
  
  bitwiseState = 1;  // the initial state
}

void loop() {
  sense();              // update the pins the FPGA reads from the Arduino
  bitwiseState = 0;         // empty the bitwiseState so that values can be read into it again
  for(int x = 18; x < 23; x++) {  // iterate over the incoming state pins
    if(digitalRead(x) == HIGH) {
        bitwiseState |= 1<<(x-18);  // every HIGH pin is bit shifted into the integer
    }
  } // the end result of the for loop is one integer, representing the state

  switch(bitwiseState) {      // the functionality of each state is given inside here
  case 0:
    ; // when all of the state pins are LOW, just do the same thing as if in the first state
  case 1:
    initialise();
    break;
  case 2:
    plankDown();
    break;
  case 3:
    chestDown();
    liftForward();
    break;
  case 4:
    liftReverse();
    break;
  case 5:
    liftStop();
    break;
  case 6:
    liftForward();
    switchMastRight();
    break;
  case 7:
    liftReverse();
    break;
  case 8:
    liftStop();
    break;
  case 9:
    cannonForward();
    break;
  case 10:
    cannonReverse();
    break;
  default:
    ; // state was incorrect, or incorrectly read, or just 'default'
  }
}

void initialise() {
// The initial state that the crazy machine should be set to
  plankUp();
  chestUp();
  liftReset();
  switchMastLeft();
  cannonReset();
}

void sense() {
    int activeStates = 0;
    int outBitState = 0;

// First we need to see which sensors are currently activated (should be 1 max)
    if (digitalRead(plankSensorIn)) {
        activeStates++;
        outBitState = 1;
    }
    if (digitalRead(liftBallSensorIn)) {
        activeStates++;
        outBitState = 2;
    }
    if (digitalRead(cannonSensorIn)) {
        activeStates++;
        outBitState = 3;
    }
// Next, we need to relay the sensor state to the FPGA
// To do this, we use binary states
    // pin 15 is the 1's binary digit
    // pin 16 is the 2's binary digit
        // 00 == no sensors are active
        // 01 == plankSensorState
        // 10 == liftBallSensorIn
        // 11 == cannonSensorIn
    // do nothing on error (else statement)
    if (activeStates < 2) {
        int x = 15;
        do {
            digitalWrite(x, outBitState % 2);   // modulo
            outBitState = outBitState / 2;      // integer division
            x++;
        } while (x < 17);
    } else {
        ; // the arduino is reading two sensors as high at the same time...
    }
}

void plankDown() {
  plankServo.write(90);
}

void plankUp() {
  plankServo.write(0);
}

void chestDown() {
  chestServo.write(0);
}

void chestUp() {
  chestServo.write(80);
}

void liftStop() {
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, LOW);
}

void liftForward() {
  digitalWrite(liftMotorPinForward, HIGH);
  digitalWrite(liftMotorPinReverse, LOW);
}

void liftReverse() {
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, HIGH);
}

void liftReset() {
  while(digitalRead(liftBottomSensorIn) != 1) {
    liftReverse();
  }
}

void liftOperate() {
  while(digitalRead(liftTopSensorIn) != 1) {
    liftForward();
  }
  delay(3);
  liftReset();
}

void switchMastRight() {
  mastServo.write(230);
}

void switchMastLeft() {
  mastServo.write(130);  
}

void cannonStop() {
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, LOW);
}

void cannonForward() {
  digitalWrite(liftMotorPinForward, HIGH);
  digitalWrite(liftMotorPinReverse, LOW);
}

void cannonReverse() {
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, HIGH);
}

void cannonReset() {
    while(digitalRead(cannonBottomSensorIn) != 1) {
      cannonReverse();
    }
}

void cannonOperate() {
  while(digitalRead(cannonTopSensorIn) != 1) {
    cannonForward();
  }
  delay(3);
  cannonReset();
}
