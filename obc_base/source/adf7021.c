/*
 * Copyright (c) 2008 Johan Christiansen
 * Copyright (c) 2012 Jeppe Ledet-Pedersen <jlp@satlab.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>


// Platform-specific GPIO
#ifndef __AVR__	// For non-AVR MCUs...
	// Assume TI...
#	include "HL_gio.h"

#	define PINSET( PINNAME ) gioSetBit( ADF_PORTPIN_##PINNAME, 1 )
#	define PINCLEAR( PINNAME ) gioSetBit( ADF_PORTPIN_##PINNAME, 0 )
#	define PINREAD( PINNAME )  gioGetPort( ADF_PORTPIN_##PINNAME )

// A quick EFM32 alternative for missing function
void delay_ms(int nDelay) {
	// Just a ballpark, assuming 14MHz clock and about 4 cycles per iteration
	int i = nDelay * (14000 / 4);
	while (i--) { }
}
#else //__AVR__
#	include <avr/sfr_defs.h>
#	include <avr/interrupt.h>
#	include <avr/io.h>
#	include <avr/pgmspace.h>
#	include <util/delay.h>

#	define PINSET( PINNAME )	ADF_PORT_##PINNAME |= _BV( ADF_##PINNAME )
#	define PINCLEAR( PINNAME )	ADF_PORT_##PINNAME &= ~_BV( ADF_##PINNAME )
#	define PINREAD( PINNAME )	(ADF_PORT_##PINNAME & ADF_##PINNAME)
#endif


#include "adf7021.h"
//#include "bluebox.h"
//#include "ptt.h"
//#include "led.h"
#define BLUEBOX_UNUSED( f )	// Ignore Bluebox-specific functionality

static adf_conf_t rx_conf, tx_conf;
static adf_sysconf_t sys_conf;
static uint32_t adf_current_syncword;


/* Default settings */
#define FREQUENCY		437450000
#define TX_WAIT_TIMEOUT		120U
#define TX_TIMEOUT_DELAY	10U
#define RX_WAIT_TIMEOUT		120U
#define CSMA_RSSI		-50
#define BAUD_RATE		2400
#define MOD_INDEX		8
#define PA_SETTING		1	// Was 8. 0 is off for testing...
#define AFC_RANGE		10
#define AFC_KI			11
#define AFC_KP			4
#define AFC_ENABLE		1
#define IF_FILTER_BW		2
#define SYNC_WORD		0x4f5a33	// xxx
#define SYNC_WORD_TOLERANCE	ADF_SYNC_WORD_ERROR_TOLERANCE_3
#define SYNC_WORD_BITS		ADF_SYNC_WORD_LEN_24
#define TRAINING_SYMBOL		0x55
#define TRAINING_MS		200
#define TRAINING_INTER_MS	200
//#define PTT_DELAY_HIGH		100
//#define PTT_DELAY_LOW		100


//extern struct bluebox_config conf;
struct adf_config {
	//uint8_t flags;
	uint32_t tx_freq;
	uint32_t rx_freq;
	//int16_t csma_rssi;
	uint16_t bitrate;
	uint8_t modindex;
	uint8_t pa_setting;
	uint8_t afc_range;
	uint8_t afc_ki;
	uint8_t afc_kp;
	uint8_t afc_enable;
	uint8_t if_bw;
	uint32_t sw;
	uint8_t swtol;
	uint8_t swlen;
	//uint8_t do_rs;
	//uint8_t do_viterbi;
	//uint8_t training_symbol;
	//uint16_t training_ms;
	//uint16_t training_inter_ms;
	//char callsign[CALLSIGN_LENGTH];
	//uint32_t tx;
	//uint32_t rx;
	//uint16_t ptt_delay_high;
	//uint16_t ptt_delay_low;
	//char *fw_revision;
} conf = {
        //.flags = CONF_FLAG_NONE,
        .tx_freq = FREQUENCY,
        .rx_freq = FREQUENCY,
        //.csma_rssi = CSMA_RSSI,
        .bitrate = BAUD_RATE,
        .modindex = MOD_INDEX,
        .pa_setting = PA_SETTING,
        .afc_range = AFC_RANGE,
        .afc_ki = AFC_KI,
        .afc_kp = AFC_KP,
        .afc_enable = AFC_ENABLE,
        .if_bw = IF_FILTER_BW,
        .sw = SYNC_WORD,
        .swtol = SYNC_WORD_TOLERANCE,
        .swlen = SYNC_WORD_BITS,
        /*.do_rs = true,
        .do_viterbi = true,
        .callsign = CALLSIGN,
        .training_ms = TRAINING_MS,
        .training_inter_ms = TRAINING_INTER_MS,
        .training_symbol = TRAINING_SYMBOL,
        .ptt_delay_high = PTT_DELAY_HIGH,
        .ptt_delay_low = PTT_DELAY_LOW,
        .tx = 0,
        .rx = 0,
        .fw_revision = FW_REVISION,
        */
};

