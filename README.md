# Overview #

`rx02_emulator` is a hardware/software emulation of a DEC RX02 (or RX01) dual 8" floppy disk drive. The software runs on an Arduino Mega2560 processor board with a custom hardware interface shield that maps a dozen or so digital port bits to the custom DEC RX drive interface protocol.

The emulator simulates two RX02 drives mapped to files on an attached MicroSD card.

This implementation was originally based on the design at:  http://www.chdickman.com/rx02/ but has been extensively modified for the Arduino and to provide the functionality necessary to pass the DEC RX02 hardware diagnostics.

Currently this design boots/runs under the XXDP and RT-11 operating systems, and passes the DEC RX02 hardware diagnostics ZRXDC0 Performance Exerciser, ZRXEA0 Formatter, and ZRXFB0 RX02/RX211 Logic/Function tests error free.

Testing to date has been performed as an RX02/RX211 interface combo in a UNIBUS PDP-11/44, and in a PDP-8m with an RX8E/RX28 interface so RX01/RX8E and RX02/RX28 configurations can be tested. I do not have access to RX11, RXV11, or RXV21 boards for testing. Limited user testing is ongoing on these other configurations.

The MicroSD card in the emulator is a FAT32 formatted filesystem, and it can be inserted (offline) into a WindowsPC to copy files to/from the device. By default, the files 'RX0.DSK' and 'RX1.DSK' are mapped to drive 0 and 1 on initialization.

Detection of single density media (256,256 bytes total) and double density media (512,512 bytes total) is done thru two differrent mechanisms. Files with the extension .RX1 will be forced to be single density, and resized appropriately (zero padded at the end, and truncated to size if required). Similarly files with the extension .RX2 will be forced to double density, and resized if necessary. Files with any other extension (ie, like .DSK) will be detected as single density if they are EXACTLY 256,256 bytes in size; and detected as double density if they are EXACTLY 512,512 bytes in size. Files that are other sizes will NOT be capable of being mounted until they are resized appropriately (the `E` and `F` commands can do this).

The emulator interfaces thru a simple ASCII terminal command line via the USB port on the Arduino device. After booting, typing: `H<cr>` types out the available commands. Below is a sample interaction showing the use of the `H`, `L`, `S` and `0` commands, and the debug level 1 log of what happens when `b dy0` is typed on the attached PDP-11/44 console terminal.

Startup configuration is saved in an ASCII text file SETUP.INI that contains user interface commands that are replayed on startup. The SETUP.INI file is written using the `W` command, and the current 0/1/Y/N/D/M/T options are saved in the file.

The hardware shield has three indicator LEDs:
<OL>
<LI>(RED) ON when INIT is asserted from the host or an error is detected in the emulator. OFF at command start.</LI>
<LI>(YEL) ON when the emulator is actively accessing the microSD card storage. OFF otherwise.</LI>
<LI>(GRN) ON when a command is inititated from the host, OFF when the command has completed (DONE set).</LI>
</OL>

Normal operation will see the GRN/YEL LEDs blinking rapidly or mostly ON. For non-storage commands (eg, buffer fill/empty) only GRN will be ON.

Hardware design files (schematics, PCB layout, BOM) are available in the documentation directory. Includes full schematic, PCB layout plots, and all gerbers.

The Arduino controller software has been moved into the source directory.

## Example ##

