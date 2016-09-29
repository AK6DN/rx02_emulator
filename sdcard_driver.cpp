//
// sdcard_driver - SD card Fat filesystem access routines
//
// (C) 2016 Don North <ak6dn_at_mindspring_dot_com>
//
// 24 Aug 2016 - donorth - Initial code
//



//
// includes
//
#include "my_project.h"
#include "sdcard_driver.h"



// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE
// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE
// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE



//
// private data
//
static HardwareSerial *debugPort = NULL; // debug serial output
static uint8_t debugLevel = 0; // debug level, 0=NONE, higher for more

static File sdfile; // SD card file data structure
static uint8_t initOk = FALSE; // set to TRUE on initialize OK



// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC
// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC
// PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC



//
// public data
//

SdFat sdcard; // SD card filesystem data structure



//
// initialize the SDcard subsystem
//
uint8_t sd_initialize (void)
{
    // configure pins
    pinMode(PIN_SD_CD, INPUT);
    pinMode(PIN_SD_MISO, INPUT);
    pinMode(PIN_SD_MOSI, OUTPUT);
    pinMode(PIN_SD_SCK, OUTPUT);
    pinMode(PIN_SD_CS_L, OUTPUT);

    // set default values
    digitalWrite(PIN_SD_MOSI, HIGH);
    digitalWrite(PIN_SD_SCK, HIGH);
    digitalWrite(PIN_SD_CS_L, HIGH);

    // start out not ok
    initOk = FALSE;

    // check for card detect
    if (!digitalRead(PIN_SD_CD)) return initOk;

    // init card, check for success
    if (!sdcard.begin(PIN_SD_CS_L, SPI_FULL_SPEED)) { sdcard.initErrorHalt(); return initOk; }

    // some card parameters
    uint8_t ctype = sdcard.card()->type();
    uint32_t csize = ((sdcard.card()->cardSize()/1024L)*512L)/1024L;
    if (debugLevel) {
        debugPort->printf(F("SD: cardType=SD%d\n"), ctype);
        debugPort->printf(F("SD: cardSize=%luMB\n"), csize);
    }

    // open the card - it should be FAT16 or FAT32
    if (!sdcard.vwd()->isOpen()) return initOk;

    // some volume parameters
    uint8_t vtype = sdcard.vol()->fatType();
    uint32_t vbpc = sdcard.vol()->blocksPerCluster()*512L;
    uint32_t vcss = sdcard.vol()->clusterSizeShift();
    uint32_t vucc = sdcard.vol()->clusterCount();
    uint32_t vsize = (vbpc/1024L)*(vucc/1024L);
    if (debugLevel) {
        debugPort->printf(F("SD: volType=FAT%d\n"), vtype);
        debugPort->printf(F("SD: volBytesPerCluster=%lu\n"), vbpc);
        debugPort->printf(F("SD: volClusterCount=%lu\n"), vucc);
        debugPort->printf(F("SD: volSizeBytes=%luMB\n"), vsize);
    }

    // success!
    initOk = TRUE;

    // list files
    sd_list_files(debugPort);

    // and done
    return initOk;
}



//
// setup the SDcard subsystem debug port
//
void sd_debug (HardwareSerial *serialPort, uint8_t level)
{
    // setup which serial port to use
    debugPort = serialPort;

    // and the debug level
    debugLevel = level;

    // and done
    return;
}



//
// list files on the SDcard
//
void sd_list_files (HardwareSerial *port)
{
    if (initOk) sdcard.ls(port, LS_DATE | LS_SIZE | LS_R);
    return;
}



//
// remove a file on the SDcard
//
void sd_remove_file (char *name)
{
    if (initOk) sdcard.remove(name);
    return;
}



//
// read bytes from an sdcard file
//
uint16_t sd_read_bytes (char *name, uint32_t pos, uint8_t *buf, uint16_t len)
{
    uint16_t num;

    // error if not setup
    if (!initOk) return 0;

    // close existing file, if open
    if (sdfile.isOpen()) sdfile.close();

    // open current file
    sdfile = sdcard.open(name, FILE_READ);

    // seek to required position
    if (sdfile.seek(pos) == false) { sdfile.close(); return 0; }

    // read requested number of bytes
    num = sdfile.read(buf, len);

    // close the file, done for now
    sdfile.close();

    // and return number of bytes read
    return num;
}



//
// write bytes to an sdcard file
//
uint16_t sd_write_bytes (char *name, uint32_t pos, uint8_t *buf, uint16_t len)
{
    uint16_t num;

    // error if not setup
    if (!initOk) return 0;

    // close existing file, if open
    if (sdfile.isOpen()) sdfile.close();

    // open current file
    sdfile = sdcard.open(name, FILE_WRITE);

    // seek to required position
    if (sdfile.seek(pos) == false) { sdfile.close(); return 0; }

    // write requested number of bytes
    num = sdfile.write(buf, len);

    // close the file, done for now
    sdfile.close();

    // and return number of bytes written
    return num;
}



//
// get size of an sdcard file
//
uint32_t sd_get_file_size (char *name)
{
    uint32_t size;

    // error if not setup
    if (!initOk) return 0;

    // close existing file, if open
    if (sdfile.isOpen()) sdfile.close();

    // open current file
    sdfile = sdcard.open(name, FILE_WRITE);

    // get file size
    size = sdfile.size();

    // close the file, done for now
    sdfile.close();

    // return the size
    return size;
}



//
// set size of an sdcard file AND fill with zeroes
//
uint32_t sd_set_file_size (char *name, uint32_t size)
{
    void    *buf;
    uint16_t len;
    uint32_t pos;

    // error if not setup
    if (!initOk) return 0;

    // close existing file, if open
    if (sdfile.isOpen()) sdfile.close();

    // get a buffer
    for (len = 1024; len > 0; len >>= 1) { if ((buf = malloc(len)) != NULL) break; }

    // fail if no memory
    if (len == 0 || buf == NULL) return 0;

    // zero the buffer
    memset(buf, 0x00, len);

    // open current file
    sdfile = sdcard.open(name, FILE_WRITE);

    // fill file with data
    for (pos = 0; pos < size; pos += len) { sdfile.write(buf, len); }

    // free buffer
    free(buf);

    // close the file, done for now
    sdfile.close();

    // truncate file if too large
    sdcard.truncate(name, size);

    // return the size
    return sd_get_file_size(name);
}



// the end
