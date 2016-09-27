//
// rx02_driver - Simple RX02 driver external interface
//
// (C) 2013 Don North <ak6dn_at_mindspring_dot_com>
//
// 27 Mar 2016 - donorth- Initial code
//

#ifndef rx02_driver_h
#define rx02_driver_h



//
// public prototypes
//
void    rx_initialize (uint8_t flag);
uint8_t rx_debug (HardwareSerial *serialPort, uint8_t level);
uint8_t rx_debug (void);
void    rx_print_state (HardwareSerial *serialPort);
char *  rx_unit_file (uint8_t unit, char *name);
char *  rx_unit_file (uint8_t unit);
uint8_t rx_emulation_type (uint8_t type);
uint8_t rx_emulation_type (void);
uint8_t rx_timing_type (uint8_t type);
uint8_t rx_timing_type (void);
void    rx_function (void);



#endif // rx02_driver_h

// the end
