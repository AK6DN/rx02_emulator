//
// rx02_driver - Simple RX02 driver external interface
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
