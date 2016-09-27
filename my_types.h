//
// local type definitions
//
// (C) 2016 Don North <ak6dn_at_mindspring_dot_com>
//

#ifndef my_types_h
#define my_types_h



//
// constants
//
#ifndef TRUE
#define TRUE true
#endif
#ifndef true
#define true TRUE
#endif
#ifndef FALSE
#define FALSE false
#endif
#ifndef false
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
