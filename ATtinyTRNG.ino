#include <DigiCDC.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

/*
 * ATtinyTRNG - True Random Number Generator
 * Cor van Wandelen 4-2023
 *
 * Rationale:
 * A small generic True Random Number generator has been made by combining two random sources on a ATtiny85 board.
 *
 * Method:
 * The xored stream of random bytes made from the 4 available ADCs on a ATtiny85 MCU are combined with the xored stream of
 * random bytes made from CPU jitter.
 * Both random sources are checked for failure: if one of them fails the MCU is put to sleep.
 * A speed of appr. 96 B/s is achieved.
 *
 * Compile:
 * Compile and upload ATtinyTRNG.ino to the ATtiny85 board using Arduino CLI:
 * arduino-cli compile -e --fqbn digistump:avr:digispark-tiny ATtinyTRNG.ino
 * arduino-cli upload -p /dev/ttyACM0 --fqbn digistump:avr:digispark-tiny ATtinyTRNG.ino
 *
 * Tests:
 * The following tests have been done at room temperature (19..22 degrees Celcius) on a file of 24MB that has been created by
 * stty -F /dev/ttyACM0 -brkint -icrnl -imaxbel -opost -isig -icanon min 1 time 0
 * cat /dev/ttyACM0 > test.bin:
 *
 * Me:
    minimum entropy: 1.00 bits per bit
 *
 * Ent:
    Entropy = 1.000000 bits per bit.

    Optimum compression would reduce the size
    of this 204722968 bit file by 0 percent.

    Chi square distribution for 204722968 samples is 0.28, and randomly
    would exceed this value 59.77 percent of the times.

    Arithmetic mean value of data bits is 0.5000 (0.5 = random).
    Monte Carlo value for Pi is 3.141752955 (error 0.01 percent).
    Serial correlation coefficient is -0.000026 (totally uncorrelated = 0.0).
 *
 * Rngtest:
    rngtest 6.16
    Copyright (c) 2004 by Henrique de Moraes Holschuh
    This is free software; see the source for copying conditions.  There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

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
 *
 * Practical implementation in Linux:
 * in crontab @reboot:
 * stty -F /dev/ttyACM0 -brkint -icrnl -imaxbel -opost -isig -icanon min 1 time 0
 *
 * /etc/modules-load.d/modules.conf: cdc-acm
 *
 * /etc/conf.d/rngd: RNGD_OPTS="-r /dev/ttyACM0 -x tpm -x nist -x qrypt -x jitter -x rtlsdr -x pkcs11
 * systemctl start rngd && systemctl enable rngd
 *
 * License:
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Sources:
 * https://github.com/Wanderingidea/Minimum-Entropy
 * https://www.chronox.de/jent.html
 * https://www.osti.gov/biblio/1825588
 * https://blogs.oracle.com/linux/post/rngd1
 * https://aliexpress.com/item/1005004213157718.html
 */

#define BLDLY    319 // determines blink interval (5 sec)

volatile uint16_t irqctr = 0;
volatile boolean goblink = false;

void setup()
{
	// setup timer interrupt
	cli();
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR0B |= (1 << CS00) | (1 << CS02);
	sei();
	TCNT0 = 0;
	TIMSK |= (1 << TOIE0);

	pinMode(1, OUTPUT);

    while (!SerialUSB);

    SerialUSB.begin();

	SerialUSB.println(F("ATtinyTRNG v.4-2023"));

	for (uint8_t n = 0; n < 20; n++) {
		SerialUSB.delay(200);
		blink();
	}
}

//timer0 interrupt code
ISR(TIMER0_OVF_vect){
	if (irqctr == BLDLY) { // 16/(1024/(639+1)) = 10 sec, 16/(1024/(63+1) = 1 sec, etc.
		goblink = true;
		irqctr = 0;
	} else {
		irqctr++;
	}
}

//temporarily disable compiler optimization
#pragma GCC push_options
#pragma GCC optimize ("O0")

//get source byte from ADC
static inline uint8_t ADCnoise8X(void)
{
	static uint8_t sc, stuckctr, m, ADCctr;
	uint8_t r;
    uint32_t ar;
    static uint32_t oar = analogRead(0);
	ADCctr++;
    ADCctr %= 5;
	for (uint8_t n = 0; n < 8; n++) {
        ar = analogRead(ADCctr);
        if (ar == oar) {
            sc++;
            if (sc > 20) {
                stuckctr++;
                if (stuckctr > 6) {
                    digitalWrite(1, HIGH);
                    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                }
            }
        } else
            sc = 0;
        oar = ar;
		r |= (ar & 0b00000001) << n;
    }
	m ^= r; // whitening
	return m;
}

//get source byte from jitter
static inline uint8_t jitter8X(void)
{
	static uint8_t m, rs, sc, stuckctr;
    uint8_t t0, dt, r = 0;
	for (uint8_t n = 0; n < 8; n++) {
        t0 = TCNT0;
        for (uint32_t n = 0; n < 20; n++) { // Xorshift32 is not used further
			m ^= m << 13;
			m ^= m >> 17;
			m ^= m << 5;
		}
        dt = TCNT0 - t0; // overflow, unsigned - it does not matter
        if (dt == 0) {
            sc++;
            if (sc > 3) {
                stuckctr++;
                if (stuckctr > 6) {
                    digitalWrite(1, HIGH);
                    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                }
            }
        } else
            sc = 0;
		r |= (dt & 0b00000001) << n;
    }
	rs ^= r; // whitening
	return rs;
}

//enable the original compiler settings
#pragma GCC pop_options

static void blink(void)
{
	digitalWrite(1, HIGH);
	SerialUSB.delay(20);
	digitalWrite(1, LOW);
}

void loop()
{
	static uint8_t out;

	if (goblink) {
		blink();
		goblink = false;
        ADCnoise8X();
        jitter8X();
    }

	out = jitter8X() ^ ADCnoise8X();

	SerialUSB.write(out);

    SerialUSB.task();
}