Sample boot log in the Arduino USB serial monitor window:
```
RX02 Emulator v1.9 (IDE 1.8.5/gcc 4.9.2) - Oct 31 2017 - 12:34:56

SD: cardType=SD3
SD: cardSize=3781MB
SD: volType=FAT32
SD: volBytesPerCluster=4096
SD: volClusterCount=964992
SD: volSizeBytes=3768MB

2016-09-24 18:02:14     512512 RX0.DSK
2016-09-24 18:02:14     512512 RX1.DSK
2016-09-24 18:36:02     512512 XXDP.DSK
2016-09-26 15:37:38     512512 RT11.DSK
2016-09-24 17:02:12         50 SETUP.INI

Valid MicroSD card detected.

RX: waiting for INIT to clear ... t=1132ms
RX: INIT has cleared t=1206ms
RX: INIT rx_xmit_es(0244)

Processing setup file 'SETUP.INI' ...
0 RX0.DSK
Setting file[0]: 'RX0.DSK'
Y 0
Setting file[0] mode: R/W
1 RX1.DSK
Setting file[1]: 'RX1.DSK'
Y 1
Setting file[1] mode: R/W
D 1
Setting debug mode: 1 (Low)
M 2
Setting emulation mode: 2 (RX02)
T 0
Setting timing mode: 0 (Fastest)
S
Current file[0]: 'RX0.DSK' (R/W)
Current file[1]: 'RX1.DSK' (R/W)
... setup file processing complete!

Initialization complete.
H

Commands available:

  0 FILENAME    -- set unit 0 file to FILENAME; default RX0.DSK
                   file name 'none' (any case) for no disk present
  1 FILENAME    -- set unit 1 file to FILENAME; default RX1.DSK
                   file name 'none' (any case) for no disk present
  y(es) N       -- set unit N file read-write (default)
  n(o) N        -- set unit N file read-only
  m(ode) N      -- set emulation mode, 0=NONE, n=RX0n; default 2
  d(ebug) N     -- debug level, 0=none, 3=max; default 1
  t(iming) N    -- timing mode, 0=fast, 1=medium, 2=normal; default 0
                   0 as fast as possible; 2 simulates real RX02 drive
  r FILENAME    -- remove file FILENAME
  e FILENAME    -- extend/truncate file FILENAME to single density size
  f FILENAME    -- extend/truncate file FILENAME to double density size
  l(ist)        -- list all files on the SD card
  s(how)        -- show current unit filename assignments
  p(rint)       -- print full emulation state
  i(nit)        -- initialize emulator (like unibus INIT)
  w(rite)       -- write current configuration into the SETUP.INI file
  h(elp)        -- display this text

Note: chars in () are optional. Case does not matter.

S
Current file[0]: 'RX0.DSK' (R/W)
Current file[1]: 'RX1.DSK' (R/W)
0 XXDP.DSK
Setting file[0]: 'XXDP.DSK'
W
Generating setup file 'SETUP.INI'

RX: waiting for INIT to clear ... t=49342ms
RX: INIT has cleared t=49438ms
RX: INIT rx_xmit_es(0244)

RX: cmd=0007
RX: RDSECT unit=0 den=S
RX: RDSECT sa=001
RX: RDSECT ta=001
RX: RDSECT rx_xmit_es(0060)

RX: waiting for INIT to clear ... t=49462ms
RX: INIT has cleared t=49557ms
RX: INIT rx_xmit_es(0244)

RX: cmd=0407
RX: RDSECT unit=0 den=D
RX: RDSECT sa=001
RX: RDSECT ta=001
RX: RDSECT pos=6656. len=256.
RX: RDSECT rx_xmit_es(0040)

RX: cmd=0403
RX: EMPBUF unit=0 den=D
RX: EMPBUF wc=200
RX: EMPBUF rx_xmit_es(0000)

RX: cmd=0407
RX: RDSECT unit=0 den=D
RX: RDSECT sa=003
RX: RDSECT ta=001
RX: RDSECT pos=7168. len=256.
RX: RDSECT rx_xmit_es(0040)

RX: cmd=0403
RX: EMPBUF unit=0 den=D
RX: EMPBUF wc=200
RX: EMPBUF rx_xmit_es(0000)

...
```

## Notes ##

(1) This code has been written with the assumption that `Xprintf` support has been added to the PRINT class in the Arduino development environment.

(2) This code uses the SDfat library available at:  https://github.com/greiman/SdFat

(3) This code was written with tap stops set at 4 (versus the default of 2). Manually edit the Arduino <B>preferences.txt</B> file tab size line to be: <B>editor.tabs.size=4</B> if desired.

(4) Right now there is some 'extraneous' code (ie, the TU58 driver interface) that is included by default but not currently used. A future plan is to add support to map a backend TU58 server to a file connection (ie, by using a pseudo filename) so that not only local MicroSD card file access can be supported, but simultaenous access to a backend PC-based file storage server can happen.

# INSTALLATION #

The following sections detail the installation and configuration of the Arduino software environment, getting the `rx02_emulator` code, compiling it, and downloading it to a target board.

## Arduino IDE ##

If you don't have the Arduino development environment installed on your system, the first step is to download and install it. Go to:

https://www.arduino.cc/

and click on the `Software` tab to get to the download page.

Scroll down to the `Download the Arduino IDE` section and download the installer appropriate for your system.

Run the install.

## SD FAT Library ##

The emulator software requires a third party SD card FAT filesystem access library. Go to:

https://github.com/greiman/SdFat

On the right hand side of the page there is a `Clone or download` green button; click it and select `Download ZIP` and save the file. It will likely be named `SdFat-master.zip`.

Unzip the contents of `SdFat-master.zip` into a folder. There should be a folder inside the top level folder named `SdFat`.

Move the folder `SdFat` into your Arduino local library folder. On Windows, this would be into the folder:

```
C:\Users\yourname\My Documents\Arduino\libraries
```

which probably just contains a `readme.txt` file. The `SdFat` library is now installed and ready for use.

## PRINTF Configuration ##

