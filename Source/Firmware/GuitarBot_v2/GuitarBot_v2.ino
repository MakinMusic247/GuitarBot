/*
 * Sketch to convert a guitar tab string into an array and play song on guitar (via solenoids and servos)
 * 1. Select song (from string in code or sd card)
 * 2. Get length of song (how many notes) and note period (how long between notes) - based on tempo/time signature?
 * 3. Initialize 'tab' array // each line represents the 6 strings of the guitar. -1 means don't pluck string, numbers refer to frets
 * 4. In loop (or ISR), loop through each array index
 *  a. get the corresponding string and fret pins 
 *  b. match int to solenoid array: low E string, fret 1 == fretPins[0][1], D string, fret 3 == fretPins[2][3]
 *  c. Press down on frets and get list of strings to play
 *  d. pluck strings
 */

int MAX_FRET_NUMBER = 2; // highest fret that has solenoid connected
int tabLength = 0;
int period = 1000; // milliseconds to delay between each note/s
unsigned long time_now = 0; // hold the millisecond delay

/* Digital pins outputting to fretboard circuit */
int fretPins[][6] { // in form [fret][string] == <digital pin number>
                      { 0,  0,  0,  0,  0,  0}, // open string, ignore 
                      {22, 23, 24, 25, 26, 27}, // fret 1, strings 1->6
                      {28, 29, 30, 31, 32, 33}  // fret 2, strings 1->6
                  }; 

/* Digital pins outputting to fretboard circuit */
int stringPins[6] { 8, 9, 10, 11, 12, 13 }; // corresponds to string E, A, D, G, B, E

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
  Serial.begin(9600); // 115200
  // get the number of entries of the tab
  tabLength = sizeof(tab)/sizeof(tab[0]); Serial.print("Tab length = "); Serial.println(tabLength);

  // setup digital pins to OUTPUT mode
  for(int i=0; i<6; i++)
  {
      pinMode(fretPins[0][i], OUTPUT); // fret 1, all strings
      pinMode(fretPins[1][i], OUTPUT); // fret 2, all strings
      pinMode(stringPins[i], OUTPUT);  // open strings
  }

  delay(2000); // wait for 2 seconds before beginning

  // Loop through each note/chord of tab and activate relevant solenoids/motors
  for(int i=0; i<tabLength; i++){
    Serial.print("phrase "); Serial.print(i); Serial.println(" ");
    play(tab[i]);
    delay(period);
  }

}


/* 
 *  Play a single line of tab
 *  1. For each int in array, set the corresponding solenoids (-1 = off, <int> = on)
 *  2. Once all solenoids set, strum servo on corresponding strings
 */
void play(int notes[6]){

  /* 1. Push solenoid down on corresponding fret for each string required */
  // loop through note played on each string (E->A->D->G->B->E) and activate corresponding solenoid
  for(int i=0; i<6; i++){
    int fret = -1;
    int string = -1;

    if(notes[i]>0 && notes[i] <= MAX_FRET_NUMBER){ // push down on fret
      fret = fretPins[notes[i]][i]; // E.g [2][6] => [<fret 1>][<high E string>]
      digitalWrite(fret, HIGH);
    }
    else{ } // note is 0, -1 or > max fret number allowed, do nothing

    Serial.print("note "); Serial.print(notes[i]); Serial.print("|"); Serial.print(" fret "); Serial.print(fret); Serial.print(", string "); Serial.println(i);
    delay(10);

    digitalWrite(fret, LOW);
  }

  
  /* 2. Pluck required strings */
  // If note is 0 --> MAX_FRET_NUMBER, activate the corresponding servo to pluck the string
  for(int i=0; i<6; i++){
    int string = -1;
    
    if(notes[i]>=0 && notes[i] <= MAX_FRET_NUMBER){
      string = stringPins[i];
      // TODO: move servo to opposite position
    }
  }

  /* 3. Reset frets/strings to original position */

}


// reset solenoids/servos to original position
void reset(){
  
}


void loop() {
//    digitalWrite(26, HIGH); //digitalWrite(frets[i], HIGH);
//    delay(1000);
//    digitalWrite(26, LOW); //digitalWrite(frets[i], LOW);
//    delay(1000);
}




/*
 * TODOS:
 * 1. include servo library nd driver shield (I2C addressed?)
 * 2. track position of each servo (will be either above or below string depending on number of notes played). Work out whether to pluck up or down
 *    - need array to store each servos current position?
 * 3. 
 */
