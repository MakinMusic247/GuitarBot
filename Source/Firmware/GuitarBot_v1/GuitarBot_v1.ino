/*
 * Sketch to convert a guitar tab string into an array and play song on guitar (via solenoids and servos)
 * 1. Select song (from string in code or sd card)
 * 2. Get length of song (how many notes) - based on tempo/time signature?
 * 3. Initialize array based on song length: char tabArray[song_length][6] // each line represents the 6 strings of the guitar. 'x' means don't pluck string, numbers refer to frets
 * 4. In loop (or ISR), loop through each array index
 *  a. convert char to int 
 *  b. match int to solenoid array: low E string, fret 1 == fretPins[0][1], D string, fret 3 == fretPins[2][3]
 */

char *tabArray1[] = { "------", "1-----",  "-1----", "--1---", "---1--",  "----1-", "-----1",  "------", "-----1", "----1-", "---1--", "--1---", "-1----", "1-----", "------" };

int tabLength = 0;
char* row ;

int pin1 = 8;
int pin2 = 9;
int pin3 = 10;
int fretPins[][6] { // in form [fret][string] == <pin number>
                      {22, 23, 24, 25, 26, 27}, // fret 1, strings 1->6
                      {28, 29, 30, 31, 32, 33}  // fret 2, strings 1->6
                  }; 
int stringPins[6] { 8, 9, 10, 11, 12, 13 }; // corresponds to string E, A, D, G, B, E

String tabString = 
  "|0-----------|-1-----------|\n\
   |--0---------|---1---------|\n\
   |----0-------|-----1-------|\n\
   |------0-----|-------1-----|\n\
   |--------0---|---------1---|\n\
   |----------0-|-----------1-|";

void setup() {
  Serial.begin(115200);
//  setupPins();
  tabLength = sizeof(tabArray1)/sizeof(tabArray1[0]); Serial.print("Tab length = "); Serial.println(tabLength);

  for(int i=0; i<6; i++)
  {
      pinMode(fretPins[0][i], OUTPUT);
      pinMode(fretPins[1][i], OUTPUT);
      pinMode(stringPins[i], OUTPUT);
  }

//  // Main loop, go through each time of tab
//  for(int i=0; i<tabLength; i++){
//    row=&(tabArray1[i][0]);
//    Serial.print("row "); Serial.print(i); Serial.print(" ");
//    play(tabArray1[i]);
//  }
}

void loop() {
    digitalWrite(8, HIGH); //digitalWrite(frets[i], HIGH);
    delay(1000);
    digitalWrite(8, LOW); //digitalWrite(frets[i], LOW);
    delay(1000);
    
//  for(int i=0; i<6; i++)
//  {
//      Serial.print("pin "); Serial.print(frets[i]); Serial.println(" HIGH");
//      digitalWrite(frets[i], HIGH);
//      delay(1000);
//      digitalWrite(frets[i], LOW);
//      delay(1000);
//  }
}

/* Play a single line of tab
 *  1. For each char in array, set the corresponding solenoids ('-' = off, <int> = on)
 *  2. Once all solenoids set, strum servo on corresponding strings
 *  
 */
void play(char notes[6])
{
  Serial.println(notes); // e.g. "1-----"
  // Loop through each string
  for(int i=0; i<6; i++){ // e.g. i=0, notes[i] = '1' --> E string, 1st fret --> fretPins[notes[i]][i]
    int fret = -1;
    int string = -1;
    switch (notes[i]) {
      case '0': // fret 0 - pluck string but don't fret 
        string = stringPins[i];
        break;
      case '1': // fret 1 - pluck string and fret 1
        string = stringPins[i];
        fret = fretPins[0][i]; // transistor array 0, string i
        digitalWrite(fret, HIGH);
        break;
      case '2': // fret 2 - pluck string and fret 1
        string = stringPins[i];
        fret = fretPins[1][i]; // transistor array 1, string i
        digitalWrite(fret, HIGH);
        break;
      default:
        break;
    }

    Serial.print("note "); Serial.print(notes[i]); Serial.print("|"); Serial.print(" fret "); Serial.print(fret); Serial.print(", string "); Serial.println(string);
    delay(1000);
    digitalWrite(fret, LOW);
  }
  
}
