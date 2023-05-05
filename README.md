# ATtinyTRNG
ATtinyTRNG - True Random Number Generator
Cor van Wandelen 4-2023

Rationale:<br>
A small generic True Random Number generator has been made by combining two random sources on a ATtiny85 board.

Method:<br>
The xored stream of random bytes made from the 8 available ADCs on a ATtiny85 MCU are combined with the xored stream of
random bytes made from CPU jitter.<br>
Both random sources are checked for failure: if one of them fails the MCU is put to sleep.<br>
A speed of appr. 96 B/s is achieved.

Tests:<br>
The following tests have been done at room temperature (19..22 degrees Celcius) on a file of 24MB that has been created by<br>
`cat /dev/ttyACM0 > test.bin`<br>

Me:<br>
 minimum entropy: 1.00 bits per bit<br>

Ent:<br>
 Entropy = 1.000000 bits per bit.<br>
<br>
 Optimum compression would reduce the size<br>
 of this 204722968 bit file by 0 percent.<br>
<br>
 Chi square distribution for 204722968 samples is 0.28, and randomly<br>
 would exceed this value 59.77 percent of the times.<br>
<br>
 Arithmetic mean value of data bits is 0.5000 (0.5 = random).<br>
 Monte Carlo value for Pi is 3.141752955 (error 0.01 percent).<br>
 Serial correlation coefficient is -0.000026 (totally uncorrelated = 0.0).<br>
<br>
Rngtest:<br>
   rngtest 6.16<br>
   Copyright (c) 2004 by Henrique de Moraes Holschuh<br>
   This is free software; see the source for copying conditions.  There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A <br>
<br>
 rngtest: starting FIPS tests...<br>
 rngtest: entropy source drained<br>
 rngtest: bits received from input: 204722968<br>
 rngtest: FIPS 140-2 successes: 10235<br>
 rngtest: FIPS 140-2 failures: 1<br>
 rngtest: FIPS 140-2(2001-10-10) Monobit: 0<br>
 rngtest: FIPS 140-2(2001-10-10) Poker: 0<br>
 rngtest: FIPS 140-2(2001-10-10) Runs: 0<br>
 rngtest: FIPS 140-2(2001-10-10) Long run: 1<br>
 rngtest: FIPS 140-2(2001-10-10) Continuous run: 0<br>
 rngtest: input channel speed: (min=280.492; avg=4261.869; max=6357.829)Mibits/s<br>
 rngtest: FIPS tests speed: (min=43.153; avg=113.487; max=123.854)Mibits/s<br>
 rngtest: Program run time: 1768244 microseconds<br>

Compile and upload ATtinyTRNG.ino with Arduino CLI:<br>
`arduino-cli compile -e --fqbn digistump:avr:digispark-tiny ATtinyTRNG.ino`<br>
`arduino-cli upload -p /dev/ttyACM0 --fqbn digistump:avr:digispark-tiny ATtinyTRNG.ino`<br>

Practical implementation in Linux:<br>
in crontab @reboot:<br>
`stty -F /dev/ttyACM0 -brkint -icrnl -imaxbel -opost -isig -icanon min 1 time 0`<br>

/etc/modules-load.d/modules.conf: cdc-acm

/etc/conf.d/rngd: RNGD_OPTS="-r /dev/ttyACM0 -x tpm -x nist -x qrypt -x jitter -x rtlsdr -x pkcs11
systemctl start rngd && systemctl enable rngd

Sources:<br>
https://github.com/Wanderingidea/Minimum-Entropy<br>
https://www.chronox.de/jent.html<br>
https://www.osti.gov/biblio/1825588<br>
https://blogs.oracle.com/linux/post/rngd1<br>
https://aliexpress.com/item/1005004213157718.html
