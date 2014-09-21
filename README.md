e1gen
=====

Small but smart command-line utility for generation of E1 (2.048 Mbit/s, ITU-T
G.704) stream.

Program receives parameters of E1 stream (structure, framing, CRC4, etc...) and
writes corresponding byte-stream to stdout. User able to pass a file for
reading timeslot data from it. Each timeslot's data can be replaced by the
specified file's data. 

How it works
============

It runs a cycle from 0 to 31 (remember that E1 contain 32 timeslots), detects
the source of data for i-th timeslot, reads one byte and writes it to the
stdout.

Source for timeslot is detected by the priority. Highest priority has the file
configured for this timeslot. Next priority is the default algorithm which is
actual for 0th timeslot for example. Next is the default "silence" value which
stays the same for all homeless timeslots.

Number of generated frames can be limited or continuous. 

Usage, options
==============

Usage:
 
    ./e1gen [OPTION]

Options:

  * -s <struct>, --structure=<struct> Sets E1 structure. 
    Values are: PCM30, PCM31, UNSTR. Default = PCM30.
  * -F <framing>, --framing=<framing> Sets E1 framing: double-frame or
      multi-frame. Values are: 2 for double-frame, 16 for multi-frame. 
      Default = 2
  * -c <on/off>, --crc4=<on/off> Enables of disables insertion of CRC-4
       bits into SYNC timeslot. Valid only with framing=16.
       Default = on.
  * -S <hexbyte>, --silence-byte=<hexbyte> Sets silence byte, which is
       transmitted into unused timeslots. Default = 0
  * -n <num-of-frames>, --num-of-frames=<integer> Sets number of frames
       to generate. Default value is 0 (unlimited). 
  * -t <tsnum:file>, --timeslot=<tsnum:file> Sets a file for a specified timeslot.
       File will be read each time timeslot data is required. Example: -t 1:/dev/null
       If file read will report EOF or error, specified timeslot will be switched
       to filled with silence byte
  * -v, --verbose print more
  * -V, --version print version
  * -h, --help print this message

So, by default E1 stream is generated as PCM30 with double-frame continuosly.

Build
=====

    $ make

Examples
========

Default empty E1 stream:

    $ e1gen | some_program

E1 stream with 1st timeslot replaced by the data from /dev/urandom.
16 frames are generated.

    $ e1gen -t 1:/dev/urandom -n16 | some_program

E1 stream with 1st and 5th timeslots replaced by the data from file '/tmp/hdlc'.
Bytes are read cyclically: one byte to 1st timeslot, next byte to 5th timeslot and so on.

    $ e1gen -t 1:/tmp/hdlc -t 5:/tmp/hdlc | some_program

 
