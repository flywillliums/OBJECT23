#include <Servo.h>  // Include the Servo.h library

const int POT_PIN_X = A0;
const int POT_PIN_Y = A1;
const int POT_PIN_Z = A2;
const int swPin = 2;
static int pinA = 2; // Our first hardware interrupt pin is digital pin 2
static int pinB = 3; // Our second hardware interrupt pin is digital pin 3
volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte encoderPos = 0; //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile byte oldEncPos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
volatile byte reading = 0; //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent

Servo servoA;
Servo servoB;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(7, OUTPUT);
  pinMode(POT_PIN_X, INPUT);
  pinMode(POT_PIN_Y, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  attachInterrupt(0,PinA,RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1,pinB,RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  servoA.attach(7); 
  servoB.attach(6); // attaches servo connected to Arduino pin 9 to the myservo object
  // Change pin # depending on where your servo is connected
}

void PinA(){
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  if(reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos --; //decrement the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void PinB(){
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos ++; //increment the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts
}
//the loop routine runs over and over again forever:
void loop(){
  pos = 0;
  Serial.print(analogRead(POT_PIN_X));
  Serial.print(" ");
  Serial.print(analogRead(POT_PIN_Y));
  Serial.print(" ");
  Serial.print(analogRead(POT_PIN_Z));
  Serial.print(" ");
  if(encoderPos == 13){
    encoderPos = 0;
    Serial.print(12);
  }
  else if(encoderPos == 255 ){
    encoderPos = 12;
    Serial.print(encoderPos);
  }
  else{
    Serial.print(encoderPos);
  }
  Serial.println(" ");
  delay(100);
  servoA.write(map(analogRead(POT_PIN_Y),40,1015,0,180));
  servoB.write(map(analogRead(POT_PIN_X),40,1015,0,180));              // tell servo to go to position in variable 'po
}