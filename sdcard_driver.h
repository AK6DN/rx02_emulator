//
// sdcard_driver - SD card Fat filesystem access external interface
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
// 24 Aug 2016 - donorth - Initial code
//

#ifndef sdcard_driver_h
#define sdcard_driver_h



//
// public constants
//

// positioning for set file size
//
#define SD_POS_AT_BEGIN 0
#define SD_POS_AT_END   1



//
// public data
//

extern SdFat sdcard; // SDcard access state



//
// public prototypes
//
uint8_t  sd_initialize (void);
void     sd_debug (HardwareSerial *serialPort, uint8_t level);
void     sd_list_files (HardwareSerial *serialPort);
uint8_t  sd_remove_file (char *name);
uint16_t sd_read_bytes (char *name, uint32_t pos, uint8_t *buf, uint16_t len);
uint16_t sd_write_bytes (char *name, uint32_t pos, uint8_t *buf, uint16_t len);
uint32_t sd_get_file_size (char *name);
uint32_t sd_set_file_size (char *name, uint32_t size, uint8_t mode);



#endif // sdcard_driver_h

// the end
