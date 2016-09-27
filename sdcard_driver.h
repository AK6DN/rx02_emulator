//
// sdcard_driver - SD card Fat filesystem access external interface
//
// (C) 2016 Don North <ak6dn_at_mindspring_dot_com>
//
// 24 Aug 2016 - donorth - Initial code
//

#ifndef sdcard_driver_h
#define sdcard_driver_h



//
// public prototypes
//
uint8_t  sd_initialize (void);
void     sd_debug (HardwareSerial *serialPort, uint8_t level);
void     sd_list_files (HardwareSerial *serialPort);
void     sd_remove_file (char *name);
uint16_t sd_read_bytes (char *name, uint32_t pos, uint8_t *buf, uint16_t len);
uint16_t sd_write_bytes (char *name, uint32_t pos, uint8_t *buf, uint16_t len);
uint32_t sd_get_file_size (char *name);
uint32_t sd_set_file_size (char *name, uint32_t size);



#endif // sdcard_driver_h

// the end
