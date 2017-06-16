/*
Follow-me controller for cleanflight iNav.
Updated to work with ESP8266 an Android App
I2C OLED screen library: https://github.com/olikraus/u8g2

Cleanflight connections
GND - GND
VCC - VCC
RX - TX
TX - RX
 
OLED connections (optional but extremely useful)
 GND - GND
 VCC - VCC
 SDA - Arduino pin A4
 SCL - Arduino pin A5
 
Radio heartbeat LED on pin 2 (optional but extremely useful)
 
 */

#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
//#include "printf.h" - NEED TO FIX THIS ONE
#include <SoftwareSerial.h>
//#include "U8glib.h" - depri
#include <U8g2lib.h>
//#include <nRF24L01.h>
//#include <RF24.h>
//#include "nRF24.h"
#include "webserver.h"
#include "cleanflight.h"
#include "rc.h"
#include "gimbal.h"
#include "msp.h"
#include "screen.h"

void setup() { 
  msp_setup();
  gimbal_setup();  
  radio_setup();
  rc_setup();
  screen_setup(); 
}

void loop() {
  msp_loop(); // calls rc_loop, sets gimbal output
  radio_loop();
  screen_loop();
}

