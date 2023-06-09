# ATtinyTRNG
ATtinyTRNG - True Random Number Generator<br>
Cor van Wandelen 4-2023<br>
![board](https://user-images.githubusercontent.com/42114791/236681679-c37b7d7e-ece2-4589-a5ea-2ed1f0db1bc8.jpg)

A small, cheap generic True Random Number Generator has been made by combining two random sources on a ATtiny85 board.

### Purpose:<br>
This True Random Number Generator produces high quality non-deterministic random numbers. As an example, these numbers can be used to seed a fast (deterministic) pseudo random number generator.

### Method:<br>
Random bytes made from the 4 available ADCs on a ATtiny85 MCU are used to seed a Middle Square Weyl Sequence prng which in turn is used to generate CPU jitter.<br>
The output of CPU jitter is then combined with the output of the Middle Square Weyl Sequence prng.
Both true random sources are checked for failure: if one of them fails the MCU is put to sleep. Every 5 seconds a activity led blinks.<br>
A speed of appr. 50 B/s is achieved.

### Compile:<br>
**Compile and upload ATtinyTRNG.ino to the ATtiny85 board using Arduino CLI:**<br>
`arduino-cli compile -e --fqbn digistump:avr:digispark-tiny ATtinyTRNG.ino`<br>
`arduino-cli upload -p /dev/ttyACM0 --fqbn digistump:avr:digispark-tiny ATtinyTRNG.ino`<br>

### Tests:<br>
The following tests have been done at room temperature (19..22 degrees Celcius) on a file of 2.3MB that has been created by<br>
`stty -F /dev/ttyACM0 -brkint -icrnl -imaxbel -opost -isig -icanon min 1 time 0`<br> 
`cat /dev/ttyACM0 > test.bin`<br>

**Me:**<br>
 minimum entropy: 0.99980954 bits per bit<br>

**Ent:**<br>
Entropy = 1.000000 bits per bit.<br>
<br>
Optimum compression would reduce the size<br>
of this 19011552 bit file by 0 percent.<br>
<br>
Chi square distribution for 19011552 samples is 0.33, and randomly<br>
would exceed this value 56.48 percent of the times.<br>
<br>
Arithmetic mean value of data bits is 0.4999 (0.5 = random).<br>
Monte Carlo value for Pi is 3.141625050 (error 0.00 percent).<br>
Serial correlation coefficient is -0.000063 (totally uncorrelated = 0.0).<br>
<br>
**Rngtest:**<br>
rngtest 6.16<br>
Copyright (c) 2004 by Henrique de Moraes Holschuh<br>
This is free software; see the source for copying conditions.  There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<br>
<br>
rngtest: starting FIPS tests...<br>
rngtest: entropy source drained<br>
rngtest: bits received from input: 19011552<br>
rngtest: FIPS 140-2 successes: 949<br>
rngtest: FIPS 140-2 failures: 1<br>
rngtest: FIPS 140-2(2001-10-10) Monobit: 0<br>
rngtest: FIPS 140-2(2001-10-10) Poker: 0<br>
rngtest: FIPS 140-2(2001-10-10) Runs: 0<br>
rngtest: FIPS 140-2(2001-10-10) Long run: 1<br>
rngtest: FIPS 140-2(2001-10-10) Continuous run: 0<br>
rngtest: input channel speed: (min=1.164; avg=4.311; max=6.209)Gibits/s<br>
rngtest: FIPS tests speed: (min=20.226; avg=117.844; max=123.854)Mibits/s<br>
rngtest: Program run time: 158105 microseconds<br>
<br>
**NIST SP800-22:**<br>
`-------<br>
monobit_test                             0.5648460187983009 PASS<br>
frequency_within_block_test              0.13757301945679695 PASS<br>
runs_test                                0.783797336913128  PASS<br>
longest_run_ones_in_a_block_test         0.8381538149869271 PASS<br>
binary_matrix_rank_test                  0.6193565682398411 PASS<br>
non_overlapping_template_matching_test   1.0053096330682256 PASS<br>
overlapping_template_matching_test       0.02574920384665839 PASS<br>
maurers_universal_test                   0.6767816307099659 PASS<br>
linear_complexity_test                   0.9294389193511788 PASS<br>
serial_test                              0.0410078100805506 PASS<br>
approximate_entropy_test                 0.2028910018441725 PASS<br>
cumulative_sums_test                     0.6181876675828197 PASS<br>
random_excursion_test                    0.020468785095130067 PASS<br>
random_excursion_variant_test            0.03714850560062697 PASS<br>
<br>
**Practical implementation in Linux:**<br>
in crontab @reboot:<br>
`stty -F /dev/ttyACM0 -brkint -icrnl -imaxbel -opost -isig -icanon min 1 time 0`<br>

/etc/modules-load.d/modules.conf: cdc-acm

/etc/conf.d/rngd: `RNGD_OPTS="-r /dev/ttyACM0 -x tpm -x nist -x qrypt -x jitter -x rtlsdr -x pkcs11`<br>
`systemctl start rngd && systemctl enable rngd`

### Sources:<br>
https://en.wikipedia.org/wiki/Hardware_random_number_generator<br>
https://www.chronox.de/jent.html<br>
https://www.osti.gov/biblio/1825588<br>
https://blogs.oracle.com/linux/post/rngd1<br>
https://github.com/Wanderingidea/Minimum-Entropy<br>
https://aliexpress.com/item/1005004213157718.html
