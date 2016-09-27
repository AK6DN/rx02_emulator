//
// tu58_driver - Simple TU58 driver external interface
//
// (C) 2016 Don North <ak6dn_at_mindspring_dot_com>
//
// 27 Mar 2016 - donorth - Initial code
//

#ifndef tu58_driver_h
#define tu58_driver_h



//
// public prototypes
//
void   tu_initialize (uint32_t serialBaud, HardwareSerial *serialPort);
void   tu_debug (HardwareSerial *serialPort, uint8_t level);
int8_t tu_nop (void);
int8_t tu_init (void);
int8_t tu_diag (void);
int8_t tu_seek (uint8_t unit, uint16_t blknum);
int8_t tu_read (uint8_t unit, uint16_t blknum, uint16_t count, uint8_t *buffer);
int8_t tu_write (uint8_t unit, uint16_t blknum, uint16_t count, uint8_t *buffer);



#endif // tu58_driver_h

// the end
