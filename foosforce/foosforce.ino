#include <SoftwareSerial.h>

#define resetPin 2
#define goalAPin 1
#define goalBPin 0
#define displayTxPin 8
#define displayRxPin 9

#define DIGIT1  0x7B
#define DIGIT2  0x7C
#define DIGIT3  0x7D
#define DIGIT4  0x7E

int minInteruptDelay = 1000; // 1s
int brightnessThreashold = 700;

int scoreA = 0;
int scoreB = 0;

int valA = 0;
int valB = 0;
int valAState = 0;
int valBState = 0;

unsigned long resetTime;
unsigned long scoreATime;
unsigned long scoreBTime;

SoftwareSerial Serial7Segment(displayRxPin, displayTxPin);

void setup() {
    pinMode(resetPin, INPUT);
    pinMode(displayTxPin, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(resetPin), reset, FALLING);

    Serial.begin(9600);
    Serial7Segment.begin(9600);

    sevenSegmentReset();
    updateDisplay();

    Serial.println("ready");
}


void sevenSegmentReset(){
    for (int i = 0; i<200; i++){
        for(int x = 0 ; x < 4 ; x++)
        {
            byte randNumber = random(0, 127); //Give me random number between 0 and 127
            Serial7Segment.write(DIGIT1 + x); //Control individual segments on a digit
            Serial7Segment.write(randNumber); //Turn on random segments
        }
    }

    Serial7Segment.write(DIGIT1);
    Serial7Segment.write(0b01000000);
    Serial7Segment.write(DIGIT2);
    Serial7Segment.write(0b01101101);
    Serial7Segment.write(DIGIT3);
    Serial7Segment.write(0b01100110);
    Serial7Segment.write(DIGIT4);
    Serial7Segment.write(0b00110111);

    delay(400);

    Serial7Segment.write(DIGIT1);
    Serial7Segment.write(0b01000000);
    Serial7Segment.write(DIGIT2);
    Serial7Segment.write(0b01010000);
    Serial7Segment.write(DIGIT3);
    Serial7Segment.write(0b01101101);
    Serial7Segment.write(DIGIT4);
    Serial7Segment.write(0b01110000);

    delay(800);
    // Reset the hardware
    Serial7Segment.write('v');
    // Let's turn on the colon (4 is colon)
    Serial7Segment.write(0x77);
    Serial7Segment.write( (1<<4) );

    // Let's setup the brightness to the max (0-255 values, 255 being the max)
    Serial7Segment.write(0x7A);
    Serial7Segment.write((byte) 255);
}

void reset() {
    if ((millis() - resetTime) > minInteruptDelay) {
        resetTime = millis();
        Serial.println("reset");
        scoreA = 0;
        scoreB = 0;
        sevenSegmentReset();
        updateDisplay();
    }
}


void displayGOAL(){
    Serial7Segment.write('v');
    for (int i = 0 ; i< 5 ; i++){
        Serial7Segment.write(DIGIT1);
        Serial7Segment.write(0b01101111);
        Serial7Segment.write(DIGIT2);
        Serial7Segment.write(0b00111111);
        Serial7Segment.write(DIGIT3);
        Serial7Segment.write(0b01110111);
        Serial7Segment.write(DIGIT4);
        Serial7Segment.write(0b00111000);

        delay(500);
        // Reset the hardware
        Serial7Segment.write('v');
        delay(200);
    }

    Serial7Segment.write(0x77);
    Serial7Segment.write( (1<<4) );
}

void goalA() {
    if ((millis() - scoreATime) > minInteruptDelay) {
        scoreATime = millis();
        Serial.println("score A");
        scoreA++;
        displayGOAL();
        updateDisplay();
    }
}

void goalB() {
    if ((millis() - scoreBTime) > minInteruptDelay) {
        scoreBTime = millis();
        Serial.println("score B");
        scoreB++;
        displayGOAL();
        updateDisplay();

    }
}

void updateDisplay() {
    char tempString[10];
    // Concat with leading zeros to the scoreboard
    sprintf(tempString, "%02d%02d", scoreA, scoreB); 
    Serial.println(tempString);

    Serial7Segment.print(tempString); //Send serial string out the soft serial port to the S7S
}

void loop(){
    valA = analogRead(goalAPin);
    if (valA < brightnessThreashold && (valAState == 1)) {
        valAState = 0;
        goalA();
    }else if (valA > 700 && (valAState == 0)) {
        valAState = 1;
    }
    valB = analogRead(goalBPin);
    if (valB < brightnessThreashold && (valBState == 1)) {
        valBState = 0;
        goalB();
    }else if (valB > 700 && (valBState == 0)) {
        valBState = 1;
    }
}

