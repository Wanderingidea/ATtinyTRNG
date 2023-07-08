# ATtinyTRNG
ATtinyTRNG - True Random Number Generator<br>
Cor van Wandelen 7-2023.1<br>
![board](https://user-images.githubusercontent.com/42114791/236681679-c37b7d7e-ece2-4589-a5ea-2ed1f0db1bc8.jpg)

A small, cheap generic True Random Number Generator has been made by combining two random sources on a ATtiny85 board.

### Purpose:<br>
This True Random Number Generator produces high quality non-deterministic random numbers. As an example, these numbers can be used to seed a fast (deterministic) pseudo random number generator.

### Method:<br>
Random bytes made from the 4 available ADCs on a ATtiny85 MCU are used to seed a Middle Square Weyl Sequence prng which in turn is used to generate CPU jitter.<br>
The output of CPU jitter is then combined with the output of the Middle Square Weyl Sequence prng. To squeeze as much entropy out of the MCU as possible, jitter of the main loop is also used.<br>
Both true random sources are checked for failure: if one of them fails the MCU is put to sleep. Every 5 seconds a activity led blinks.<br>
A speed of appr. 54 B/s is achieved.

### Compile:<br>
**Compile and upload ATtinyTRNG.ino to the ATtiny85 board using Arduino CLI:**<br>
`arduino-cli compile -e --fqbn digistump:avr:digispark-tiny ATtinyTRNG.ino`<br>
`arduino-cli upload -p /dev/ttyACM0 --fqbn digistump:avr:digispark-tiny ATtinyTRNG.ino`<br>

### Tests:<br>
The following tests have been done at room temperature (19..22 degrees Celcius) on a file of 2.6 MB that has been created by<br>
`stty -F /dev/ttyACM0 -brkint -icrnl -imaxbel -opost -isig -icanon min 1 time 0`<br> 
`cat /dev/ttyACM0 > test.bin`<br>

**Me:**<br>
```
minimum entropy: 0.99968046 bits per bit
```
**Ent:**<br>
```            
Entropy = 1.000000 bits per bit.

Optimum compression would reduce the size
of this 22229064 bit file by 0 percent.

Chi square distribution for 22229064 samples is 1.09, and randomly
would exceed this value 29.63 percent of the times.

Arithmetic mean value of data bits is 0.4999 (0.5 = random).
Monte Carlo value for Pi is 3.143382170 (error 0.06 percent).
Serial correlation coefficient is 0.000699 (totally uncorrelated = 0.0).
```
**Rngtest:**<br>
```
rngtest 6.16
Copyright (c) 2004 by Henrique de Moraes Holschuh
This is free software; see the source for copying conditions.  There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

rngtest: starting FIPS tests...
rngtest: entropy source drained
rngtest: bits received from input: 22229064
rngtest: FIPS 140-2 successes: 1110
rngtest: FIPS 140-2 failures: 1
rngtest: FIPS 140-2(2001-10-10) Monobit: 0
rngtest: FIPS 140-2(2001-10-10) Poker: 0
rngtest: FIPS 140-2(2001-10-10) Runs: 0
rngtest: FIPS 140-2(2001-10-10) Long run: 1
rngtest: FIPS 140-2(2001-10-10) Continuous run: 0
rngtest: input channel speed: (min=1.433; avg=4.433; max=6.209)Gibits/s
rngtest: FIPS tests speed: (min=30.616; avg=113.743; max=123.854)Mibits/s
rngtest: Program run time: 191298 microseconds
```
**NIST SP800-22:**<br>
```
monobit_test                             0.29631082567160816 PASS
frequency_within_block_test              0.2670881774723328 PASS
runs_test                                0.0009755312129244377 FAIL
longest_run_ones_in_a_block_test         0.31479034613636525 PASS
binary_matrix_rank_test                  0.7725289791486538 PASS
non_overlapping_template_matching_test   1.039453644187749  PASS
overlapping_template_matching_test       0.33962534450833626 PASS
maurers_universal_test                   0.9059121900535229 PASS
linear_complexity_test                   0.22886410125438497 PASS
serial_test                              0.0283208929213678 PASS
approximate_entropy_test                 0.02834573920951043 PASS
cumulative_sums_test                     0.35500899604305225 PASS
random_excursion_test                    0.10414609594429489 PASS
random_excursion_variant_test            0.09625989832347308 PASS
```
**Practical implementation in Linux:**<br>
in crontab @reboot:<br>
`stty -F /dev/ttyACM0 -brkint -icrnl -imaxbel -opost -isig -icanon min 1 time 0`<br>

/etc/modules-load.d/modules.conf:<br> 
`cdc-acm`

/etc/conf.d/rngd:<br> `RNGD_OPTS="-r /dev/ttyACM0 -x rdrand -x tpm -x nist -x qrypt -x jitter -x rtlsdr -x pkcs11`<br>
`systemctl start rngd && systemctl enable rngd`

### Sources:<br>
https://en.wikipedia.org/wiki/Hardware_random_number_generator<br>
https://github.com/Wanderingidea/ESPTRNG<br>
https://www.chronox.de/jent.html<br>
https://www.osti.gov/biblio/1825588<br>
https://blogs.oracle.com/linux/post/rngd1<br>
https://github.com/Wanderingidea/Minimum-Entropy<br>
https://aliexpress.com/item/1005004213157718.html
