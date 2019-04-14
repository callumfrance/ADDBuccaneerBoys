#include <Servo.h>

//declare pins for sensors
const int plankSensorIn = 1;
const int liftBallSensorIn = 2;
const int liftTopSensorIn = 3;      // sensor that detects lift has reached top
const int liftBottomSensorIn = 4;   // sensor that detects lift has reached bottom
const int cannonSensorIn = 5;
const int cannonTopSensorIn = 6;    // sensor that detects cannon has reached top
const int cannonBottomSensorIn = 7; // sensor that detects cannon has reached bottom

//declare actuator pins
const int plankServoPin = 8;
const int chestServoPin = 9;
const int liftMotorPinForward = 10;
const int liftMotorPinReverse = 11;
const int mastServoPin = 12;
const int cannonMotorPinForward = 13;
const int cannonMotorPinReverse = 14;

//declare FPGA output pins
const int plankSensorOut = 15;
const int liftBallSensorOut = 16;
const int cannonSensorOut = 17;

//declare FPGA input pins
const int state1 = 18;
const int state2 = 19;
const int state3 = 20;
const int state4 = 21;
const int state5 = 22;

//create servos
Servo plankServo;
Servo chestServo;
Servo mastServo;
Servo cannonServo;

//state is global - it identifies the current actions being undertaken
int state;

void setup() {
  //declare pin modes for sensors
  for (int i = 1; i < 8; i++) { // set first 7 (sensor) pins as inputs
    pinMode(i, INPUT);
  }

  //declare state output pins
  plankServo.attach(plankServoPin);
  pinMode(liftMotorPinForward, OUTPUT);
  pinMode(liftMotorPinReverse, OUTPUT);
  chestServo.attach(chestServoPin);
  mastServo.attach(mastServoPin);
  pinMode(cannonMotorPinForward, OUTPUT);
  pinMode(cannonMotorPinReverse, OUTPUT);

  //declare FPGA pinmodes
  pinMode(plankSensorOut, OUTPUT);
  pinMode(liftBallSensorOut, OUTPUT);
  pinMode(cannonSensorOut, OUTPUT);

  for (int i = 18; i < 23; i++) { // set the 5 states as inputs
    pinMode(i, INPUT);
  }

  state = 1; // the initial state is 1
}

void loop() {
  sense();
  if(digitalRead(state1) == 1){
    //state 1
    initialise();
    state = 0;
  }
  else if(digitalRead(state2) == 1){
    //state 2
    plankDown();
    state = 0;
  }
  else if(digitalRead(state3) == 1){
    //state 3
    chestDown();
    liftOperate();
    state = 0;
  }
  else if(digitalRead(state4) == 1){
    //state 4
    switchMastRight();
    liftOperate();
    state = 0;
  }
  else if(digitalRead(state5) == 1){
    //state 5
    cannonOperate();
    state = 1;
  }
    
}

void initialise(){
  plankUp();
  chestUp();
  liftReset();
  switchMastLeft();
  cannonReset();
}

void sense(){
  if(digitalRead(plankSensorIn) == 1){
    digitalWrite(plankSensorOut, HIGH);
  }
  else{
    digitalWrite(plankSensorOut, LOW);
  }

  if(digitalRead(liftBallSensorIn) == 1){
    digitalWrite(liftBallSensorOut, HIGH);
  }
  else{
    digitalWrite(liftBallSensorOut, LOW);
  }

  if(digitalRead(cannonSensorIn) == 1){
    digitalWrite(cannonSensorOut, HIGH);
  }
  else{
    digitalWrite(cannonSensorOut, LOW);
  }
}

void plankDown(){
  plankServo.write(90);
}

void plankUp(){
  plankServo.write(0);
}

void chestDown(){
  chestServo.write(0);
}

void chestUp(){
  chestServo.write(80);
}

void liftStop(){
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, LOW);
}

void liftForward(){
  digitalWrite(liftMotorPinForward, HIGH);
  digitalWrite(liftMotorPinReverse, LOW);
}

void liftReverse(){
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, HIGH);
}

void liftReset(){
  while(digitalRead(liftBottomSensorIn) != 1){
    liftReverse();
  }
}

void liftOperate(){
  while(digitalRead(liftTopSensorIn) != 1){
    liftForward();
  }
  delay(3);
  liftReset();
}

void switchMastRight(){
  mastServo.write(230);
}

void switchMastLeft(){
  mastServo.write(130);  
}

void cannonStop(){
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, LOW);
}

void cannonForward(){
  digitalWrite(liftMotorPinForward, HIGH);
  digitalWrite(liftMotorPinReverse, LOW);
}

void cannonReverse(){
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, HIGH);
}

void cannonReset(){
    while(digitalRead(cannonBottomSensorIn) != 1){
      cannonReverse();
    }
}

void cannonOperate(){
  while(digitalRead(cannonTopSensorIn) != 1){
    cannonForward();
  }
  delay(3);
  cannonReset();
}
