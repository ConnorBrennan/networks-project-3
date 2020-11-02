#
# This file should be updated with your name and any notes about your code.
#

Connor Brennan

I handle errors by computing a fletcher's checksum and verifying it before a packet is processed and the ack incremented.  Any packets after a lost or corrupted file are dropped until the missing file is resent in order to ensure in order delivery.  Acks are sent every 10 packets recieved, and based on a timer to ensure the data stream continues to completion without stilling.
The main issue I didn't account for is corruption of the ack number.  If the ack number gets corrupted such that it's higher than it should be, the program may flush the entire buffer and then have no way to proceed.