enum {
	ADF_OFF,
	ADF_ON,
	ADF_TX,
	ADF_RX
} adf_state;

enum {
	ADF_PA_OFF,
	ADF_PA_ON
} adf_pa_state;



void adf_write_reg(adf_reg_t *reg)
{
	signed char i, j;
	unsigned char byte;

	// xxx
	//delay_ms(100);

	PINCLEAR( SLE );
	PINCLEAR( SCLK );

	/* Clock data out MSbit first */
	for (i=3; i>=0; i--) {
		byte = reg->byte[i];

		for (j=8; j>0; j--) {
			PINCLEAR( SCLK );
			if (byte & 0x80)
				PINSET( SDATA );
			else 
				PINCLEAR( SDATA );
			PINSET( SCLK );
			byte += byte;
		}
		PINCLEAR( SCLK );
	}

	/* Strobe the latch */
	PINSET( SLE );
	PINSET( SLE );	// Not sure why. Maybe add slight delay.
	PINCLEAR( SDATA );
	PINCLEAR( SLE );
}

adf_reg_t adf_read_reg(unsigned int readback_config)
{
	adf_reg_t register_value;
	signed char i, j;

	/* Write readback and ADC control value */
	register_value.whole_reg = ((readback_config & 0x1F) << 4);

	/* Address the readback setup register */
	register_value.whole_reg |= 7; 
	adf_write_reg(&register_value);
	register_value.whole_reg = 0;

	/* Read back value */
	PINCLEAR( SDATA );
	PINCLEAR( SCLK );
	PINSET( SLE );

	/* Clock in first bit and discard (DB16 is not used) */
	PINSET( SCLK );
	unsigned char byte = 0;
	PINCLEAR( SCLK );

	/* Clock in data MSbit first */
	for (i=1; i>=0; i--) {
		for (j=8; j>0; j--) {
			PINSET( SCLK );
			PINSET( SCLK );
			byte += byte;
			if (PINREAD( SREAD ))
				byte |= 1;
			PINCLEAR( SCLK );
		}
		register_value.byte[i] = byte;
	}

	PINSET( SCLK );
	PINCLEAR( SLE );
	PINCLEAR( SCLK );
	printf("%ul", register_value.whole_reg);
	return register_value;
}

