//
// local type definitions
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

#ifndef my_types_h
#define my_types_h



//
// constants
//
#ifndef TRUE
#define TRUE true
#else
#define true TRUE
#endif
#ifndef FALSE
#define FALSE false
#else
#define false FALSE
#endif
#ifndef NULL
#define NULL (0)
#endif
#ifndef null
#define null (0)
#endif



//
// very short delays
//
#define delay_1c(xxx) {asm("nop");}
#define delay_2c(xxx) {delay_1c();delay_1c();}
#define delay_3c(xxx) {delay_1c();delay_2c();}
#define delay_4c(xxx) {delay_2c();delay_2c();}
#define delay_5c(xxx) {delay_1c();delay_4c();}



//
// structured datatypes
//
typedef union { // either:
    struct { // four 8b bytes
	      uint8_t   ll;
	      uint8_t   lh;
	      uint8_t   hl;
	      uint8_t   hh;
    } b;
    struct { // two 16b words
	      uint16_t   l;
	      uint16_t   h;
    } w;
    uint32_t l; // or one 32b long
} uint32s_t;
//
typedef union { // either:
    struct { // two 8b bytes
	      uint8_t   l;
	      uint8_t   h;
    } b;
    uint16_t w; // or one 16b word
} uint16s_t;



#endif // my_types_h

// the end
