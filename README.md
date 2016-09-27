<B>rx02_emulator</B> is a hardware/software emulation of a DEC RX02 dual 8" floppy disk drive. The software code runs on an Arduino Mega2560 processor board with a custom hardware interface shield that maps a dozen or so port bits to the custom DEC RX02 interface protocol.

The emulator simulates two RX02 drives mapped to files on an attached MicroSD card.

This implementation was originally based on the design at:  http://www.chdickman.com/rx02/ but has been extensively modified for the Arduino and to correct issues regarding to hardware functionality.

As it currently stands this implementation boots/runs on the XXDP and RT-11 operating systems, and passes the DEC RX02 hardware diagnostics ZRXDC0 Performance Exerciser, ZRXEA0 Formatter, and ZRXFB0 RX02/RX211 Logic/Function Test.

<B>More detailed information to be provided ...</B>

The MicroSD card in the emulator is a FAT32 formatted filesystem, and it can be inserted (offline) into a WindowsPC to copy files to/from the device. By default, the files 'RX0.DSK' and 'RX1.DSK' are mapped to drive 0 and 1 on initialization.

The emulator interfaces thru a simple ASCII termianl command line via the USB port on the Arduino device. After booting, typing:  H<cr>  types out the available commands.

```
RX02 Emulator v1.0 - Sep 26 2016 - 14:51:21
No valid MicroSD card detected.
RX: waiting for INIT to clear ... t=1100ms
RX: INIT has cleared t=1101ms
RX: INIT rx_xmit_es(0004)

Initialization complete.
H

Commands available:

  0 filename.dsk    -- set unit 0 file name; default RX0.DSK
                       filename 'none' (any case) for no disk present
  1 filename.dsk    -- set unit 1 file name; default RX1.DSK
                       filename 'none' (any case) for no disk present
  m(ode) (RX0)N     -- set emulation mode RX01/RX02; default RX02
  d(ebug) N         -- debug level, 0=none, 2=max; default 1
  t(iming) N        -- timing mode, 0=fast, 1=medium, 2=normal; default 0
                       0 is as fast as possible; 2 simulates real RX02 drive
  l(ist)            -- list all files on the SD card
  s(how)            -- show current unit filename assignments
  p(rint)           -- print full emulation state
  i(nit)            -- initialize emulator (like unibus INIT)
  h(elp)            -- display this text

Note: chars in () are optional
```

