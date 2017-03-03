#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include "ATmega_types.h"
#include "SCD5583.h"

static uint8 init_io_pins(void);
static void fc_out(uint8);
static uint8 get_cpld_data(void);

#define CPLD_RST_ADDR		15
#define CPLD_STATUS_ADDR	14
#define CPLD_BASE_CLK		80000000L

int main() {
	uint8 addr, i;
	uint32 val;
	char str[256];
	/*
	CLKPR = 0x80;
	CLKPR = 0;
	*/
	init_io_pins();
	disp_rst();
	clear_disp();
	strcpy(str, "f=");
	disp_printf(str);
	while (1) {
		fc_out(CPLD_RST_ADDR);
		do asm ("nop"); while (!(get_cpld_data() & 1));
		fc_out(CPLD_STATUS_ADDR);
		do asm ("nop"); while (!(get_cpld_data() & 2));
		do asm ("nop"); while (get_cpld_data() & 2);
		for (addr = 0, val = 0; addr < 7; addr++) {
			fc_out(addr);
			asm ("nop");
			val |= (uint32)get_cpld_data() << 4 * addr;
		}
		strcpy(str, "f= ");
		sprintf(str + strlen(str), "%lu", val);
		strcat(str, " Hz");
		clear_disp();
		disp_printf(str);
		for (i = 0; i < 10; i++) wait_loop(65535);
	}
	return 0;
}

/*
	port D: 0..3 als Ausgang
			4..7 als Eingang
*/
static uint8 init_io_pins(void) {
    DDRA |= (uint8)(1 << RST_PIN);
    DDRA |= (uint8)(1 << LOAD_0_PIN);
    DDRA |= (uint8)(1 << LOAD_1_PIN);
    DDRA |= (uint8)(1 << SDCLK_PIN);
    DDRA |= (uint8)(1 << SDATA_PIN);
    PORTA &= (uint8)~(1 << RST_PIN);
    PORTA |= (uint8)(1 << LOAD_0_PIN);
    PORTA |= (uint8)(1 << LOAD_1_PIN);
    PORTA &= (uint8)~(1 << SDCLK_PIN);
    PORTA |= (uint8)(1 << SDATA_PIN);
	DDRD = 0x0F;
	/*
	DDRD |= (uint8)(1 << PIND0)|
			(uint8)(1 << PIND1)|
			(uint8)(1 << PIND2)|
			(uint8)(1 << PIND3);
	DDRD &= (uint8)(1 << PIND4);
	DDRD &= (uint8)(1 << PIND5);
	DDRD &= (uint8)(1 << PIND6);
	DDRD &= (uint8)(1 << PIND7);
	*/
    return 0;
}

static void fc_out(uint8 val) {
	PORTD = (val & 0x0F);
	return;
}

static uint8 get_cpld_data(void) {
	uint8 val = 0;
	val = PIND >> 4;
	return val;
}
