# ATtinyTRNG
ATtinyTRNG - True Random Number Generator<br>
Cor van Wandelen 6-2023<br>
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
```

```
**Ent:**<br>
```            

```
**Rngtest:**<br>
```

```
**NIST SP800-22:**<br>
```

```
**Practical implementation in Linux:**<br>
in crontab @reboot:<br>
`stty -F /dev/ttyACM0 -brkint -icrnl -imaxbel -opost -isig -icanon min 1 time 0`<br>

/etc/modules-load.d/modules.conf:<br> 
`cdc-acm`

/etc/conf.d/rngd:<br> `RNGD_OPTS="-r /dev/ttyACM0 -x tpm -x nist -x qrypt -x jitter -x rtlsdr -x pkcs11`<br>
`systemctl start rngd && systemctl enable rngd`

### Sources:<br>
https://en.wikipedia.org/wiki/Hardware_random_number_generator<br>
https://github.com/Wanderingidea/ESPTRNG<br>
https://www.chronox.de/jent.html<br>
https://www.osti.gov/biblio/1825588<br>
https://blogs.oracle.com/linux/post/rngd1<br>
https://github.com/Wanderingidea/Minimum-Entropy<br>
https://aliexpress.com/item/1005004213157718.html
