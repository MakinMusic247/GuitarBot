/*
 * Sketch to convert a guitar tab string into an array and play song on guitar (via solenoids and servos)
 * 1. Select song (from string in code or sd card)
 * 2. Get length of song (how many notes) and note period (how long between notes) - based on tempo/time signature?
 * 3. Initialize 'tab' array // each line represents the 6 strings of the guitar. -1 means don't pluck string, numbers refer to frets
 * 4. In loop (or ISR), loop through each array index
 *  a. get the corresponding string and fret pins 
 *  b. match int to servo motor channel
 *  c. Press down on frets and get list of strings to play
 *  d. pluck strings
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN               150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX               600 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_POS_CENTRE       85  // default angle of servo motor (slightly left of 90 degrees so it doesn't get stuck on string)
#define SERVO_POS_LEFT         60  // angle of servo to the left of the string
#define SERVO_POS_RIGHT        120 // angle of servo to the right of the string
#define SERVO_FRET_POS_CENTRE  90  // default angle of servo motor (for frets)
#define SERVO_FRET_POS_LEFT    60  // angle of servo to the left side fret
#define SERVO_FRET_POS_RIGHT   120 // angle of servo to the right side fret
#define SERVO_FREQ             50  // Analog servos run at ~50 Hz updates

int string_servo_positions[3] = {SERVO_POS_LEFT, SERVO_POS_CENTRE, SERVO_POS_RIGHT};   // angles to position each bridge servo (85 is the default centre position - string should be located at 90 degrees)
int string_servo_pulses[3] = {0, 0, 0};          // Corresponding pulse length to the servo angle

int MAX_FRET_NUMBER = 2; // highest fret that has solenoid connected
int tabLength = 0;
int period = 500; // milliseconds to delay between each note/s
unsigned long time_now = 0; // hold the millisecond delay

typedef struct {
    int channel;      // channel of servo motor
    int _currentPos;  // position of the servo (left=SERVOMIN, right=SERVOMAX)
} stringServo; 

// {<channel num>, <current position>}
stringServo strings[6] = {
    {0, -1}, // E
    {1, -1}, // A
    {2, -1}, // D
    {3, -1}, // G
    {4, -1}, // B
    {5, -1}, // e
};


// TODO: add string 'note' field to map between MIDI note and 
typedef struct {
    int channel;    
    int _direction;  // direction that the servo needs to move to press the fret (left=-1, right=1)
    int _currentPos; // current position of the servo motor (left=-1, centre=0, right=1)
} fret; 


// {<channel num>, <direction>, <current position>}
fret frets[6] = {
    {6,  -1, 0}, // E
    {7,   1, 0}, // A
    {8,  -1, 0}, // D
    {9,   1, 0}, // G
    {10, -1, 0}, // B
    {11,  1, 0}, // e
};


/* Digital pins outputting to fretboard circuit */
// example: fret = fretPins[notes[i]][i]; // E.g [1][6] => [<fret 1>][<high E string>]
fret fretPins[][6] { // in form [fret][string] == <digital pin number>
                      { {-1,0,0} , {-1,0,0} , {-1,0,0} , {-1,0,0} , {-1,0,0} , {-1,0,0} },  // open string, ignore 
                      { {6,-1,0} , {6, 1,0} , {7,-1,0} , {7, 1,0} , {8,-1,0} , {8, 1,0} },  // fret 1, strings 1->6
                      { {9,-1,0} , {9, 1,0} , {10,-1,0}, {10, 1,0}, {11,-1,0}, {11, 1,0}},  // fret 2, strings 1->6
                   }; 


/* 
 *  Guitar tab to be played 
 *  each array of length 6 corresponds to the strings {E,A,D,G,B,E} and the fret to be played
 *  -1 => no note played on string, 0 => open string, 1 => fret 1 of string, ...
*/
int tab[][6]  { // each array of size 6
                {-1, -1, -1, -1, -1, -1},  // first note/s to be played
                { 0, -1, -1, -1, -1, -1},  // 2nd note/s to be played
                {-1,  0, -1, -1, -1, -1},  // 3rd ...
                {-1, -1,  0, -1, -1, -1},
                {-1, -1, -1,  0, -1, -1},
                {-1, -1, -1, -1,  0, -1},
                {-1, -1, -1, -1, -1,  0},
                {-1, -1, -1, -1, -1, -1},
                { 1, -1, -1, -1, -1, -1},
                {-1,  1, -1, -1, -1, -1},
                {-1, -1,  1, -1, -1, -1},
                {-1, -1, -1,  1, -1, -1},
                {-1, -1, -1, -1,  1, -1},
                {-1, -1, -1, -1, -1,  1},
                {-1, -1, -1, -1, -1, -1}
              };



