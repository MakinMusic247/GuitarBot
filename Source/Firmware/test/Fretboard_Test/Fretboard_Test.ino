/*
 * Test servo motors of for the GuitarBot fretboard hardware
 * 1. Initialize an array of type fretpin (pin, direction)
 *    - pin = the arduino digital output pin
 *    - direction = direction servo motor turns to press fret (1=clockwise, -1=anticlockwise)
 * 2. Setup
 *    - set the digital pins to OUTPUT mode and attach the servos
 *    - set servos to initial position (90 degrees)
 * 3. Main loop:
 *    - Go through each servo and turn clockwise and anticlockwise
 */
#include <Servo.h>

int pinEA = 8;
int pinDG = 9;
int pinBe = 10;

typedef struct {
    int pin;        // digital pin number
    int _direction; // direction of the servo (clockwise=1, counterclockwise=-1)
} fretpin;    

fretpin fretpins[6] = {
    {22, -1}, // E
    {22,  1}, // A
    {23, -1}, // D
    {23,  1}, // G
    {24, -1}, // B
    {24,  1}, // e
};

Servo servoEA;
Servo servoDG;
Servo servoBe;

void setup() {
  Serial.begin(9600); // open the serial port at 9600 bps:
  // Set digital pins to OUTPUT mode
  pinMode(pinEA, OUTPUT);
  pinMode(pinDG, OUTPUT);
  pinMode(pinBe, OUTPUT);

  // Attach servo motors
  servoEA.attach(pinEA);
  servoDG.attach(pinDG);
  servoBe.attach(pinBe);

  // Set initial servo position (90 degrees)
  servoEA.write(90);
  delay(1000);
  servoDG.write(90);
  delay(1000);
  servoBe.write(90);
  delay(1000);

}

void loop() {
  // servo 1
  Serial.println("E");
  servoEA.write(170);
  delay(2000);
  servoEA.write(90);
  delay(2000);
  Serial.println("A");
  servoEA.write(10);
  delay(2000);
  servoEA.write(90);
  delay(2000);

  // servo 2
  Serial.println("E");
  servoDG.write(170);
  delay(2000);
  servoDG.write(90);
  delay(2000);
  Serial.println("A");
  servoDG.write(10);
  delay(2000);
  servoDG.write(90);
  delay(2000);

  // servo 3
  Serial.println("E");
  servoBe.write(170);
  delay(2000);
  servoBe.write(90);
  delay(2000);
  Serial.println("A");
  servoBe.write(10);
  delay(2000);
  servoBe.write(90);
  delay(2000);

}