void adf_set_power_on(unsigned long adf_xtal)
{
	/* Store locally the oscillator frequency */
	sys_conf.adf_xtal = adf_xtal;

#ifdef _EFM_DEVICE
	// -- Configure inputs
	GPIO_PinModeSet(ADF_PORTPIN_SWD, gpioModeInput, 0);
	GPIO_PinModeSet(ADF_PORTPIN_SREAD, gpioModeInput, 0);
	GPIO_PinModeSet(ADF_PORTPIN_MUXOUT, gpioModeInput, 0);
	// -- Configure outputs
	GPIO_PinModeSet(ADF_PORTPIN_SCLK, gpioModePushPull, 0);
	GPIO_PinModeSet(ADF_PORTPIN_SDATA, gpioModePushPull, 0);
	GPIO_PinModeSet(ADF_PORTPIN_SLE, gpioModePushPull, 0);
	GPIO_PinModeSet(ADF_PORTPIN_CE, gpioModePushPull, 0);
#elif defined(__AVR__)
	/* Ensure the ADF GPIO port is correctly initialised */
	ADF_PORT_DIR_SWD 	&= ~_BV(ADF_SWD);
	ADF_PORT_DIR_SCLK 	|=  _BV(ADF_SCLK);
	ADF_PORT_DIR_SREAD	&= ~_BV(ADF_SREAD);
	ADF_PORT_DIR_SDATA	|=  _BV(ADF_SDATA);
	ADF_PORT_DIR_SLE	|=  _BV(ADF_SLE);
	ADF_PORT_DIR_MUXOUT	&= ~_BV(ADF_MUXOUT);
	ADF_PORT_DIR_CE		|=  _BV(ADF_CE);
#else
#	error Pins not configured!
#endif

	// Enable the ADF
	PINSET( CE );

	/* write R1, Turn on Internal VCO */
	sys_conf.r1.address_bits 	= 1;
	sys_conf.r1.r_counter 		= 2;
	sys_conf.r1.clockout_divide 	= 0;
	sys_conf.r1.xtal_doubler 	= 0;
	sys_conf.r1.xosc_enable 	= 1;	// 1 is external XTAL
	sys_conf.r1.xtal_bias 		= 2;//3;
	//EFM_ASSERT( sys_conf.r1_reg.whole_reg == 0x00005021 );	// check bit alignment 0b 101 0000 0010 0001
	sys_conf.r1.cp_current 		= 2;//3;
	sys_conf.r1.vco_enable 		= 1;//1;
	sys_conf.r1.rf_divide_by_2	= 0;//1;
	sys_conf.r1.vco_bias 		= 10;//1;//15;
	sys_conf.r1.vco_adjust 		= 0;//1;
	sys_conf.r1.vco_inductor 	= 0;
//xxx
	adf_write_reg(&sys_conf.r1_reg);

	//ADF7021.h:#define ADF7021_REG1_UHF1        0x00575021
	//ADF7021.h:#define ADF7021_REG1_UHF2        0x00535021

	// write R15, set CLK_MUX to enable SPI
	sys_conf.r15.address_bits 	= 15;
	sys_conf.r15.rx_test_mode  	= 0;
	sys_conf.r15.tx_test_mode 	= 0;
	sys_conf.r15.sd_test_mode 	= 0;
	sys_conf.r15.cp_test_mode 	= 0;
	sys_conf.r15.clk_mux 		= 7;
	//sys_conf.r15.clk_mux 		= 0; // xxx SPI not set up yet

	sys_conf.r15.pll_test_mode 	= 0;
	sys_conf.r15.analog_test_mode 	= 0;
	sys_conf.r15.force_ld_high 	= 0;
	sys_conf.r15.reg1_pd 		= 0;
	sys_conf.r15.cal_override 	= 0;
//xxx
	adf_write_reg(&sys_conf.r15_reg);

	// write R14, enable test DAC
	sys_conf.r14.address_bits	= 14;
	sys_conf.r14.test_tdac_en 	= 0;
	sys_conf.r14.test_dac_offset 	= 0;
	sys_conf.r14.test_dac_gain 	= 0;
	sys_conf.r14.pulse_ext 		= 0;
	sys_conf.r14.leak_factor 	= 0;
	sys_conf.r14.ed_peak_resp 	= 0;
	adf_write_reg(&sys_conf.r14_reg);

	adf_state = ADF_ON;
	adf_pa_state = ADF_PA_OFF;

	/*
	// xxx
	delay_ms( 300 );
	adf_init_rx_mode(conf.bitrate, conf.modindex, conf.rx_freq, conf.if_bw);
	delay_ms( 300 );
	*/
}

void adf_set_power_off()
{
	/* Turn off chip enable */
	PINCLEAR( CE );

	adf_state = ADF_OFF;
	adf_pa_state = ADF_PA_OFF;
}