The emulator code is written assuming that the printf/sprintf methods have been added to the serial I/O and file access routines. The default Arduino install does not have this support so it needs to be configured.

The following page:

http://playground.arduino.cc/Main/Printf

describes several ways to do this; the preferred method I suggest is described under `Adding printf() to Print class`.

It is not difficult, but does require editing a standard Arduino system file and adding some code.

On windows, the target file to be modified is:

```
C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Print.h
```

the following code:

```
#include <stdarg.h>
#define PRINTF_BUF 80 // define the tmp buffer size (change if desired)
   void printf(const char *format, ...)
   {
   char buf[PRINTF_BUF];
   va_list ap;
        va_start(ap, format);
        vsnprintf(buf, sizeof(buf), format, ap);
        for(char *p = &buf[0]; *p; p++) // emulate cooked mode for newlines
        {
                if(*p == '\n')
                        write('\r');
                write(*p);
        }
        va_end(ap);
   }
#ifdef F // check to see if F() macro is available
   void printf(const __FlashStringHelper *format, ...)
   {
   char buf[PRINTF_BUF];
   va_list ap;
        va_start(ap, format);
#ifdef __AVR__
        vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
#else
        vsnprintf(buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
#endif
        for(char *p = &buf[0]; *p; p++) // emulate cooked mode for newlines
        {
                if(*p == '\n')
                        write('\r');
                write(*p);
        }
        va_end(ap);
   }
#endif
```

needs to be added near the end of the file, just prior to the second to last non blank line:  `};`

NOTE this change must be made ANY TIME a new version of the Arduino IDE is installed, as the new install will overwrite the change to the `Print.h` file. In most recent Arduino upddates (since 1.6.X at least) the `Print.h` file has not changed, so a copy of the updated file can be squirreled away to save some time in the update process. Note that there was a minor change to the base `Print.h` file as of Arduino IDE 1.8.2 (this change should be integrated in the updated Print.h file as well).

At this point the Arduino IDE should be fully setup for use in compiling the emulator software.

Under the `Preferences` menu, I find ticking the `Show verbose output` `compilation` and `upload` boxes to be useful.

I also recommend checking the `Verify code after upload` box in the same dialog.

## RX02 Emulator ##

To download the RX02 emulator Arduino software, go to:

https://github.com/AK6DN/rx02_emulator

and (like in the library download) click the `Clone or download` green button on the right side of the page, select `Download ZIP`, and save the file. It will likely be named `rx02_emulator-master.zip`.

Unzip the contents into a folder, it will likely end up being named `rx02_emulator-master`. You must rename it to `rx02_emulator` as the Arduino IDE requires the outer level folder name match the project name (which is `rx02_emulator.ino`).

## Compiling ##

You should now be able to launch the Arduino IDE by going into the `source/rx02_emulator` folder and double clicking on the top level design file `rx02_emulator.ino`.

Before compiling you need to select the board and processor type.

Under the `Tools` menu select `Board:` and validate it is: `Arduino/Genuino Mega or Mega 2560`

Under the `Tools` menu select `Processor:` and validate it is: `ATmega2560`

You should now be able to, under the `Sketch` menu, select `Verify/Compile` (or type `CTRL-R`) to compile.

There should be no errors flagged if the SDfat library was installed correctly, the printf mod was performed, and the board and processor type have been configured correctly.

## Uploading ##

After verifying the compile you are now ready to be able to upload the code to your board.

NOW plug your Arduino Mega2560 board into a USB port on your system and wait for the USB driver to install.

When done, go to the `Tools` menu (again) and select the `Port:` entry to correspond to the USB serial port your board has been configured as.

You should now be able to select menu `Sketch` then `Upload` (or type `CTRL-U`) to upload the code to your board.

## Running ##

After a successful upload, your Arduino should start running immediately. To get access to the menu window, select under the `Tools` menu the `Serial Monitor` item (or type `CTRL-SHIFT-M`).

You will need to configure the baud rate / line ending of the monitor window before you can see output and type input.

In the lower right of the window, select the maximum baud rate `250000 baud` that the Arduino supports over USB. This minimizes the impact of the debug output window on performance, and the emulator software selects this rate by default. As of emulator code v1.8 and Arduino IDE 1.8.2 or later it is possible to enable higher debug window baud rates (500Kb, 1Mb, or 2Mb) but not all Arduino Mega2560 boards may support these rates 100% reliably. You will have to test your board individually to see if a higher baud rate can be enabled.

In the lower right, select `Carriage Return` line ending, so when you type in the text box at the top of the window, and then click `SEND` a `CR` character will be appended to the end of the string.

At this point you should be seeing debug output in the serial monitor window, and be able to send menu commands to it.

## The End ##

