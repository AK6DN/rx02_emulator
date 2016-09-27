//
// project definitions
//
// (C) 2016 Don North <ak6dn_at_mindspring_dot_com>
//

#ifndef my_project_h
#define my_project_h



//
// includes
//
#include <Arduino.h>
#include <HardwareSerial.h>
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
//
#include <SPI.h>
#include <SdFat.h>



//
// local type definitions
#include "my_types.h"



//
// standard analog pins 0..5
//
// NONE



//
// standard digital pins 0.. 13,  extra mega digital pins 14..53
//
const byte PIN_CTLR_RUN_H      =  2; // PE4 input, interrupt
const byte PIN_CTLR_INIT_H     =  3; // PE5 input, interrupt
//
const byte PIN_CTLR_DONE_H     = 28; // PA6 output
const byte PIN_CTLR_ERROR_H    = 29; // PA7 output
const byte PIN_CTLR_12BIT_H    = 30; // PC7 input
const byte PIN_CTLR_TR_RQST_H  = 31; // PC6 output
const byte PIN_CTLR_DATAO_H    = 32; // PC5 output
const byte PIN_CTLR_DATAI_H    = 33; // PC4 input
const byte PIN_CTLR_OUT_H      = 34; // PC3 output
const byte PIN_CTLR_SHIFT_H    = 35; // PC2 output
const byte PIN_CTLR_ACLO_H     = 36; // PC1 output
const byte PIN_CTLR_DMA_MODE_L = 37; // PC0 input
//
const byte PIN_SD_CD           = 49; // card detect input, active high
const byte PIN_SD_MISO         = 50; // card MISO/DO input, active high
const byte PIN_SD_MOSI         = 51; // card MOSI/DI output, active high
const byte PIN_SD_SCK          = 52; // card SCK output, active high
const byte PIN_SD_CS_L         = 53; // chip select output, active low
//
const byte PIN_LED1            = 10; // output, LED1, active high
const byte PIN_LED2            = 11; // output, LED2, active high
const byte PIN_LED3            = 12; // output, LED3, active high
const byte PIN_LEDY            = 13; // output, onboard yellow LED, active high



#endif // my_project_h

// the end
