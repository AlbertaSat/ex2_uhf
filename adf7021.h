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

#ifndef _ADF7021_H_
#define _ADF7021_H_

#include <stdint.h>
#include <stdbool.h>

//#include "em_device.h"
//#include "em_chip.h"

/* Board xtal frequency (MCU clock frequency???) */
//bluebox:
#define XTAL_FREQ 		16000000
//#define XTAL_FREQ 		14000000
//#define XTAL_FREQ 		1000000

// Bitfields must be left in position and not be aligned to word boundaries
#define PACKED	__attribute__ ((__packed__))
//#define CHAR	char
#define CHAR	int			// Prevent compiler warnings with char bitfield alignment for fields that span a byte boundary.

#ifdef EX2_DEVBOARD

#define ADF_PINNUM_SWD	8
#define ADF_PIN_SWD		gpioPortB, ADF_PINNUM_SWD

//#define ADF_PIN_SCLK	gpioPortD, 9
#define ADF_PIN_SCLK	gpioPortC, 11	// Moved to accommodate cable
#define ADF_PIN_SREAD	gpioPortD, 10
#define ADF_PIN_SDATA	gpioPortC, 10
#define ADF_PIN_SLE		gpioPortD, 11
#define ADF_PIN_MUXOUT	gpioPortD, 12
#define ADF_PIN_CE		gpioPortD, 8


#elif defined(BBSTANDARD)
// Example configuration for AVR chips
#define ADF_PORT_SWD		PORTE
#define ADF_PORT_IN_SWD		PINE
#define ADF_PORT_DIR_SWD	DDRE
#define ADF_SWD 		6

#define ADF_PORT_SCLK		PORTC
#define ADF_PORT_IN_SCLK	PINC
#define ADF_PORT_DIR_SCLK	DDRC
#define ADF_SCLK 		7

#define ADF_PORT_SREAD		PORTC
#define ADF_PORT_IN_SREAD	PINC
#define ADF_PORT_DIR_SREAD	DDRC
#define ADF_SREAD 		6

#define ADF_PORT_SDATA		PORTB
#define ADF_PORT_IN_SDATA	PINB
#define ADF_PORT_DIR_SDATA	DDRB
#define ADF_SDATA 		6

#define ADF_PORT_SLE		PORTB
#define ADF_PORT_IN_SLE		PINB
#define ADF_PORT_DIR_SLE	DDRB
#define ADF_SLE 		5

#define ADF_PORT_MUXOUT		PORTD
#define ADF_PORT_IN_MUXOUT	PIND
#define ADF_PORT_DIR_MUXOUT	DDRD
#define ADF_MUXOUT 		6

#define ADF_PORT_CE		PORTD
#define ADF_PORT_IN_CE		PIND
#define ADF_PORT_DIR_CE		DDRD
#define ADF_CE 			7
#endif //BBSTANDARD

typedef union
{
	unsigned long whole_reg;
	struct {
		unsigned int lower;
		unsigned int upper;
	} word;
	unsigned char byte[4];
} adf_reg_t;

typedef struct {
	struct {
		double data_rate;
		double mod_index;
		unsigned long freq;
	} desired;
	struct {
		double data_rate;
		double mod_index;
		unsigned long freq;
		unsigned int freq_dev;
	} real;
	union {
		adf_reg_t r0_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned int  frac_n : 15;
			unsigned int  int_n : 8;
			unsigned CHAR rx_on : 1;
			unsigned CHAR uart_mode : 1;
			unsigned CHAR muxout : 3;
		} PACKED r0;
	};
	union {
		adf_reg_t r2_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned CHAR modulation_scheme : 3;
			unsigned CHAR pa_enable : 1;
			unsigned CHAR pa_ramp : 3;
			unsigned CHAR pa_bias : 2;
			unsigned CHAR power_amplifier : 6;
			unsigned int  tx_frequency_deviation : 9;
			unsigned CHAR tx_data_invert : 2;
			unsigned CHAR rcosine_alpha : 1;
		} PACKED r2;
	};
	union {
		adf_reg_t r3_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned CHAR bbos_clk_divide: 2;
			unsigned CHAR dem_clk_divide : 4;
			unsigned CHAR cdr_clk_divide : 8;
			unsigned CHAR seq_clk_divide : 8;
			unsigned CHAR agc_clk_divide : 6;
		} PACKED r3;
	};
	union {
		adf_reg_t r4_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned CHAR demod_scheme : 3;
			unsigned CHAR dot_product : 1;
			unsigned CHAR rx_invert : 2;
			unsigned int  disc_bw : 10;
			unsigned int  post_demod_bw : 10;
			unsigned CHAR if_bw : 2;
		} PACKED r4;
	};
	union {
		adf_reg_t r5_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned CHAR if_cal_coarse : 1;
			unsigned int  if_filter_divider : 9;
			unsigned CHAR if_filter_adjust : 6;
			unsigned CHAR ir_phase_adjust_mag : 4;
			unsigned CHAR ir_phase_adjust_direction : 1;
			unsigned CHAR ir_gain_adjust_mac : 5;
			unsigned CHAR ir_gain_adjust_iq : 1;
			unsigned CHAR ir_gain_adjust_updn : 1;
		} PACKED r5;
	};
	union {
		adf_reg_t r6_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned CHAR if_fine_cal : 1;
			unsigned CHAR if_cal_lower_tone : 8;
			unsigned CHAR if_cal_upper_tone : 8;
			unsigned CHAR if_cal_dwell_time : 7;
			unsigned CHAR ir_cal_source_drive : 2;
			unsigned CHAR ir_cal_source_div_2 : 1;
		} PACKED r6;
	};
} adf_conf_t;

