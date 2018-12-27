//
// tu58_driver - Simple TU58 driver
//
// Copyright (c) 2013-2016, Donald N North
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
// 21 Oct 2013 - donorth - Initial code
// 27 Mar 2016 - donorth- Ported to Arduino from CCS/Microchip
//



//
// includes
//
#include "my_project.h"
#include "tu58.h"
#include "tu58_driver.h"



// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE
// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE
// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE



//
// private data
//
static HardwareSerial *tu58Port = NULL; // emulator serial connection

static HardwareSerial *debugPort = NULL; // debug serial output

static uint8_t debugLevel = 0; // debug level, 0=none, higher is more

static uint8_t tu_buffer[sizeof(tu_datpkt_t)]; // shared data buffer



//
// send a buffer to the TU58
//
static int8_t tu_sndbuf (uint8_t *buffer, uint16_t count)
{
    // send the whole buffer
    return tu58Port->write(buffer, count) == count ? TUE_SUCC : TUE_FAIL;
}



//
// flush output to the TU58
//
static void tu_flush (void)
{
    // make sure all output sent
    tu58Port->flush();
    return;
}



//
// receive a char from the TU58
//
static uint8_t tu_rcvchr (void)
{
    uint32_t last;

    // return immediately if a character is available
    if (tu58Port->available() > 0) return tu58Port->read();

    // get current system time in milliseconds
    last = millis();
    while (TRUE) {
	      // wait until a character is available
	      if (tu58Port->available() > 0) return tu58Port->read();
        // return a null character if none returned within a few seconds
        if (millis()-last > 2000L) return TUF_NULL;
    }
}



//
// send a command packet to the TU58
//
static int8_t tu_sndcmd (tu_cmdpkt_t *cmdpkt)
{
    uint8_t *buffer = (uint8_t *)cmdpkt;
    uint32s_t chksum;
    uint8_t c;
    uint8_t count = sizeof(tu_cmdpkt_t);
    uint8_t cmd[sizeof(tu_cmdpkt_t)];
    uint8_t *ptr = cmd;

    // loop for all bytes except two chksum bytes
    chksum.l = 0;
    while (count-- > 2) {
	      // get even byte, add to chksum, send
	      c = *buffer++;
	      *ptr++ = c;
	      chksum.l += word(0,c);
	      if (count-- > 2) {
	          // get odd byte, add to chksum, send
	          c = *buffer++;
	          *ptr++ = c;
	          chksum.l += word(c,0);
	      }
	      // 16b end around carry
	      chksum.w.l += chksum.w.h;
	      chksum.w.h = 0;
    }
    // add chksum lo and hi
    *ptr++ = chksum.b.ll;
    *ptr++ = chksum.b.lh;

    // full buffer send
    return tu_sndbuf(cmd, sizeof(cmd));
}



//
// receive an end packet from the TU58
//
static int8_t tu_rcvend (uint8_t flag)
{
    uint32s_t chksum;
    uint32s_t rcvsum;
    uint8_t succ;
    uint8_t count;

    // if flag is NUL or CONT then ignore it
    while (flag == TUF_NULL || flag == TUF_CONT) flag = tu_rcvchr();
    // actual flag must be for control packet, else error
    if (flag != TUF_CTRL) return TUE_COMM+1;
    // next must be the packet length
    if ((flag = tu_rcvchr()) != TU_CTRL_LEN) return TUE_COMM+2;
    // next must be the END packet type
    if ((flag = tu_rcvchr()) != TUO_END) return TUE_COMM+3;
    // next will be the SUCCESS code
    succ = tu_rcvchr();
    // form the initial checksum
    chksum.l = word(TU_CTRL_LEN, TUF_CTRL) + word(succ, TUO_END);

    // read the remaining bytes in the control packet
    for (count = (TU_CTRL_LEN-2)/2; count > 0; --count) {
	      // even byte
	      chksum.l += word(0,tu_rcvchr());
	      // odd byte
	      chksum.l += word(tu_rcvchr(),0);
	      // 16b end around carry
	      chksum.w.l += chksum.w.h;
	      chksum.w.h = 0;
    }

    // now read the checksum from the TU58 and compare
    rcvsum.b.ll = tu_rcvchr();
    rcvsum.b.lh = tu_rcvchr();

    // either return success code if CHKSUM OK, else error
    return (rcvsum.w.l == chksum.w.l) ? succ : TUE_COMM+4;
}



// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC
// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC
// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC



