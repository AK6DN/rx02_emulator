//
// RX8E/RX28/RX11/RX211/RXV11/RXV21 to TU58/serial or SDcard Interface
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
// 22-Aug-2015 donorth Initial version
//

//
// definitions
//
#define USE_RX02   1
#define TEST_RX02  0
#define DEBUG_RX02 1
//
#define USE_SD     1
#define TEST_SD    0
#define DEBUG_SD   1
//
#define USE_TU58   0
#define TEST_TU58  0
#define DEBUG_TU58 

#define VERSION  "v1.5"



//
// includes
//
#include "my_project.h"
#include "led_driver.h"
#include "tu58_driver.h"
#include "rx02_driver.h"
#include "sdcard_driver.h"



//
// global variables
//
HardwareSerial *tty = &Serial;

char setup_filename[] = "SETUP.INI";

#if TEST_TU58
uint8_t c_run;
uint8_t c_nop;
uint8_t c_init;
uint8_t c_diag;
uint8_t c_seek;
uint8_t c_read;
uint8_t c_write;
//
uint16_t number;
uint16_t block;
uint16_t value;
//
uint8_t tmp_buffer[128];
#endif // TEST_TU58



#if TEST_SD
//
// SD card file access test
//
void sd_test (void)
{
    uint8_t wrdata[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    uint8_t wrorder[] = { 2, 1, 3, 0, 5, 4 };
    uint8_t rddata[16];
    uint8_t rdorder[] = { 5, 4, 3, 2, 1, 0 };
    char name[] = "TEST1.DSK";
    uint32_t pos;
    uint16_t num;
    uint16_t len;
    uint16_t cnt;
    uint8_t  i;

    tty->printf("\n========================================\n");

    tty->printf("\nSDtest removing test file: %s\n", name);
    sd_remove_file(name);
    sd_list_files();

    pos = sizeof(wrorder)*sizeof(wrdata);
    tty->printf("\nSDtest setting file size to %lu.\n", pos);
    tty->printf("  file size is %lu. bytes\n", sd_set_file_size(name, pos, SD_POS_AT_BEGIN));

    for (cnt = 0; cnt < sizeof(wrorder); ++cnt) {
        len = sizeof(wrdata);
        pos = len*wrorder[cnt];
        tty->printf("\nSDtest (%d) writing test file, %u. bytes at offset %lu.\n", cnt, len, pos);
        for (i = 0; i < sizeof(wrdata); ++i) { wrdata[i] = (wrdata[i] & 0xF) | (wrorder[cnt]<<4); }
        num = sd_write_bytes(name, pos, wrdata, len);
        tty->printf("  wrote %u. bytes\n", num);
        tty->printf("  file size is %lu. bytes\n", sd_get_file_size(name));
    }

    tty->printf("\nSDtest getting file size\n");
    pos = sd_get_file_size(name);
    tty->printf("  file size is %lu. bytes\n", pos);

    for (cnt = 0; cnt < sizeof(rdorder); ++cnt) {
        len = sizeof(rddata);
        pos = len*rdorder[cnt];
        tty->printf("\nSDtest (%d) reading test file, %u. bytes at offset %lu.\n", cnt, len, pos);
        num = sd_read_bytes(name, pos, rddata, len);
        tty->printf("  read %u. bytes:\n ", num);
        for (i = 0; i < num; ++i) { tty->printf(" %02X", rddata[i]); }
        tty->printf("\n");
    }

    tty->printf("\n========================================\n\n");
    return;
}
#endif // TEST_SD



//
// run user command
//
void run_command (char *cmd)
{
    char c;
    uint8_t i, j;
    char *arg;
    static const char timing_mode[][9] = { "Fastest", "Medium", "RealRX02" };
    static const char  debug_mode[][8] = { "Off", "Low", "High", "Extreme" };
    static const char access_mode[][4] = { "R/W", "R/O" };

    // get pointer to next char after 1st space (else NULL if no spaces)
    if ((arg = strchr(cmd, ' ')) != NULL) arg++;
    // set to NULL if arg is a zero length string (ie, line ended with a trailing space)
    if (*arg == 0) arg = NULL;

    // skip to a new line
    tty->printf(F("\n"));

    // parse the command
    switch (toupper(*cmd)) {

        // "0 filename.txt" set unit 0/1 file name
        case '0':
        case '1':
            i = *cmd-'0';
            if (arg) {
                tty->printf(F("Setting file[%d]: '%s'\n"), i, rx_unit_file(i, arg));
            } else {
                tty->printf(F("Current file[%d]: '%s'\n"), i, rx_unit_file(i));
            }
            break;

        //  "yes N" set unit N as read-write mode
        case 'Y':
            if (arg && isdigit(c = *arg)) {
                i = c-'0';
                j = rx_unit_mode(i, RX_FILE_READ_WRITE);
                tty->printf(F("Setting file[%d] mode: %s\n"), i, access_mode[j]);
            }
            break;

        //  "no N" set unit N as read-only mode
        case 'N':
            if (arg && isdigit(c = *arg)) {
                i = c-'0';
                j = rx_unit_mode(i, RX_FILE_READ_ONLY);
                tty->printf(F("Setting file[%d] mode: %s\n"), i, access_mode[j]);
            }
            break;

        //  "mode N" set emulation mode, 1/2/3 as RX01/RX02/RX03
        case 'M':
            if (arg && isdigit(c = *arg)) {
                i = rx_emulation_type(c-'0');
                tty->printf(F("Setting emulation mode: %d (RX0%d)\n"), i, i);
            } else {
                i = rx_emulation_type();
                tty->printf(F("Current emulation type: %d (RX0%d)\n"), i, i);
            }
            break;

        // "debug N" debug level set, 0/1/2/3
        case 'D':
            if (arg && isdigit(c = *arg)) {
                i = rx_debug(tty, c-'0');
                tty->printf(F("Setting debug mode: %d (%s)\n"), i, debug_mode[i]);
            } else {
                i = rx_debug();
                tty->printf(F("Current debug mode: %d (%s)\n"), i, debug_mode[i]);
            }
            break;

        // "timing N" timing mode set, 0/1/2 as fast/medium/slow
        case 'T':
            if (arg && isdigit(c = *arg)) {
                i = rx_timing_type(c-'0');
                tty->printf(F("Setting timing mode: %d (%s)\n"), i, timing_mode[i]);
            } else {
                i = rx_timing_type();
                tty->printf(F("Current timing mode: %d (%s)\n"), i, timing_mode[i]);
            }
            break;

        // "show" show filename assignments
        case 'S':
            for (i = RX_UNIT_MIN; i <= RX_UNIT_MAX; ++i)
                tty->printf(F("Current file[%d]: '%s' (%s)\n"), i, rx_unit_file(i), access_mode[rx_unit_mode(i)]);
            break;

        // "print" print emulation state
        case 'P':
            rx_print_state(tty);
            break;

        // "list" files on the SD card
        case 'L':
            sd_list_files(tty);
            break;

        // "init" do an RX emulator initialize
        case 'I':
            tty->printf(F("Sending INIT ...\n"));
            rx_initialize(false);
            tty->printf(F("... INIT complete\n"));
            break;

        // "write" the setup file from the current configuration
        case 'W':
            setup_write(setup_filename);
            break;

        // "help" or "?"
        case 'H':
        case '?':
            // Note keep any single printf command string at 80 characters maximum
            tty->printf(F("\nCommands available:\n\n"));
            tty->printf(F("  0 filename.dsk    -- set unit 0 file name; default RX0.DSK\n"));
            tty->printf(F("                       filename 'none' (any case) for no disk present\n"));
            tty->printf(F("  1 filename.dsk    -- set unit 1 file name; default RX1.DSK\n"));
            tty->printf(F("                       filename 'none' (any case) for no disk present\n"));
            tty->printf(F("  y(es) N           -- set unit N file read-write (default)\n"));
            tty->printf(F("  n(o) N            -- set unit N file read-only\n"));
            tty->printf(F("  m(ode) N          -- set emulation mode, 0=NONE, n=RX0n; default 2\n"));
            tty->printf(F("  d(ebug) N         -- debug level, 0=none, 3=max; default 1\n"));
            tty->printf(F("  t(iming) N        -- timing mode, 0=fast, 1=medium, 2=normal; default 0\n"));
            tty->printf(F("                       0 as fast as possible; 2 simulates real RX02 drive\n"));
            tty->printf(F("  l(ist)            -- list all files on the SD card\n"));
            tty->printf(F("  s(how)            -- show current unit filename assignments\n"));
            tty->printf(F("  p(rint)           -- print full emulation state\n"));
            tty->printf(F("  i(nit)            -- initialize emulator (like unibus INIT)\n"));
            tty->printf(F("  w(rite)           -- write current configuration into the SETUP.INI file\n"));
            tty->printf(F("  h(elp)            -- display this text\n"));
            tty->printf(F("\nNote: chars in () are optional. Case does not matter.\n"));
            break;

        // unknown command
        default:
            tty->printf(F("?Unknown command?\n"));
            break;

    }

    return;
}



//
// process user input
//
void run_user (char c)
{
    static char cmd[40];
    static uint8_t i = 0;

    // echo non-control characters
    if (!isControl(c)) tty->write(c);

    // dispatch on index
    if (i == 0) {
        // start a command if printing, save
        if (isgraph(c)) cmd[i++] = c;
    } else if (iscntrl(c)) {
        // process control character
        if (c == 'U'-'@') {
            // ^U delete/ignore input
            tty->printf(F("^U\n"));
            i = 0;
        } else if (c == 'M'-'@' || c == 'J'-'@') {
            // CR or LF executes command
            cmd[i] = NULL;
            run_command(cmd);
            i = 0;
        }
    } else {
        // don't save multiple consecutive spaces
        if (isspace(c) && isspace(cmd[i-1])) {
            // skip consecutive spaces
        } else {
            // else just save next character if room
            if (i < sizeof(cmd)-2) cmd[i++] = c;
        }
    }

#if TEST_TU58
    if (isDigit(c))    { value = 10*value + (c - '0'); }
    else if (c == 'B') { tty->printf("BLOCK %u\n", value);  block = value;  value = 0; }
    else if (c == 'N') { tty->printf("NUMBER %u\n", value); number = value; value = 0; }
    else if (c == 'I') { tty->printf("INIT\n");  c_init = 1; }
    else if (c == 'R') { tty->printf("READ\n");  c_read = 1; }
    else if (c == 'W') { tty->printf("WRITE\n"); c_write = 1; }
    else if (c == 'C') { tty->printf("COPY\n");  c_read = c_write = 1; }
    else if (c == 'D') { tty->printf("DIAG\n");  c_diag = 1; }
    else if (c == 'P') { tty->printf("NOP\n");   c_nop = 1; }
    else if (c == 'S') { tty->printf("SEEK\n");  c_seek = 1; }
    else if (c == 'H') { tty->printf("HALT\n");  c_read = c_write = 0; }
#endif // TEST_TU58

    return;
}



#if TEST_TU58
//
// run tu58 commands
//
void run_tu58 (void)
{
    int8_t sts;

    if (true) {

        if (c_init) {
            sts = tu_init();
            if (DEBUG_TU58) tty->printf("init: status=%d\n", sts);
        }

        if (c_diag) {
            sts = tu_diag();
            if (DEBUG_TU58) tty->printf("diag: status=%d\n", sts);
        }

        if (c_nop) {
            sts = tu_nop();
            if (DEBUG_TU58) tty->printf("nop: status=%d\n", sts);
        }

        if (c_seek) {
            sts = tu_seek(0, block);
            if (DEBUG_TU58) tty->printf("seek:  unit=%d block=0x%04X status=%d\n", 0, block, sts);
        }

        if (c_read) {
            sts = tu_read(0, block, sizeof(tmp_buffer), tmp_buffer);
            if (DEBUG_TU58) {
                uint16_t i;
                tty->printf("read:  unit=%d block=0x%04X count=0x%04X status=%d\n", 0, block, sizeof(tmp_buffer), sts);
                for (i = 0; i < sizeof(tmp_buffer); ++i) {
                    if (i % 32 == 0) tty->printf("  ");
                    tty->printf(" %02X", tmp_buffer[i]);
                    if (i % 32 == 31) tty->printf("\n");
                }
            }
        }

        if (c_write) {
            sts = tu_write(1, block, sizeof(tmp_buffer), tmp_buffer);
            if (DEBUG_TU58) {
                uint16_t i;
                tty->printf("write: unit=%d block=0x%04X count=0x%04X status=%d\n", 1, block, sizeof(tmp_buffer), sts);
                for (i = 0; i < sizeof(tmp_buffer); ++i) {
                    if (i % 32 == 0) tty->printf("  ");
                    tty->printf(" %02X", tmp_buffer[i]);
                    if (i % 32 == 31) tty->printf("\n");
                }
            }
        }

        if (c_read || c_write || c_seek || c_init || c_diag || c_nop) {
            number -= 1;
            block += 1;
            if (number == 0) {
                c_read = c_write = c_seek = c_init = c_diag = c_nop = 0;
                number = 1;
            }
        }

    }

    return;
}
#endif // TEST_TU58



//
// write setup file from current configuration
//
void setup_write (char *name)
{
    File init;
    uint8_t i;

    // remove current file, if exists
    if (sdcard.exists(name)) sdcard.remove(name);

    // indicate generating file
    tty->printf(F("Generating setup file '%s'\n"), name);

    // open file and write config commands
    init = sdcard.open(name, FILE_WRITE);
    for (i = RX_UNIT_MIN; i <= RX_UNIT_MAX; ++i) {
        init.printf(F("%d %s\n"), i, rx_unit_file(i));
        init.printf(F("%c %d\n"), rx_unit_mode(i) == RX_FILE_READ_ONLY ? 'N' : 'Y', i);
    }
    init.printf(F("D %d\n"), rx_debug());
    init.printf(F("M %d\n"), rx_emulation_type());
    init.printf(F("T %d\n"), rx_timing_type());
    init.printf(F("S\n"));
    init.close();

    return;
}



//
// read setup file and set into current configuration
//
void setup_read (char *name)
{
    File init;

    // check file exists
    if (!sdcard.exists(name)) return;

    // indicate processing file
    tty->printf(F("Processing setup file '%s' ...\n"), name);

    // open file and read config commands
    init = sdcard.open(name, FILE_READ);
    while (init.available()) run_user(init.read());
    init.close();

    // and done
    tty->printf(F("... setup file processing complete!\n\n"));
    return;
}



//
// setup routine runs once at power up
//
void setup (void)
{
    // control/status input/output
    tty->begin(250000, SERIAL_8N2);

    // init LED drivers
    led_initialize();

    // say hello
    tty->printf(F("RX02 Emulator %s - %s - %s\n"), VERSION, __DATE__, __TIME__);
    delay(1000);

#if USE_SD
    // check for SD card
    if (DEBUG_SD) sd_debug(tty, DEBUG_SD);
    if (sd_initialize()) {
        tty->printf(F("Valid MicroSD card detected.\n"));
  #if TEST_SD
        sd_test();
  #endif // TEST_SD
    } else {
        tty->printf(F("No valid MicroSD card detected.\n"));
    }
#endif // USE_SD

#if TEST_TU58
    // init globals
    number = 1;
    block = 0;
    value = 0;
    c_write = 0;
    c_read = 0;
    c_seek = 0;
    c_diag = 0;
    c_init = 0;
    c_nop = 0;
#endif // TEST_TU58

#if USE_TU58
    // init tu58 interface
    if (DEBUG_TU58) tu_debug(tty, DEBUG_TU58);
    tu_initialize(1000000L, &Serial1);
#endif // USE_TU58

#if USE_RX02
    // init rx11/211/8e interface
    if (DEBUG_RX02) rx_debug(tty, DEBUG_RX02);
    rx_initialize(true);
#endif // USE_RX02

#if USE_SD
    // initial configuration file ?
    if (sdcard.exists(setup_filename)) {
        // exists, process it
        setup_read(setup_filename);
    } else {
        // does not exist, create it
        setup_write(setup_filename);
    }
#endif // USE_SD

    // done
    tty->printf(F("Initialization complete.\n"));
    return;
}



//
// loop routine runs continuously
//
void loop (void)
{
    // check if user typed a character
    if (tty->available()) run_user(tty->read());

#if USE_TU58
  #if TEST_TU58
    // tu58 commands
    run_tu58();
   #endif // TEST_TU58
#endif // USE_TU58

#if USE_RX02
    // process RX function
    rx_function();
#endif // USE_RX02

    // done
    return;
}



// the end
