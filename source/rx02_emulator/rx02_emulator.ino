//
// RX8E/RX28/RX11/RX211/RXV11/RXV21 to TU58/serial or SDcard Interface
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
#define DEBUG_TU58 0

// program version id
#define VERSION  "v1.96"

// baud rate for USB serial debug port
//
//   250Kb is highest that AVR Mega2560/16U2 combo can run (ArduinoIDE 1.8.1 or earlier)
//   2000Kb is the highest that AVR Mega2560/16U2 combo can run (ArduinoIDE 1.8.2 or later)
//
//   All 'genuine' Arduino Mega2560s use this combo, so should work ok.
//
//   Mega2560 'clone' boards NOT using the 16U2 as a USB/Serial interface
//   may have issues running at this rate. Boards will have to be tested.
//   On some of these boards 230.4K or 115.2K may be the highest possible
//   rate possible. This will have an impact if using any 'debug' mode.
//
//   Depending upon the Mega2560 board, 2Mb or 1Mb may not even work 100%
//   reliably, so if enabling those modes you must test your Arduino board
//   to see if it performs as you expect (the L command is a good test).
//
#if ARDUINO >= 10802
// IDE 1.8.2 and later enable 500Kb, 1Mb, and 2Mb baud rates.
//#define SERIAL_BAUD_RATE 2000000L
//#define SERIAL_BAUD_RATE 1000000L
//#define SERIAL_BAUD_RATE  500000L
#define SERIAL_BAUD_RATE  250000L
#else
// IDE 1.8.1 and earlier 250Kb is the max baud rate.
#define SERIAL_BAUD_RATE  250000L
#endif



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
HardwareSerial *tty = &Serial; // optionally &Serial1 or &Serial2 for other UART connectors

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
    sd_list_files(tty);

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
    uint32_t size;
    static const char timing_mode[][9]  = { "Fastest", "Medium", "RealRX02" };
    static const char  debug_mode[][8]  = { "Off", "Low", "High", "Extreme" };
    static const char access_mode[][4]  = { "R/W", "R/O" };
    static const char  drive_mode[][10] = { "RX01+RX11", "RX01+RX8E", "RX02+any" };
    static const char ext_rx1[] = ".RX1";
    static const char ext_rx2[] = ".RX2";

    // get pointer to next char after 1st space (else NULL if no spaces)
    if ((arg = strchr(cmd, ' ')) != NULL) arg++;
    // set to NULL if arg is a zero length string (ie, line ended with a trailing space)
    if (*arg == 0) arg = NULL;

    // skip to a new line
    tty->printf(F("\n"));

    // parse the command
    switch (toupper(*cmd)) {

        // "0 FILENAME" set unit 0/1 file name
        case '0':
        case '1':
            i = *cmd-'0';
            if (arg) {
                size = sd_get_file_size(arg);
                if (sd_file_ext_matches(arg, ext_rx1) && size != rx_dsk_size(RX_DEN_SD)) {
                    size = sd_set_file_size(arg, rx_dsk_size(RX_DEN_SD), SD_POS_AT_END);
                    tty->printf(F("Updated file: '%s' to %lu. bytes\n"), arg, size);
                } else if (sd_file_ext_matches(arg, ext_rx2) && size != rx_dsk_size(RX_DEN_DD)) {
                    size = sd_set_file_size(arg, rx_dsk_size(RX_DEN_DD), SD_POS_AT_END);
                    tty->printf(F("Updated file: '%s' to %lu. bytes\n"), arg, size);
                }
                tty->printf(F("Setting file[%d]: '%s'\n"), i, rx_unit_file(i, arg));
                size = sd_get_file_size(arg);
                if (size != 0 && size != rx_dsk_size(RX_DEN_SD) && size != rx_dsk_size(RX_DEN_DD)) {
                    tty->printf(F("WARNING: file size not SD or DD ... use E/F command to correct!\n"));
                } else if (size == rx_dsk_size(RX_DEN_DD) && rx_emulation_type() != RX_TYPE_RX02) {
                    tty->printf(F("WARNING: file size DD mounted in mode RX01!\n"));
                }
            } else {
                tty->printf(F("Current file[%d]: '%s'\n"), i, rx_unit_file(i));
            }
            break;

        // "extend FILENAME" extend/create file name as single density sized (but not .RX2 files)
        case 'E':
            if (arg) {
                size = sd_get_file_size(arg);
                if (sd_file_ext_matches(arg, ext_rx2)) {
                    tty->printf(F("Unchanged file: '%s' is %lu. bytes (use F command)\n"), arg, size);
                } else if (size == rx_dsk_size(RX_DEN_SD)) {
                    tty->printf(F("Unchanged file: '%s' is %lu. bytes (SD)\n"), arg, size);
                } else if (size < rx_dsk_size(RX_DEN_SD)) {
                    size = sd_set_file_size(arg, rx_dsk_size(RX_DEN_SD), SD_POS_AT_END);
                    tty->printf(F("Extended file: '%s' to %lu. bytes (SD)\n"), arg, size);
                } else {
                    size = sd_set_file_size(arg, rx_dsk_size(RX_DEN_SD), SD_POS_AT_END);
                    tty->printf(F("Truncated file: '%s' to %lu. bytes (SD)\n"), arg, size);
                }
            }
            break;

        // "full FILENAME" extend/create file name as double density sized (but not .RX1 files)
        case 'F':
            if (arg) {
                size = sd_get_file_size(arg);
                if (sd_file_ext_matches(arg, ext_rx1)) {
                    tty->printf(F("Unchanged file: '%s' is %lu. bytes (use E command)\n"), arg, size);
                } else if (size == rx_dsk_size(RX_DEN_DD)) {
                    tty->printf(F("Unchanged file: '%s' is %lu. bytes (DD)\n"), arg, size);
                } else if (size < rx_dsk_size(RX_DEN_DD)) {
                    size = sd_set_file_size(arg, rx_dsk_size(RX_DEN_DD), SD_POS_AT_END);
                    tty->printf(F("Extended file: '%s' to %lu. bytes (DD)\n"), arg, size);
                } else {
                    size = sd_set_file_size(arg, rx_dsk_size(RX_DEN_DD), SD_POS_AT_END);
                    tty->printf(F("Truncated file: '%s' to %lu. bytes (DD)\n"), arg, size);
                }
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

        // "remove FILENAME" remove file name
        case 'R':
            if (arg && sd_remove_file(arg)) {
                tty->printf(F("Removed file: '%s'\n"), arg);
            } else {
                if (arg) tty->printf(F("No such file\n"));
            }
            break;

        // "show" show filename assignments
        case 'S':
            for (i = RX_UNIT_MIN; i <= RX_UNIT_MAX; ++i)
                tty->printf(F("Current file[%d]: '%s' (%s)\n"), i, rx_unit_file(i), access_mode[rx_unit_mode(i)]);
            break;

        // "list" files on the SD card
        case 'L':
            sd_list_files(tty);
            break;

        // "mode N" mode set emulation mode, 0/1/2 as RX01+RX11, RX01+RX8E, RX02+any
        case 'M':
            if (arg && isdigit(c = *arg)) {
                i = rx_emulation_type(c-'0');
                tty->printf(F("Setting emulation mode: %d (%s)\n"), i, drive_mode[i]);
            } else {
                i = rx_emulation_type();
                tty->printf(F("Current emulation type: %d (%s)\n"), i, drive_mode[i]);
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

        // "print" print emulation state
        case 'P':
            rx_print_state(tty);
            break;

        // "init" do an RX emulator initialize
        case 'I':
            tty->printf(F("Sending INIT ...\n"));
            rx_initialize(false);
            tty->printf(F("... INIT complete\n"));
            break;

        // "write" write the setup file from the current configuration
        case 'W':
            setup_write(setup_filename);
            break;

#ifdef USE_TIMELIB_H
        // "zap" setup the initial timestamp, as in: Z 2017-03-31 19:30:45
        case 'Z':
            if (arg) {
                uint16_t yr; uint8_t mo, da, hh, mm, ss;
                // scan a time string
                if (sscanf(arg, "%u-%hhu-%hhu %hhu:%hhu:%hhu", &yr, &mo, &da, &hh, &mm, &ss) == 6) {
                    if (1970 <= yr && yr <= 2099 && 01 <= mo && mo <= 12 && 01 <= da && da <= 31 &&
                        00 <= hh && hh <= 23 && 00 <= mm && mm <= 59 && 00 <= ss && ss <= 59) {
                        // initialize time
                        setTime(hh,mm,ss, da,mo,yr);
                        tty->printf(F("Date/time set!\n"));
                    } else {
                        tty->printf(F("Invalid date/time format!\n"));
                    }
                }
            }
            break;
#endif // USE_TIMELIB_H

        // "help" or "?"
        case 'H':
        case '?':
            // Note keep any single printf command string at 80 characters maximum
            tty->printf(F("\nCommands available:\n\n"));
            tty->printf(F("  0 FILENAME    -- set unit 0 file to FILENAME; default RX0.DSK\n"));
            tty->printf(F("                   file name 'none' (any case) for no disk present\n"));
            tty->printf(F("  1 FILENAME    -- set unit 1 file to FILENAME; default RX1.DSK\n"));
            tty->printf(F("                   file name 'none' (any case) for no disk present\n"));
            tty->printf(F("  y(es) N       -- set unit N file read-write (default)\n"));
            tty->printf(F("  n(o) N        -- set unit N file read-only\n"));
            tty->printf(F("  m(ode) N      -- set emulation mode, 0=RX01+RX11, 1=RX01+RX8E, "));
            tty->printf(F("2=RX02+any; default 2\n"));
            tty->printf(F("  d(ebug) N     -- debug level, 0=none, 3=max; default 1\n"));
            tty->printf(F("  t(iming) N    -- timing mode, 0=fast, 1=medium, 2=normal; default 0\n"));
            tty->printf(F("                   0 as fast as possible; 2 simulates real RX02 drive\n"));
            tty->printf(F("  r FILENAME    -- remove file FILENAME\n"));
            tty->printf(F("  e FILENAME    -- extend/truncate file FILENAME to single density size\n"));
            tty->printf(F("  f FILENAME    -- extend/truncate file FILENAME to double density size\n"));
            tty->printf(F("  l(ist)        -- list all files on the SD card\n"));
            tty->printf(F("  s(how)        -- show current unit filename assignments\n"));
            tty->printf(F("  p(rint)       -- print full emulation state\n"));
            tty->printf(F("  i(nit)        -- initialize emulator (like unibus INIT)\n"));
            tty->printf(F("  w(rite)       -- write current configuration into the SETUP.INI file\n"));
#ifdef USE_TIMELIB_H
            tty->printf(F("  z(ap) STAMP   -- set current timestamp for file access\n"));
            tty->printf(F("                   format is: YYYY-MO-DA HH:MM:SS\n"));
#endif // USE_TIMELIB_H
            tty->printf(F("  h(elp)        -- display this text\n"));
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
            cmd[i] = '\0';
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
    tty->begin(SERIAL_BAUD_RATE, SERIAL_8N2);

    // init LED drivers
    led_initialize();

    // say hello
    tty->printf(F("RX02 Emulator %s (IDE %u.%u.%u/gcc %s) - %s - %s\n"),
                VERSION,
                (ARDUINO/10000)%100, (ARDUINO/100)%100, (ARDUINO/1)%100,
                __VERSION__, __DATE__, __TIME__);
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