//
// initialize the TU58 subsystem
//
void tu_initialize (uint32_t serialBaud, HardwareSerial *serialPort)
{
    uint16_t size;

    // setup which serial port to use
    tu58Port = serialPort;

    // tu58 emulator connection
    tu58Port->begin(serialBaud, SERIAL_8N2);

    // flush any output
    tu58Port->flush();

    // check size of serial buffers is correct
    if ((size = tu58Port->availableForWrite()) < 250) {
        if (debugLevel) debugPort->printf(F("TU58: Warning, tu58 serial buffer is too small at %d.; should be at least %d. bytes\n"), size, 250);
    }

    // and done
    return;
}



//
// setup the TU58 subsystem debug port
//
void tu_debug (HardwareSerial *serialPort, uint8_t level)
{
    // setup which serial port to use
    debugPort = serialPort;

    // and debug level
    debugLevel = level;

    // and done
    return;
}



//
// send an init sequence to the TU58
//
int8_t tu_init (void)
{
    uint8_t retry;
    uint8_t count;
    uint8_t c;
    static uint8_t init_init[] = { TUF_INIT, TUF_INIT };

    // wait for a CONT response w/retry
    for (retry = 100; --retry; ) {

	      // supposed to send a BREAK here ...
       	
	      // check that emulator is attached on the host side
	      if (TRUE) {

	          // send two INIT commands
	          if (debugLevel) debugPort->printf(F("TU58: tu_init sending <INIT><INIT>\n"));
	          tu_sndbuf(init_init, sizeof(init_init));
	          if (debugLevel) debugPort->printf(F("TU58: tu_init <INIT><INIT> sent!\n"));
	          tu_flush();

	          // wait for a CONT or INIT response w/retry
	          for (count = 250; --count; ) {
		            c = tu_rcvchr();
		            if (debugLevel) debugPort->printf(F("TU58: tu_init received 0x%02X\n"), c);
		            if (c == TUF_CONT) {
		                // successful
		                if (debugLevel) debugPort->printf(F("TU58: tu_init SUCCESS\n"));
		                return TUE_SUCC;
		            } else if (c == TUF_INIT) {
		                // back to outer loop
		                count = 1;
	              }
	          } // for (count...

	      } // if TRUE

    } // for (retry...

    // else we fail
    return TUE_COMM+6;
}



//
// send a DIAGNOSE command to the TU58
//
int8_t tu_diag (void)
{
    int8_t sts;
    tu_cmdpkt_t packet;

    // build a command packet and send it
    memset(&packet, 0x00, sizeof(packet));
    packet.flag = TUF_CTRL;
    packet.length = TU_CTRL_LEN;
    packet.opcode = TUO_DIAGNOSE;
    sts = tu_sndcmd(&packet);
    if (debugLevel) debugPort->printf(F("TU58: tu_diag status=%d\n"), sts);
    if (sts) return sts;

    // now wait for an end packet response
    return tu_rcvend(TUF_NULL);
}



//
// send a NOP command to the TU58
//
int8_t tu_nop (void)
{
    int8_t sts;
    tu_cmdpkt_t packet;

    // build a command packet and send it
    memset(&packet, 0x00, sizeof(packet));
    packet.flag = TUF_CTRL;
    packet.length = TU_CTRL_LEN;
    packet.opcode = TUO_NOP;
    sts = tu_sndcmd(&packet);
    if (debugLevel) debugPort->printf(F("TU58: tu_nop status=%d\n"), sts);
    if (sts) return sts;

    // now wait for an end packet response
    return tu_rcvend(TUF_NULL);
}



//
// send a SEEK command to the TU58
//
int8_t tu_seek (uint8_t unit, uint16_t blknum)
{
    int8_t sts;
    tu_cmdpkt_t packet;

    // build a command packet and send it
    memset(&packet, 0x00, sizeof(packet));
    packet.flag = TUF_CTRL;
    packet.length = TU_CTRL_LEN;
    packet.opcode = TUO_SEEK;
    packet.modifier = TUM_B128;
    packet.unit = unit;
    packet.block = blknum;
    sts = tu_sndcmd(&packet);
    if (debugLevel) debugPort->printf(F("TU58: tu_seek unit=%d block=%u status=%d\n"), unit, blknum, sts);
    if (sts) return sts;

    // now wait for an end packet response
    return tu_rcvend(TUF_NULL);
}



