
void setupPins()
{
//  for(int i=0; i<sizeof(fretPins); i++)
//  {
//    Serial.print("Setting pin "); Serial.print(fretPins[i]); Serial.println(", output");
//    pinMode(fretPins[i], OUTPUT);
//  }
}


void convertTabToArray()
{
   Serial.print("Converting tab to array");
   Serial.println(tabString);


  char tabArray1[][6] = 
  {
    {'x','x','x','x','x','x'},
    {'0','x','x','x','x','x'},
    {'x','0','x','x','x','x'},
    {'x','x','0','x','x','x'},
    {'x','x','x','0','x','x'},
    {'x','x','x','x','0','x'},
    {'x','x','x','x','x','0'},
    {'x','x','x','x','x','x'},
    {'1','x','x','x','x','x'},
    {'x','1','x','x','x','x'},
    {'x','x','1','x','x','x'},
    {'x','x','x','1','x','x'},
    {'x','x','x','x','1','x'},
    {'x','x','x','x','x','1'},
    {'x','x','x','x','x','x'}
  };
  
}


/*
 * Get the length of the guitar tab
 */
int getTabLength()
{
  
}
