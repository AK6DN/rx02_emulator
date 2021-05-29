//
// led_driver - LED driver routines
//
// Copyright (c) 2015-2021, Donald N North
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
// 24 Aug 2016 - donorth - Initial code
//



//
// includes
//
#include "my_project.h"
#include "led_driver.h"



// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE
// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE
// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE



//
// private data
//



// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC
// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC
// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC



//
// initialize the LED subsystem
//
void led_initialize (void)
{
    // configure pins
    pinMode(PIN_LEDY, OUTPUT);
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);

    // init all LEDs OFF
    led_state(red,    off);
    led_state(yellow, off);
    led_state(green,  off);
    led_state(base,   off);

    // done
    return;
}



//
// set LED color NNN to XXX
//
void led_state (LED_color color, LED_state state)
{
    if      (color == red)    digitalWrite(PIN_LED1, state == on ? HIGH : LOW);
    else if (color == yellow) digitalWrite(PIN_LED2, state == on ? HIGH : LOW);
    else if (color == green)  digitalWrite(PIN_LED3, state == on ? HIGH : LOW);
    else if (color == base)   digitalWrite(PIN_LEDY, state == on ? HIGH : LOW);

    return;
}



// the end
