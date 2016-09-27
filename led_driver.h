//
// led_driver - LED driver interface
//
// (C) 2016 Don North <ak6dn_at_mindspring_dot_com>
//
// 24 Aug 2016 - donorth - Initial code
//

#ifndef led_driver_h
#define led_driver_h



//
// public datatypes
//
enum LED_color { red, yellow, green, base };
enum LED_state { off, on };



//
// public prototypes
//
void led_initialize (void);
void led_state (LED_color color, LED_state state);



#endif // led_driver_h

// the end
