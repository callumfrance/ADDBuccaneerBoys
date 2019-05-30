#include <Servo.h>

// declare pins for sensors
const int plankSensorIn = 14;
const int liftBallSensorIn = 15;
const int liftTopSensorIn = 16;
const int cannonTopSensorIn = 17;
const int cannonBottomSensorIn = 18;

// declare actuator pins
const int plankServoPin = 12;
const int chestServoPin = 10;
const int liftMotorPinForward = 2;
const int liftMotorPinReverse = 3;
const int mastServoPin = 11;
const int cannonMotorPinForward = 4;
const int cannonMotorPinReverse = 5;

const int stateOutput = 6;
const int binOne = 7;
const int binTwo = 8;
const int binThree = 9;

// create servos
Servo plankServo;
Servo chestServo;
Servo mastServo;

// state is global - it identifies the current actions being undertaken
int state = 1;
int cannonInitialized = 0;
int liftInit = 0;

void setup() {
  Serial.begin(115200);
  // declare pin modes for sensors
  pinMode(plankSensorIn, INPUT);
  pinMode(liftBallSensorIn, INPUT);
  pinMode(liftTopSensorIn, INPUT);
  pinMode(cannonTopSensorIn, INPUT);
  pinMode(cannonBottomSensorIn, INPUT);
  pinMode(plankServoPin, OUTPUT);
  pinMode(chestServoPin, OUTPUT);
  pinMode(mastServoPin, OUTPUT);
  pinMode(liftMotorPinForward, OUTPUT);
  pinMode(liftMotorPinReverse, OUTPUT);
  pinMode(cannonMotorPinForward, OUTPUT);
  pinMode(cannonMotorPinReverse, OUTPUT);
  pinMode(stateOutput, OUTPUT);
  pinMode(binOne, INPUT);
  pinMode(binTwo, INPUT);
  pinMode(binThree, INPUT);
  
  plankServo.attach(plankServoPin);
  chestServo.attach(chestServoPin);
  mastServo.attach(mastServoPin);

  digitalWrite(cannonMotorPinForward, LOW);
  digitalWrite(cannonMotorPinReverse, HIGH);
  delay(500);
  digitalWrite(cannonMotorPinForward, LOW);
  digitalWrite(cannonMotorPinReverse, LOW);
  delay(1000);
  liftInit = 0;

}

void loop() {

  int a = digitalRead(binThree) == HIGH;
  int b = digitalRead(binTwo) == HIGH;
  int c = digitalRead(binOne) == HIGH;

  if(!a && !b && !c){
    state = 1;
    Serial.println("State 1");
  }
  else if(!a && !b && c){
    state = 2;
    Serial.println("State 2");
  }
  else if(!a && b && !c){
    state = 3;
    Serial.println("State 3");
  }
  else if(!a && b && c){
    state = 4;
    Serial.println("State 4");
  }
  else if(a && !b && !c){
    state = 5;
    Serial.println("State 5");
  }
  else if(a && !b && c){
    state = 6;
    Serial.println("State 6");
  }
  else if(a && b && !c){
    state = 7;
    Serial.println("State 7");
  }
  else if(a && b && c){
    state = 8;
    Serial.println("State 8");
  }

  switch(state) {      // the functionality of each state is given inside here
  case 0:
    ; // when all of the state pins are LOW, just do the same thing as if in the first state
  case 1:
    Serial.println("State 1");
    Serial.println("Switching mast");
    switchMastLeft();
    Serial.println("Reversing cannon");
    cannonInitialized = 0;
    while(cannonInitialized == 0){
      Serial.println(cannonInitialized);
      cannonReverse();
    }
    delay(300);
    Serial.println("Plank goes up");
    plankUp();
    Serial.println("Chest goes up");
    delay(300);
    chestUp();
    Serial.println("Lift Rises");
    delay(300);
    liftInit = 0;
    while(liftInit == 0){
      liftForwardNoStateSwitch(); 
    }
    liftInit = 0;
    delay(300);
    Serial.println("Lift Lowers");
    liftReverse();
    while(state == 1){
      Serial.println("Waits for input");
      if(digitalRead(plankSensorIn) == HIGH){
        Serial.println("Got it");
        switchState();
      }
    }
    break;
  case 2:
    delay(300);
    Serial.println("State 2");
    plankDown();
    while(state == 2){
      delay(10);
      if(digitalRead(liftBallSensorIn) == HIGH){
        switchState();
      }
    }
    break;
  case 3:
  delay(300);
    Serial.println("State 3");
    delay(100);
    liftForward();
    break;
  case 4:
  delay(300);
    Serial.println("State 4");
    delay(300);
    while(state == 4){
      if(digitalRead(liftBallSensorIn) == HIGH){
        liftReverseWithSwitch();
      }
    }
    break;
  case 5:
    Serial.println("State 5");
    delay(300);
    while(state == 5){
      liftForward();
    }
    break;
  case 6:
    Serial.println("State 6");
    delay(300);
    while(state == 6){
      if(digitalRead(liftBallSensorIn) == HIGH){
        liftReverseWithSwitch();
      }
    }
    break;
  case 7:
    Serial.println("State 7");
    switchMastRight();
    chestDown();
      while(state == 7){
        delay(100);
        liftForward();
      }
    break;
  case 8:
    Serial.println("State 8");
    delay(1000);
    liftReverse();
    while(state == 8){
      cannonForward();
    }
    break;
  default:
    ; // state was incorrect, or incorrectly read, or just 'default'
  }
}

