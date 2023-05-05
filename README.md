# ATtinyTRNG
random bytes made from CPU jitter.
Both random sources are checked for failure: if one of them fails the MCU is put to sleep.
A speed of appr. 96 B/s is achieved.

Tests:
The following tests have been done at room temperature (19..22 degrees Celcius) on a file of 19MB that has been created by
cat /dev/ttyACM0 > test.bin:
Me:
 minimum entropy: 1.00 bits per bit

Ent:
 Entropy = 1.000000 bits per bit.

 Optimum compression would reduce the size
 of this 204722968 bit file by 0 percent.

 Chi square distribution for 204722968 samples is 0.28, and randomly
 would exceed this value 59.77 percent of the times.

 Arithmetic mean value of data bits is 0.5000 (0.5 = random).
 Monte Carlo value for Pi is 3.141752955 (error 0.01 percent).
 Serial correlation coefficient is -0.000026 (totally uncorrelated = 0.0).

Rngtest:
 rngtest 6.16
 Copyright (c) 2004 by Henrique de Moraes Holschuh
 This is free software; see the source for copying conditions.  There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A 

 rngtest: starting FIPS tests...
 rngtest: entropy source drained
 rngtest: bits received from input: 204722968
 rngtest: FIPS 140-2 successes: 10235
 rngtest: FIPS 140-2 failures: 1
 rngtest: FIPS 140-2(2001-10-10) Monobit: 0
 rngtest: FIPS 140-2(2001-10-10) Poker: 0
 rngtest: FIPS 140-2(2001-10-10) Runs: 0
 rngtest: FIPS 140-2(2001-10-10) Long run: 1
 rngtest: FIPS 140-2(2001-10-10) Continuous run: 0
 rngtest: input channel speed: (min=280.492; avg=4261.869; max=6357.829)Mibits/s
 rngtest: FIPS tests speed: (min=43.153; avg=113.487; max=123.854)Mibits/s
 rngtest: Program run time: 1768244 microseconds

Compile and upload ATtinyTRNG.ino with Arduino CLI:
arduino-cli compile -e --fqbn digistump:avr:digispark-tiny ATtinyTRNG.ino
arduino-cli upload -p /dev/ttyACM0 --fqbn digistump:avr:digispark-tiny ATtinyTRNG.ino

Practical implementation in Linux:
in crontab @reboot:
stty -F /dev/ttyACM0 -brkint -icrnl -imaxbel -opost -isig -icanon min 1 time 0

/etc/modules-load.d/modules.conf: cdc-acm

/etc/conf.d/rngd: RNGD_OPTS="-r /dev/ttyACM0 -x tpm -x nist -x qrypt -x jitter -x rtlsdr -x pkcs11
systemctl start rngd && systemctl enable rngd

License:
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Sources:
https://github.com/Wanderingidea/Minimum-Entropy
https://www.chronox.de/jent.html
https://www.osti.gov/biblio/1825588
https://blogs.oracle.com/linux/post/rngd1
https://aliexpress.com/item/1005004213157718.html
