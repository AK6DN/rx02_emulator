<B>rx02_emulator</B> is a hardware/software emulation of a DEC RX02 dual 8" floppy disk drive. The software runs on an Arduino Mega2560 processor board with a custom hardware interface shield that maps a dozen or so digital port bits to the custom DEC RX02 interface hardware protocol.

The emulator simulates two RX02 drives mapped to files on an attached MicroSD card.

This implementation was originally based on the design at:  http://www.chdickman.com/rx02/ but has been extensively modified for the Arduino and to provide the functionality necessary to pass the DEC RX02 hardware diagnostics.

Currently this design boots/runs under the XXDP and RT-11 operating systems, and passes the DEC RX02 hardware diagnostics ZRXDC0 Performance Exerciser, ZRXEA0 Formatter, and ZRXFB0 RX02/RX211 Logic/Function tests error free.

Testing to date has been performed as an RX02/RX211 interface combo in a UNIBUS PDP-11/44, and in a PDP-8m with an RX8E/RX28 interface so RX01/RX8E and RX02/RX28 configurations can be tested. I do not have access to RX11, RXV11, or RXV21 boards for testing.

The MicroSD card in the emulator is a FAT32 formatted filesystem, and it can be inserted (offline) into a WindowsPC to copy files to/from the device. By default, the files 'RX0.DSK' and 'RX1.DSK' are mapped to drive 0 and 1 on initialization.

The emulator interfaces thru a simple ASCII terminal command line via the USB port on the Arduino device. After booting, typing:  H<cr>  types out the available commands. Here is a sample interaction showing the use of the H, L, S and 0 commands, and the debug level 1 log of what happens when 'b dy0' is typed on the attached PDP-11/44 console terminal.

Startup configuration is saved in an ASCII text file SETUP.INI that contains user interface commands that are replayed on startup. The SETUP.INI file is written using the W command, and the current 0/1/Y/N/D/M/T options are saved in the file.

The hardware shield has three indicator LEDs:
<OL>
<LI>(RED) ON when INIT is asserted from the host, and when an error is detected in the emulator. Turned OFF at the start of a new function.</LI>
<LI>(YEL) ON when the emulator is actively accessing the microSD card storage. OFF otherwise.</LI>
<LI>(GRN) ON when a command is inititated from the host, OFF when the command has completed (DONE set).</LI>
</OL>

Normal operation will see the GRN/YEL LEDs blinking rapidly or mostly ON. For non-storage commands (eg, buffer fill/empty) only GRN will be ON.

Sample boot log in the Arduino USB serial monitor window:
```
RX02 Emulator v1.3 - Oct 23 2016 - 23:47:10

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

  0 filename.dsk    -- set unit 0 file name; default RX0.DSK
                       filename 'none' (any case) for no disk present
  1 filename.dsk    -- set unit 1 file name; default RX1.DSK
                       filename 'none' (any case) for no disk present
  y(es) N           -- set unit N file read-write (default)
  n(o) N            -- set unit N file read-only
  m(ode) N          -- set emulation mode, 0=NONE, n=RX0n; default 2
  d(ebug) N         -- debug level, 0=none, 3=max; default 1
  t(iming) N        -- timing mode, 0=fast, 1=medium, 2=normal; default 0
                       0 as fast as possible; 2 simulates real RX02 drive
  l(ist)            -- list all files on the SD card
  s(how)            -- show current unit filename assignments
  p(rint)           -- print full emulation state
  i(nit)            -- initialize emulator (like unibus INIT)
  w(rite)           -- write current configuration into the SETUP.INI file
  h(elp)            -- display this text

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

Notes:

(1) This code has been written with the assumption that <B>Xprintf</B> support has been added to the PRINT class in the Arduino development environment.
Refer to:  http://playground.arduino.cc/Main/Printf  for instructions on how to do this.

(2) This code uses the SDfat library available at:  https://github.com/greiman/SdFat . It must be installed as an accessible library in your Arduino environment.

(3) This code was written with tap stops set at 4 (versus the default of 2). Manually edit the Arduino <B>preferences.txt</B> file tab size line to be: <B>editor.tabs.size=4</B> if desired.

(4) Right now there is a lot of 'extraneous' code (ie, the TU58 driver interface) that is included by default but not currently used. A future plan is to add support to map a backend TU58 server to a file connection (ie, by using a pseudo filename) so that not only local MicroSD card file access can be supported, but simultaenous access to a backend PC-based file storage server can happen.

Don
