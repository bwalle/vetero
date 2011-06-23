#!/usr/bin/perl -w
#
# (c) 2011, Bernhard Walle <bernhard@bwalle.de>
#
# Simple simulator using Unix ptys. Just edit the vetero configuration to use
# the printed slave as serial device. Then you can send data to the device
# until you are happy.
#
# The advantage of Ptys is that the application can treat it as normal serial
# line. It just ignores baud rate setting etc (which a normal named pipe would
# not).

use IO::Pty;

$pty = new IO::Pty;

$slave = $pty->slave;

print "Slave: " . $slave->ttyname() . "\n";

do {
    print "Press RETURN to send a new dataset, Ctrl-C to quit.\n";
    print $pty '$1;1;;;;;;;;;;;;;;;;;;20,2;53;3,6;1486;0;0' . "\n";
} while (<STDIN>);

close($slave);
close($pty);

# vim: set sw=4 ts=4 et:
