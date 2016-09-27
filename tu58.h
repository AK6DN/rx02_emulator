//
// tu58 - Emulate a TU58 over a serial line
//
// Original (C) 1984 Dan Ts'o <Rockefeller Univ. Dept. of Neurobiology>
// Update (C) 2005-2016 Don North <ak6dn_at_mindspring_dot_com>
//
// This is the TU58 emulation program written at Rockefeller Univ., Dept. of
// Neurobiology. We copyright (C) it and permit its use provided it is not
// sold to others. Originally written by Dan Ts'o circa 1984 or so.
//

#ifndef tu58_h
#define tu58_h



// TU58 Radial Serial Protocol

// Packet Flag / Single Byte Commands

#define TUF_NULL	0	  // null
#define	TUF_DATA	1	  // data packet
#define	TUF_CTRL	2	  // control packet
#define	TUF_INIT	4	  // initialize
#define TUF_BOOT	8	  // boot
#define	TUF_CONT	16	// continue
#define	TUF_XON		17	// flow control start (XON)
#define	TUF_XOFF	19	// flow control stop (XOFF)

// Opcodes

#define	TUO_NOP		    0	  // no operation
#define	TUO_INIT	    1	  // initialize
#define	TUO_READ	    2	  // read block
#define	TUO_WRITE	    3	  // write block
#define	TUO_SEEK	    5	  // seek to block
#define TUO_DIAGNOSE	7	  // run diagnostics
#define TUO_GETSTATUS	8	  // get status
#define TUO_SETSTATUS	9	  // set status
#define TUO_GETCHAR	  10	// get characteristics
#define	TUO_END		    64	// end packet

// Modifiers

#define TUM_RDRS    1	  // read with reduced sensitivity
#define TUM_WRRV    1	  // write with read verify
#define TUM_B128	128	  // special addressing mode

// Switches

#define TUS_MRSP	8	    // modified RSP sync mode
#define TUS_MAIN	16	  // maintenance mode

// End packet success codes

#define TUE_SUCC	 0	  // success
#define TUE_SUCR	 1	  // success with retry
#define TUE_FAIL 	-1	  // failed self test
#define TUE_PARO 	-2	  // partial operation
#define TUE_BADU 	-8	  // bad unit
#define TUE_BADF	-9	  // no cartridge
#define TUE_WPRO	-11	  // write protected
#define TUE_DERR	-17	  // data check error
#define TUE_SKRR	-32	  // seek error
#define TUE_MTRS	-33	  // motor stopped
#define TUE_BADO	-48	  // bad op code
#define TUE_BADB	-55	  // bad block number
#define TUE_COMM	-127	// communications error

// lengths of packets

#define TU_CTRL_LEN	10	// size of cmd packet (opcode..block bytes)
#define TU_DATA_LEN	128	// size of data transfer segment
#define TU_CHAR_LEN	24	// size of getchar data packet
#define TU_BOOT_LEN	512	// size of a boot block



// Packet format, cmd/end vs data

typedef struct {
    uint8_t	  flag;			// packet type
    uint8_t	  length;		// message length
    uint8_t	  opcode;		// operation code
    uint8_t	  modifier; // modifier
    uint8_t	  unit;		  // drive number
    uint8_t	  switches;	// switches
    uint16_t	sequence;	// sequence number, always zero
    uint16_t	count;		// byte count for read or write
    uint16_t	block;		// block number for read, write, or seek
    uint16_t	chksum;		// checksum, 16b end-around carry
} tu_cmdpkt_t;

typedef struct {
    uint8_t	flag;			          // packet type
    uint8_t	length;			        // message length
    uint8_t	data[TU_DATA_LEN];	// ptr to 1..DATALEN data bytes
    uint16_t	chksum;			      // checksum, 16b end-around carry
} tu_datpkt_t;

typedef union {	// either:
    tu_cmdpkt_t	cmd;		// a control packet
    tu_datpkt_t	dat;		// a data packet
} tu_packet_t;



#endif // tu58_h

// the end
