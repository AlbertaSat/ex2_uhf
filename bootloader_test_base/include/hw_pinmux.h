/* ------------------------------------------------------------------------- */
/* pinmux.h : 				     	     				 */
/*                                                                           */
/* Copyright (c) Texas Instruments 1997, All right reserved.                 */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#ifndef __PINMUX_H__
#define __PINMUX_H__

	void Gladiator_IOMM_UNLOCK(void);
    void Gladiator_IOMM_LOCK();	
    void Gladiator_PINMUX_DEFAULT();
	void Gladiator_PINMUX_AD2EVT(void);
	void Gladiator_PINMUX_AD2_ALT_TRIGGER_SOURCE(void);
	void Gladiator_PINMUX_MDIO(void);
	void Gladiator_PINMUX_EMAC_RMII(void);
	void Gladiator_PINMUX_EMAC_MII(void);
	void Gladiator_PINMUX_EMIF(void);
	void Gladiator_PINMUX_RTP_DMM(void);
	void Gladiator_PINMUX_SCI2(void);
	void Gladiator_PINMUX_NHET2(void);
	void Gladiator_PINMUX_USB(void);
	void Gladiator_PINMUX_OHCI_PORT_0(void);
	void Gladiator_PINMUX_OHCI_PORT_1(void);
	void Gladiator_PINMUX_I2C(void);
	void Gladiator_PINMUX_MIBSPI1_MUX_PINS(void);
	void Gladiator_PINMUX_SPI2_MUX_PINS(void);
	void Gladiator_PINMUX_MIBSPI3_MUX_PINS(void);
	void Gladiator_PINMUX_SPI4_MUX_PINS(void);
    void Gladiator_PINMUX_MIBSPI5_MUX_PINS();	

#endif

