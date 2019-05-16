int state = 0;
const int outStateBit0 = 15;
const int outStateBit1 = 16;
const int outStateBit2 = 25; // Note - pin 17 is the 3.3 V pin

void setup() {
    pinMode(outStateBit0, OUTPUT);
    pinMode(outStateBit1, OUTPUT);
    pinMode(outStateBit2, OUTPUT);

    // Will remain constant as the setup runs just once
    digitalWrite(outStateBit0, LOW);
    digitalWrite(outStateBit1, HIGH);
}

void loop() {
    // Write to pin 25 something that uses the ternary operator to determine which
    digitalWrite(outStateBit2, (state) ? HIGH : LOW);
    // Inverts the state so that next time it writes the opposite state
    state != state;
    // A 2.5 second delay
    delay(250);
}
