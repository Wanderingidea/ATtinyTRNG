#include <DigiCDC.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

/*
 * ATtinyTRNG - True Random Number Generator
 * Cor van Wandelen 6-2023
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
static inline uint32_t ADCnoise32X(void)
{
	static uint8_t sc, stuckctr, ADCctr;
	uint32_t r = 0;
	uint32_t ar;
	static uint32_t m, oar = analogRead(0);
	ADCctr++;
	ADCctr %= 5;
	for (uint8_t n = 0; n < 32; n++) {
		ar = analogRead(ADCctr);
		if (ar == oar) {
			sc++;
			if (sc > 20) {
				stuckctr++;
				if (stuckctr > 6)
					set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			}
		} else {
			sc = 0;
		}
		oar = ar;
		r |= (ar & 0b00000001) << n;
	}
	m ^= r; // whitening
	return m;
}

//get source byte from jitter
static inline uint8_t jitter8X(void)
{
	static uint64_t ctr, s = ADCnoise32X() | 1, x, w;
	static uint8_t rs, sc, stuckctr;
	uint8_t r = 0;
	uint8_t t0, dt;
	for (uint8_t n = 0; n < 8; n++) {
		t0 = TCNT0;
		for (uint32_t n = 0; n < 25; n++) {
			//Middle Square Weyl Sequence prng
			//http://export.arxiv.org/pdf/1704.00358
			x *= x;
			w += s;
			x += w;
			x = (x >> 32) | (x << 32);
			if (ctr++ == 0)
				s = ADCnoise32X() | 1;
		}
		dt = TCNT0 - t0;
		if (dt == 0) {
			sc++;
			if (sc > 3) {
				stuckctr++;
				if (stuckctr > 6)
					set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			}
		} else {
			sc = 0;
		}
		r |= (dt & 0b00000001) << n;
	}
	rs ^= r;
	return rs ^ (uint8_t)x;
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
	if (goblink) {
		blink();
		goblink = false;
		jitter8X();
	}

	SerialUSB.write(jitter8X());

	SerialUSB.task();
}