void setup() {
  Serial.begin(115200); // 115200
  // get the number of entries of the tab
  tabLength = sizeof(tab)/sizeof(tab[0]); Serial.print("Tab length = "); Serial.println(tabLength);

  setupPWM();

  Serial.println("************************************************************************");
  delay(2000); // wait for 2 seconds before beginning
  // TODO: have button press to play song (so I can set up the servo position relative to the string) 

  // Loop through each note/chord of tab and activate relevant solenoids/motors
  for(int i=0; i<tabLength; i++){
    Serial.print("phrase "); Serial.print(i); Serial.println(" ");
    play(tab[i]);
    delay(period);
    Serial.println();
  }

  reset();
  
}


/* 
 *  Play a single line of tab
 *  1. For each int in array, set the corresponding solenoids (-1 = off, <int> = on)
 *  2. Once all solenoids set, strum servo on corresponding strings
 */
void play(int notes[6]){

  /* 1. Tilt servo to push down corresponding fretboard note */
  // loop through note played on each string (E->A->D->G->B->E) and activate corresponding solenoid
  for(int i=0; i<6; i++){
//    fret f = -1;
    int string = -1;

    if(notes[i]>0 && notes[i] <= MAX_FRET_NUMBER){ // push down on fret
      fret f = fretPins[notes[i]][i]; // E.g [1][6] => [<fret 1>][<high E string>]
      Serial.print("fret "); Serial.print(notes[i]); Serial.print(" |"); Serial.print(" servo channel "); Serial.println(f.channel);
      pressFret(f);
    }
    else{ } // note is 0, -1 or > max fret number allowed, do nothing
  }

  
  /* 2. Pluck required strings */
  // If note is 0 --> MAX_FRET_NUMBER, activate the corresponding servo to pluck the string
  for(int i=0; i<6; i++){
    int string = -1;

    if(notes[i]>-1){
      if(notes[i]>=1){
        Serial.print("string "); Serial.print(i+1); Serial.println(" |"); //Serial.print(" fret "); Serial.println(notes[i]);
      }else{
        Serial.print("string "); Serial.print(i+1); Serial.print(" |"); Serial.println(" open string "); 
      }
    }
    
    delay(10);
    
    if(notes[i]>=0 && notes[i] <= MAX_FRET_NUMBER){ // check if current string needs to be plucked
      pluckString(i);
      delay(20);
    }
  }

  /* 3. Reset frets/strings to original position */

}


void pressFret(fret f){
  int angle = SERVO_FRET_POS_CENTRE;

  // Get the servo channel (if == -1, move servo to centre position)
  if(f.channel == -1)         angle = SERVO_FRET_POS_CENTRE; // open string --> move servo to centre position
  else if(f._direction == -1) angle = SERVO_FRET_POS_LEFT;   // 
  else if(f._direction == 1)  angle = SERVO_FRET_POS_RIGHT;  // 
  else                        angle = SERVO_FRET_POS_CENTRE; // 

  // move to current fret using pwm
  Serial.print("move to angle angle: "); Serial.println(angle);
  int pulselength = map(angle, 0, 180, SERVOMIN, SERVOMAX); 
  // hit the fret
  if(f.channel >= 0) pwm.setPWM(f.channel, 0, pulselength);  
}



// Pick the guitar string with the corresponding servo motor to the desired angle
// TODO: use pulselength mapping table string_servo_pulses[] to avoiding using map() function each time 
void pluckString(int servonum){
  int angle = 0;
  // Get servo current position
  int pos = strings[servonum]._currentPos;  
  
  if(pos < 0)                       angle = SERVO_POS_LEFT; // go to default position
  else if(pos == SERVO_POS_CENTRE)  angle = SERVO_POS_RIGHT;  // CENTRE (slightly left --> right)
  else if(pos == SERVO_POS_LEFT)    angle = SERVO_POS_RIGHT;  // Left --> right
  else                              angle = SERVO_POS_LEFT;   // 

  Serial.print("current angle: "); Serial.print(pos); Serial.print(" --> "); Serial.print("new angle: "); Serial.println(angle);
  
  // Convert angle into pulse
  int pulselength = map(angle, 0, 180, SERVOMIN, SERVOMAX);   
  Serial.print(angle); Serial.print(" --> "); Serial.println(pulselength);

  // play the string
  pwm.setPWM(servonum, 0, pulselength);

  // Update the current position of the servo
  strings[servonum]._currentPos = angle;
}


// reset servos to original position
void reset(){
  Serial.print("Resetting...");
  int pulselength = map(90, 0, 180, SERVOMIN, SERVOMAX);
  for(int servonum = 0; servonum < 6; servonum++){
    pwm.setPWM(servonum, 0, pulselength);  
  }

  for(int servonum = 6; servonum < 9; servonum++){
    pwm.setPWM(servonum, 0, pulselength);  
  }
}


void loop() {

}



//void pluckString1(int servonum, int angle){
//  // Convert angle into pulse
//  int pulselength = map(angle, 0, 180, SERVOMIN, SERVOMAX);   
//  Serial.print(angle); Serial.print(" --> "); Serial.println(pulselength);
//
//  // play the string
//  pwm.setPWM(servonum, 0, pulselength);
//}