void adf_find_clocks(adf_conf_t *conf)
{
	/* Find desired F.dev. */
	unsigned char tx_freq_dev = (unsigned char) round( ((double)conf->desired.mod_index * 0.5 * conf->desired.data_rate * 65536.0) / (0.5 * sys_conf.adf_xtal));
	double freq_dev = (tx_freq_dev * sys_conf.adf_xtal) / 65536.0;

	/* Find K */
	unsigned int k = round(100000 / (freq_dev));

	/* Run a variable optimisation for Demod clock divider */
	int i_dem;
	int w_residual = INT16_MAX, residual;

	double demod_clk, data_rate_real;
	unsigned int cdr_clk_divide, disc_bw;

	for (i_dem = 1; i_dem < 15; i_dem++) {
		demod_clk = (double) sys_conf.adf_xtal / i_dem;
		disc_bw = round((k * demod_clk) / 400000);
		cdr_clk_divide = round(demod_clk / ((double)conf->desired.data_rate * 32));

		if (disc_bw > 660)
			continue;

		if (cdr_clk_divide > 255)
			continue;

		data_rate_real = (sys_conf.adf_xtal / ((double) i_dem * (double) cdr_clk_divide * 32.0));
		residual = abs((unsigned int) data_rate_real - conf->desired.data_rate);

		/* Search for a new winner */
		if (w_residual > residual) {
			w_residual = residual;
			conf->r3.dem_clk_divide = i_dem;
		}
	}

	/* Demodulator clock */
	demod_clk = (double) sys_conf.adf_xtal / conf->r3.dem_clk_divide;

	/* CDR clock */
	conf->r3.cdr_clk_divide = (unsigned int) round(demod_clk / ((double)conf->desired.data_rate * 32));

	/* Data rate and freq. deviation */
	conf->real.data_rate = (sys_conf.adf_xtal / ((double) conf->r3.dem_clk_divide * (double) conf->r3.cdr_clk_divide * 32.0));
	conf->real.freq_dev = (unsigned char) round( ((double)conf->desired.mod_index * 0.5 * conf->real.data_rate * 65536.0) / (0.5 * sys_conf.adf_xtal));

	/* Discriminator bandwidth */
	conf->r4.disc_bw = round((k * demod_clk) / 400000);

	/* Post demodulation bandwidth */
	conf->r4.post_demod_bw = round(((conf->real.data_rate * 0.75) * 3.141592654 * 2048.0) / demod_clk);

	/* K odd or even */
	if (k & 1) { 
		if (((k + 1) / 2) & 1) {
			conf->r4.rx_invert = 2;
			conf->r4.dot_product = 1;
		} else {
			conf->r4.rx_invert = 0;
			conf->r4.dot_product = 1;
		}
	} else {
		if ((k / 2) & 1) {
			conf->r4.rx_invert = 2;
			conf->r4.dot_product = 0;
		} else {
			conf->r4.rx_invert = 0;
			conf->r4.dot_product = 0;
		}
	}
}

void adf_init_rx_mode(unsigned int data_rate, uint8_t mod_index, unsigned long freq, uint8_t if_bw)
{
	/* Calculate the RX clocks */
	rx_conf.desired.data_rate = data_rate;
	rx_conf.desired.mod_index = mod_index;
	rx_conf.desired.freq = freq;
	adf_find_clocks(&rx_conf);

	/* Setup RX Clocks */
	rx_conf.r3.seq_clk_divide = round(sys_conf.adf_xtal / 100000.0);
	rx_conf.r3.agc_clk_divide = round((rx_conf.r3.seq_clk_divide * sys_conf.adf_xtal) / 10000.0);
	rx_conf.r3.bbos_clk_divide = 2; // 16
	rx_conf.r3.address_bits = 3;

	/* IF filter calibration */
	rx_conf.r5.if_filter_divider = (sys_conf.adf_xtal / 50000);
	rx_conf.r5.if_cal_coarse = 1;
	rx_conf.r5.address_bits = 5;

	/* write R0, turn on PLL */
	double n = ((freq-100000) / (sys_conf.adf_xtal * 0.5));
	unsigned long n_int = floor(n);
	unsigned long n_frac = round((n - floor(n)) * 32768);

	rx_conf.r0.rx_on = 1;
	rx_conf.r0.uart_mode = 1;
	rx_conf.r0.muxout = 2;
	rx_conf.r0.int_n = n_int;
	rx_conf.r0.frac_n = n_frac;
	rx_conf.r0.address_bits = 0;

	/* write R4, turn on demodulation */
	rx_conf.r4.demod_scheme = 1;
	rx_conf.r4.if_bw = if_bw;	// 0 = 12.5, 1 = 18.75, 2 = 25 KHz
	rx_conf.r4.address_bits = 4;
}

