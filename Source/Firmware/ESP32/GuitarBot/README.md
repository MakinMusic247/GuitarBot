# ESP IDF Project for Guitar Playing robot

## Overview
The Guitar bot uses a series of SG90 servo motors that are fixed above the frets and strings and can play notes. 
Servos are used on two different ways:
*   Bridge: pluck the guitar strings near the bridge
*   Frets: push down 1 of 2 frets

The fret servos are configured so that each servo hovers over 2 frets (each of a different string). The servos are coupled to two linear pushers that apply the force to fret the note. Note: only 1 fret can be played at a time per servo motor. 


## Code overview
The code contains 2 main parts:
*   main.c: the main program loop that receives the instructions for notes to play, initializes the hardware/peripherals and positions the servo motor 
*   components/servo: the library that handles servo control