void plankUp() {
  int pos;
  for (pos = 0; pos <= 115; pos += 1) {
    plankServo.write(pos);
    delay(10);
  }
}

void plankDown() {
  int pos;
  for (pos = 115; pos >= 0; pos -= 1) {
    plankServo.write(pos);
    delay(5);
  }
}

void chestDown() {
  chestServo.write(180);
}

void chestUp() {
  chestServo.write(140);
}

void liftStop() {
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, LOW);
}

void liftReverse() {
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, HIGH);
  delay(6000);
  liftStop();
}

void liftReverseWithSwitch() {
  digitalWrite(liftMotorPinForward, LOW);
  digitalWrite(liftMotorPinReverse, HIGH);
  delay(6000);
  liftStop();
  switchState();
}

void liftForward() {
  delay(300);
  if(digitalRead(liftTopSensorIn) == LOW) {
    digitalWrite(liftMotorPinForward, HIGH);
    digitalWrite(liftMotorPinReverse, LOW);
  }
  else{
    liftStop();
    switchState();
  }
}

void liftForwardNoStateSwitch() {
  delay(10);
  if(digitalRead(liftTopSensorIn) == LOW) {
    digitalWrite(liftMotorPinForward, HIGH);
    digitalWrite(liftMotorPinReverse, LOW);
  }
  else{
    liftStop();
    liftInit = 1;
  }
}

void switchMastRight() {
  mastServo.write(140);
}

void switchMastLeft() {
  mastServo.write(40);  
}

void cannonStop() {
  digitalWrite(cannonMotorPinForward, LOW);
  digitalWrite(cannonMotorPinReverse, LOW);
}

void cannonForward() {
  delay(100);
  if(digitalRead(cannonTopSensorIn) == LOW) {
    digitalWrite(cannonMotorPinForward, LOW);
    digitalWrite(cannonMotorPinReverse, HIGH);
  }
  else{
    cannonStop();
      digitalWrite(cannonMotorPinForward, HIGH);
      digitalWrite(cannonMotorPinReverse, LOW);
      delay(1000);
    cannonStop();
  }
}

void cannonReverse() {
  while(cannonInitialized != 1){
    delay(100);
    if(digitalRead(cannonBottomSensorIn) == LOW) {
      digitalWrite(cannonMotorPinForward, HIGH);
      digitalWrite(cannonMotorPinReverse, LOW);
    }
    else{
      cannonStop();
      cannonInitialized = 1;
    }
  }
}

void switchState(){
  delay(50);
  digitalWrite(stateOutput, HIGH);
  delay(100);
  digitalWrite(stateOutput, LOW);
  delay(2000);
  Serial.println("PULSE");
  int a; 
  if(digitalRead(binThree) == HIGH){
    a = 1;
  }
  else{
    a = 0;
  }
  int b; 
    if(digitalRead(binTwo) == HIGH){
    b = 1;
  }
  else{
    b = 0;
  }
  int c; 
  if(digitalRead(binOne) == HIGH){
    c = 1;
  }
  else{
    c = 0;
  }

Serial.println(a);
Serial.println(b);
Serial.println(c);
  
  if(!a && !b && !c){
    state = 1;
    Serial.println("State 1");
  }
  else if(!a && !b && c){
    state = 2;
    Serial.println("State 2");
  }
  else if(!a && b && !c){
    state = 3;
    Serial.println("State 3");
  }
  else if(!a && b && c){
    state = 4;
    Serial.println("State 4");
  }
  else if(a && !b && !c){
    state = 5;
    Serial.println("State 5");
  }
  else if(a && !b && c){
    state = 6;
    Serial.println("State 6");
  }
  else if(a && b && !c){
    state = 7;
    Serial.println("State 7");
  }
  else if(a && b && c){
    state = 8;
    Serial.println("State 8");
  }
  

//    state = state + 1;
}

