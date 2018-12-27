//
// tu58_driver - Simple TU58 driver external interface
//
// (C) 2016 Don North <ak6dn_at_mindspring_dot_com>
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