void adf_init_tx_mode(unsigned int data_rate, uint8_t mod_index, unsigned long freq)
{
	/* Calculate the RX clocks */
	tx_conf.desired.data_rate = data_rate;
	tx_conf.desired.mod_index = mod_index;
	tx_conf.desired.freq = freq;
	adf_find_clocks(&tx_conf);

	/* Setup default R3 values */
	tx_conf.r3.seq_clk_divide = round(sys_conf.adf_xtal / 100000.0);
	tx_conf.r3.agc_clk_divide = round((tx_conf.r3.seq_clk_divide * sys_conf.adf_xtal) / 10000.0);
	tx_conf.r3.bbos_clk_divide = 2; // 16
	tx_conf.r3.address_bits = 3;

	/* write R0, turn on PLL */
	double n = (freq / (sys_conf.adf_xtal * 0.5));
	unsigned long n_int = floor(n);
	unsigned long n_frac = round((n - floor(n)) * 32768);

	tx_conf.r0.rx_on = 0;
	tx_conf.r0.uart_mode = 1;
	tx_conf.r0.muxout = 2;
	tx_conf.r0.int_n = n_int;
	tx_conf.r0.frac_n = n_frac;
	tx_conf.r0.address_bits = 0;

	/* Set the calcualted frequency deviation */
	tx_conf.r2.tx_frequency_deviation = tx_conf.real.freq_dev;

	/* Set PA and modulation type */
	tx_conf.r2.power_amplifier = conf.pa_setting;    // 0 = OFF, 63 = MAX (WARNING NON LINEAR)
	tx_conf.r2.pa_bias = 3;	            // 0 = 5uA, 1 = 7uA, 2 = 9uA, 3 = 11 uA
	tx_conf.r2.pa_ramp = 7;	            // 0 = OFF, 1 = LOWEST, 7 = HIGHEST
	tx_conf.r2.pa_enable = 1;           // 0 = OFF, 1 = ON
	tx_conf.r2.modulation_scheme = 1;   // 0 = FSK, 1 = GFSK, 5 = RCFSK
	tx_conf.r2.address_bits = 2;

	/* Ensure rewrite of PA register */
	adf_pa_state = ADF_PA_OFF;
}

void adf_afc_on(unsigned char range, unsigned char ki, unsigned char kp)
{
	/* write R10, turn AFC on */
	sys_conf.r10.afc_en = 1;
	sys_conf.r10.afc_scaling_factor = 524; /* (2^24 * 500 / XTAL_FREQ) */
	sys_conf.r10.ki = ki;
	sys_conf.r10.kp = kp;
	sys_conf.r10.afc_range = range;
	sys_conf.r10.address_bits = 10;
	adf_write_reg(&sys_conf.r10_reg);
}

void adf_afc_off(void)
{
	sys_conf.r10.afc_en = 0;
	adf_write_reg(&sys_conf.r10_reg);
}

void adf_set_tx_power(char pasetting)
{
	tx_conf.r2.power_amplifier = pasetting;
	adf_write_reg(&tx_conf.r2_reg);
}

void adf_set_rx_sync_word(unsigned long word, unsigned char len, unsigned char error_tolerance)
{
	adf_reg_t register_value;

	/* write R11, configure sync word detect */
	adf_current_syncword = word;
	register_value.whole_reg = 11;
	register_value.whole_reg |= word << 8;
	register_value.whole_reg |= error_tolerance << 6;
	register_value.whole_reg |= len << 4;
	adf_write_reg(&register_value);

	/* write R12, start sync word detect */
	adf_set_threshold_free();
}

void adf_set_threshold_free(void)
{
	/* write R12, start sync word detect */
	sys_conf.r12.packet_length = 255;
	sys_conf.r12.swd_mode = 1;
	sys_conf.r12.lock_thres_mode = 1;
	sys_conf.r12.address_bits = 12;
	adf_write_reg(&sys_conf.r12_reg);
}

void adf_set_rx_mode(void)
{
	if (adf_state == ADF_TX) {
		if (rx_conf.r3_reg.whole_reg != tx_conf.r3_reg.whole_reg)
			adf_write_reg(&rx_conf.r3_reg);
		adf_write_reg(&rx_conf.r0_reg);
	} else {
		adf_write_reg(&rx_conf.r3_reg);
		adf_write_reg(&rx_conf.r5_reg);
		adf_write_reg(&rx_conf.r0_reg);
		adf_write_reg(&rx_conf.r4_reg);
	}

	BLUEBOX_UNUSED( led_off(LED_TRANSMIT) );
	BLUEBOX_UNUSED( ptt_low(conf.ptt_delay_low) );

	adf_state = ADF_RX;
}

