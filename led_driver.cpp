//
// led_driver - LED driver routines
//
// (C) 2016 Don North <ak6dn_at_mindspring_dot_com>
//
// 24 Aug 2016 - donorth - Initial code
//



//
// includes
//
#include "my_project.h"
#include "led_driver.h"



// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE



//
// private data
//



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