//
// send a READ command to the TU58
//
int8_t tu_read (uint8_t unit, uint16_t blknum, uint16_t count, uint8_t *buffer)
{
    int8_t sts;
    uint8_t cnt;
    uint8_t dat;
    uint32s_t chksum;
    uint32s_t rcvsum;
    tu_cmdpkt_t packet;

    // build a command packet and send it
    memset(&packet, 0x00, sizeof(packet));
    packet.flag = TUF_CTRL;
    packet.length = TU_CTRL_LEN;
    packet.opcode = TUO_READ;
    packet.modifier = TUM_B128;
    packet.unit = unit;
    packet.count = count;
    packet.block = blknum;
    sts = tu_sndcmd(&packet);
    if (debugLevel) debugPort->printf(F("TU58: tu_read unit=%d block=%u count=%u status=%d\n"), unit, blknum, count, sts);
    if (sts) return sts;

    // process data packets as they are received
    while (TRUE) {
	      if ((dat = tu_rcvchr()) == TUF_DATA) {
	          // next byte is length of the data
	          cnt = tu_rcvchr();
	          if (debugLevel) debugPort->printf(F("TU58: tu_read bytes=%u\n"), cnt);
	          // form the initial checksum
	          chksum.l = word(cnt, TUF_DATA);
	          // read the remaining bytes in the data packet
	          while (cnt > 0) {
		            // even byte
		            dat = tu_rcvchr();
		            chksum.l += word(0,dat);
		            if (count > 0) { *buffer++ = dat; count--; }
		            cnt--;
		            if (cnt > 0) {
		                // odd byte
		                dat = tu_rcvchr();
		                chksum.l += word(dat,0);
		                if (count > 0) { *buffer++ = dat; count--; }
		                cnt--;
		            }
		            // 16b end around carry
		            chksum.w.l += chksum.w.h;
		            chksum.w.h = 0;
	          }
	          // now read the checksum from the TU58 and compare
	          rcvsum.b.ll = tu_rcvchr();
	          rcvsum.b.lh = tu_rcvchr();
	          // validate checksum, return if error, else continue
	          if (rcvsum.w.l != chksum.w.l) return TUE_DERR;
	      } else if (dat == TUF_CONT) {
	          // ignore CONTs, just continue
	      } else {
	          // else process an end packet response
	          return tu_rcvend(dat);
	      }
    } // while TRUE

}



//
// send a WRITE command to the TU58
//
int8_t tu_write (uint8_t unit, uint16_t blknum, uint16_t count, uint8_t *buffer)
{
    int8_t sts;
    uint8_t cnt;
    uint8_t dat;
    uint8_t *ptr;
    uint32s_t chksum;
    tu_cmdpkt_t packet;

    // build a command packet and send it
    memset(&packet, 0x00, sizeof(packet));
    packet.flag = TUF_CTRL;
    packet.length = TU_CTRL_LEN;
    packet.opcode = TUO_WRITE;
    packet.modifier= TUM_B128;
    packet.unit = unit;
    packet.count = count;
    packet.block = blknum;
    sts = tu_sndcmd(&packet);
    if (debugLevel) debugPort->printf(F("TU58: tu_write unit=%d block=%u count=%u status=%d\n"), unit, blknum, count, sts);
    if (sts) return sts;

    // send data packets
    while ((dat = tu_rcvchr()) == TUF_CONT) {
	      // compute remaining number of bytes to send
	      cnt = count > TU_DATA_LEN ? TU_DATA_LEN : count;
	      count -= cnt;
	      if (debugLevel) debugPort->printf(F("TU58: tu_write bytes=%u\n"), cnt);
	      if (cnt == 0) return TUE_PARO;
	      // setup buffer
	      ptr = tu_buffer;
	      // first byte is data packet flag
	      *ptr++ = TUF_DATA;
	      // next byte is length of the data block                                       
	      *ptr++ = cnt;
	      // form the initial checksum
	      chksum.l = word(cnt, TUF_DATA);
	      // read the remaining bytes in the data packet
	      while (cnt > 0) {
	          // even byte
	          dat = *buffer++;
	          *ptr++ = dat;
	          chksum.l += word(0, dat);
	          cnt--;
	          if (cnt > 0) {
		            // odd byte
		            dat = *buffer++;
		            *ptr++ = dat;
		            chksum.l += word(dat, 0);
		            cnt--;
	          }
	          // 16b end around carry
	          chksum.w.l += chksum.w.h;
	          chksum.w.h = 0;
	      }
	      // add chksum lo and hi
	      *ptr++ = chksum.b.ll;
	      *ptr++ = chksum.b.lh;
	      // send data buffer
	      sts = tu_sndbuf(tu_buffer, ptr-tu_buffer);
	      if (sts) return sts;
    }

    // process an end packet response
    return tu_rcvend(dat);
}



// the end