void adf_set_tx_mode(void)
{
	/* Turn on PA the first time we transmit */
	//xxx if (adf_pa_state == ADF_PA_OFF) {
	{
		adf_write_reg(&tx_conf.r2_reg);
		adf_pa_state = ADF_PA_ON;
	}

	BLUEBOX_UNUSED( ptt_high(conf.ptt_delay_high) );
	BLUEBOX_UNUSED( led_on(LED_TRANSMIT) );

	// xxx
	if (0 && adf_state == ADF_RX) {
		if (rx_conf.r3_reg.whole_reg != tx_conf.r3_reg.whole_reg)
			adf_write_reg(&tx_conf.r3_reg);
		adf_write_reg(&tx_conf.r0_reg);
	} else {
		adf_write_reg(&tx_conf.r3_reg);
		adf_write_reg(&tx_conf.r0_reg);
	}

	adf_state = ADF_TX;
}

unsigned int adf_readback_version(void)
{
	adf_reg_t readback = adf_read_reg(0x1C);
	return readback.word.lower;
}

signed int adf_readback_rssi(void)
{
	unsigned char gain_correction[] = { 86, 0, 0, 0, 58, 38, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	adf_reg_t readback = adf_read_reg(0x14);

	char rssi = readback.byte[0] & 0x7F;
	int gc = (readback.word.lower & 0x780) >> 7;
	double dbm = ((rssi + gain_correction[gc]) * 0.5) - 130;

	return round(dbm);
}
int adf_readback_rssi_raw(void)
{
	adf_reg_t readback = adf_read_reg(0x14);
	return readback.whole_reg;
}

int adf_readback_afc(void)
{
	adf_reg_t readback = adf_read_reg(0x10);
	return 100000 - round(readback.word.lower * ((uint32_t)XTAL_FREQ >> 18));
}


int adf_readback_temp(void)
{
	/* Enable ADC */
	adf_reg_t register_value;
	register_value.whole_reg = 8;
	register_value.whole_reg &= 1 << 8;

	adf_reg_t readback = adf_read_reg(0x16);
	//return round(-40 + ((68.4 - (readback.byte[0] & 0x7F)) * 9.32));	// Fahrenheit
	//float: return (496.5f - (7.2f * (readback.byte[0] & 0x7F)));	// Celsius
	return round(496.5f - (7.2f * (readback.byte[0] & 0x7F)));	// Celsius
}
/*int adf_readback_tempF(void)
{
	// Enable ADC
	adf_reg_t register_value;
	register_value.whole_reg = 8;
	register_value.whole_reg &= 1 << 8;

	adf_reg_t readback = adf_read_reg(0x16);
	return round(-40 + ((68.4 - (readback.byte[0] & 0x7F)) * 9.32));	// Fahrenheit
}*/

float adf_readback_voltage(void)
{
	/* Enable ADC */
	adf_reg_t register_value;
	register_value.whole_reg = 8;
	register_value.whole_reg &= 1 << 8;
	adf_write_reg(&register_value);

	adf_reg_t readback = adf_read_reg(0x15);
	return (readback.byte[0] & 0x7F) / 21.1;
}

void adf_test_tx(int mode)
{
	adf_reg_t register_value;
	register_value.whole_reg = 15;
	register_value.whole_reg |= mode << 8;
	adf_write_reg(&register_value);
}

void adf_test_off(void)
{
	adf_reg_t register_value;
	register_value.whole_reg = 15;
	adf_write_reg(&register_value);
	adf_state = ADF_RX;
}

void adf_configure(void)
{
	adf_set_rx_sync_word(conf.sw, conf.swlen, conf.swtol);
	adf_init_rx_mode(conf.bitrate, conf.modindex, conf.rx_freq, conf.if_bw);
	adf_init_tx_mode(conf.bitrate, conf.modindex, conf.tx_freq);
	adf_afc_on(conf.afc_range, conf.afc_ki, conf.afc_kp);
	adf_set_rx_mode();
}

void adf_reset(void)
{
	adf_test_off();
	adf_set_power_off();
	delay_ms(100);
	adf_set_power_on(XTAL_FREQ);
	adf_configure();
}

