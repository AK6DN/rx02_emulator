//
// project definitions
//
// Copyright (c) 2015-2016, Donald N North
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// DONALD N NORTH BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Donald N North shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from Donald N North.
//

#ifndef my_project_h
#define my_project_h



//
// configuration
//
// use:  https://github.com/PaulStoffregen/Time  TimeLib.h if defined (uncomment next line)
// #define USE_TIMELIB_H
//
// use:  https://github.com/greiman/SdFat  SdFat.h if defined (uncomment next line)
#define USE_SDFAT_H



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
#ifdef USE_SDFAT_H
#include <SPI.h>
#include <SdFat.h>
#endif // USE_SDFAT_H
//
#ifdef USE_TIMELIB_H
#include <TimeLib.h>
#endif // USE_TIMELIB_H



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