typedef struct {
	unsigned long adf_xtal;
	union {
		adf_reg_t r1_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned CHAR r_counter : 3;
			unsigned CHAR clockout_divide : 4;
			unsigned CHAR xtal_doubler : 1;
			unsigned CHAR xosc_enable : 1;
			unsigned CHAR xtal_bias : 2;
			unsigned CHAR cp_current : 2;
			unsigned CHAR vco_enable : 1;
			unsigned CHAR rf_divide_by_2 : 1;
			unsigned CHAR vco_bias : 4;
			unsigned CHAR vco_adjust : 2;
			unsigned CHAR vco_inductor : 1;
		} PACKED r1;
	};
	union {
		adf_reg_t r10_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned CHAR afc_en : 1;
			unsigned int  afc_scaling_factor : 12;
			unsigned CHAR ki : 4;
			unsigned CHAR kp : 3;
			unsigned CHAR afc_range : 8;
		} PACKED r10;
	};
	union {
		adf_reg_t r12_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned CHAR lock_thres_mode : 2;
			unsigned CHAR swd_mode : 2;
			unsigned CHAR packet_length : 8;
		} PACKED r12;
	};
	union {
		adf_reg_t r14_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned CHAR test_tdac_en : 1;
			unsigned int test_dac_offset : 16;
			unsigned CHAR test_dac_gain : 4;
			unsigned CHAR pulse_ext : 2;
			unsigned CHAR leak_factor : 3;
			unsigned CHAR ed_peak_resp : 2;
		} PACKED r14;
	};
	union {
		adf_reg_t r15_reg;
		struct {
			unsigned CHAR address_bits : 4;
			unsigned CHAR rx_test_mode : 4;
			unsigned CHAR tx_test_mode : 3;
			unsigned CHAR sd_test_mode : 3;
			unsigned CHAR cp_test_mode : 3;
			unsigned CHAR clk_mux : 3;
			unsigned CHAR pll_test_mode : 4;
			unsigned CHAR analog_test_mode : 4;
			unsigned CHAR force_ld_high : 1;
			unsigned CHAR reg1_pd : 1;
			unsigned CHAR cal_override : 2;
		} PACKED r15;
	};
} adf_sysconf_t;


void adf_write_reg(adf_reg_t *reg);
adf_reg_t adf_read_reg(unsigned int readback_config);

void adf_set_power_on(unsigned long adf_xtal);
void adf_set_power_off(void);

void adf_init_rx_mode(unsigned int data_rate, uint8_t mod_index, unsigned long freq, uint8_t if_bw);
void adf_init_tx_mode(unsigned int data_rate, uint8_t mod_index, unsigned long freq);
void adf_set_rx_mode(void);
void adf_set_tx_mode(void);

void adf_afc_on(unsigned char range, unsigned char ki, unsigned char kp);
void adf_afc_off(void);
void adf_set_tx_power(char pasetting);

#define ADF_SYNC_WORD_LEN_12	        0
#define ADF_SYNC_WORD_LEN_16	        1
#define ADF_SYNC_WORD_LEN_20	        2
#define ADF_SYNC_WORD_LEN_24	        3

#define ADF_SYNC_WORD_ERROR_TOLERANCE_0	0
#define ADF_SYNC_WORD_ERROR_TOLERANCE_1	1
#define ADF_SYNC_WORD_ERROR_TOLERANCE_2	2
#define ADF_SYNC_WORD_ERROR_TOLERANCE_3	3

void adf_set_rx_sync_word(unsigned long word, unsigned char len, unsigned char error_tolerance);
void adf_set_threshold_free(void);

#define ADF_TEST_TX_MODE_CARRIER_ONLY	1
#define ADF_TEST_TX_MODE_TONE_HIGH	2
#define ADF_TEST_TX_MODE_TONE_LOW	3
#define ADF_TEST_TX_MODE_PATTERN_1010	4
#define ADF_TEST_TX_MODE_PATTERN_PN9	5
#define ADF_TEST_TX_MODE_SYNC_BYTE	6

void adf_test_tx(int mode);
void adf_test_off(void);

unsigned int adf_readback_version(void);
signed int adf_readback_rssi(void);
int adf_readback_rssi_raw(void);
signed int adf_readback_afc(void);

// Returns temperature reading in Celsius (resolution no better than 7.2 degrees)
int adf_readback_temp(void);
float adf_readback_voltage(void);
void adf_configure(void);
void adf_reset(void);

#endif /* _ADF7021_H_ */
