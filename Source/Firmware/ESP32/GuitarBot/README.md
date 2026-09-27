# ESP IDF Project for Guitar Playing robot

## Overview
The Guitar bot uses a series of SG90 servo motors that are fixed above the frets and strings and can play notes. 
Servos are used on two different ways:
*   Bridge: pluck the guitar strings near the bridge
*   Frets: push down 1 of 2 frets

The fret servos are configured so that each servo hovers over 2 frets (each of a different string). The servos are coupled to two linear pushers that apply the force to fret the note. Note: only 1 fret can be played at a time per servo motor. 


## Code overview
The code contains the main parts:
*   main.c: the main program loop that receives the instructions for notes to play, initializes the hardware/peripherals and positions the servo motor 
*   components/servo: the library that handles servo control
*   components/Waveshare (optional, remove if not needed): using a Waveshare ESP32-S3 touch screen device UI to control GuitarBot



## Resources
*   PCA9685 Servo motor driver:
    *   https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf
    *   https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
*   LVGL open source graphics library: https://lvgl.io/docs/open
*   Waveshare ESP32-S3 board: https://docs.waveshare.com/ESP32-S3-Touch-LCD-2.8
*   3D printed model CAD software: https://www.tinkercad.com/