
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// our servo # counter
uint8_t servonum = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("8 channel Servo test!");
//  pwm.resetDevices();

  pwm.begin();
//  pwm.setOscillatorFrequency(27000000);
//  pwm.setPWMFreq(50); //(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);
//  pwm.setPWM(servonum, 0, 600);
}

void loop() {
  Serial.println("test");
  delay(500);

  pwm.setPWM(0, 0, 150);
  delay(1000);
  pwm.setPWM(0, 0, 200);
  delay(1000);
  pwm.setPWM(0, 0, 250);
  delay(1000);

  // pulselength = map(degrees, 0, 180, SERVOMIN, SERVOMAX);
  
}
//  
//  // Drive each servo one at a time using setPWM()
//  Serial.println(servonum);
//  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
//    pwm.setPWM(servonum, 0, pulselen);
//  }
//
//  delay(500);
//  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
//    pwm.setPWM(servonum, 0, pulselen);
//    Serial.print("Pulse length:");
//    Serial.println(pulselen);
//  }

//  delay(500);
