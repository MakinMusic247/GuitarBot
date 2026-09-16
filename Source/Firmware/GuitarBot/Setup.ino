

void setupPWM(){
  // Initialize PWM library
  Serial.println("Initialize PWM");
  pwm.begin(); // start the pwm clock
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  reset();

  // Convert servo positions from degrees to pulse length (used by PWMServoDriver library)
  Serial.println("bridge servo pulse lengths: ");
  for(int i=0; i<3; i++){
    int angle = string_servo_positions[i];
    int pulselength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
    string_servo_pulses[i] = pulselength;
  }

  // Test servo positions
  Serial.println("Testing servo positions: ");
  for(int servonum = 0; servonum < 6; servonum++){
    Serial.print("Servo channel "); Serial.println(servonum);
    pluckString(servonum);
    delay(20);
  }

  Serial.println();

}
