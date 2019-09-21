//*****************************************************************************
//
// hw_pinmux.c
// Author      : QJ Wang. qjwang@ti.com
// Date        : 9-19-2012
//
// Copyright (c) 2006-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************


#include "hw_pinmux.h"

// IOMM_BASE = 0xFFFFEA00

void Gladiator_IOMM_UNLOCK()
{
	/*Unlock the IOMM Register*/
	*(int *) 0xFFFFEA38  = 0x83E70B13;  /* kicker 0 register, unlock CPU write access to PINMMR registers */
    *(int *) 0xFFFFEA3C  = 0x95A4F1E0;  /* kicker 1 register, */
}

void Gladiator_IOMM_LOCK()
{
	/*lock the IOMM Register*/
	*(int *) 0xFFFFEA38  = 0x00000000;  /* kicker 0 register, lock CPU write access to PINMMR registers */
    *(int *) 0xFFFFEA3C  = 0x00000000;  /* kicker 1 register, */
}

void Gladiator_PINMUX_DEFAULT()
{
	 *(int *) (0xFFFFEA00 + 0x110)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x114)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x118)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x11C)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x120)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x124)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x128)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x12C)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x130)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x134)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x138)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x13C)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x140)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x144)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x148)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x14C)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x150)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x154)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x158)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x15C)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x160)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x164)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x168)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x16C)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x170)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x174)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x178)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x17C)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x180)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x184)  = 0x01010101;
	 *(int *) (0xFFFFEA00 + 0x188)  = 0x01010101;
}

void Gladiator_PINMUX_AD2EVT()
{
 *(int *) 0xFFFFEB34  = 0x01020101;//P9   // -- ADM2EVT
 *(int *) 0xFFFFEB84  = 0x01000101;//P29 // Disable GIOB[2]
}


void Gladiator_PINMUX_AD2_ALT_TRIGGER_SOURCE()
{
 *(int *) 0xFFFFEB88 = 0x00000002;//P30  // Alternate MIBADC Trigger Hookup
}

void Gladiator_PINMUX_MDIO()
{
	*(int *) 0xFFFFEB2C  = 0x00000400; 
	*(int *) 0xFFFFEB30  = 0x00000400; 
}

void Gladiator_PINMUX_EMAC_RMII()
{
	*(int *) 0xFFFFEB38  = 0x02010204;//P10  //RMIIRXER
	*(int *) 0xFFFFEB3C  = 0x08020101;//P11  //RMII_RXD0
	*(int *) 0xFFFFEB40  = 0x01010204;//P12  //RMII RXD1
	*(int *) 0xFFFFEB54  = 0x02040200;//P17  //RMII_RMCRSDV
	*(int *) 0xFFFFEB44  = 0x01080808;//P13  //RMII_TXEN , RMII_TX_D1 ,RMII_TX_D0
	*(int *) 0xFFFFEB48  = 0x01010401;//P14; //RMII_REFCLK
}

void Gladiator_PINMUX_EMAC_MII()
{
	*(int *) 0xFFFFEB38  &= 0xFFFFFF00; //P10[1]  //Mux 10 Rx_ER
	*(int *) 0xFFFFEB38  |= (1 << 1);   //P10[1]  //Mux 10 Rx_ER
	
	*(int *) 0xFFFFEB3C  &= 0x00FFFFFF; //P11[26]   //Mux 11 Rx[0]
	*(int *) 0xFFFFEB3C  |= (1 << 26);  //P11[26]   //Mux 11 Rx[0]

	*(int *) 0xFFFFEB40  &= 0x0000FF00;//P12[1,18,26]    //Mux 12 Rx[3],Rx[2],Rx[1]
	*(int *) 0xFFFFEB40  |= ((1<<26) | (1<<18) | (1<<1));//P12[1,18,26]    //Mux 12 Rx[3],Rx[2],Rx[1]

	*(int *) 0xFFFFEB44  &= 0x00000000;//P13[2, 10, 26,18]   //Mux 13 Tx[2],TxEn,Tx[1],Tx[0]
	*(int *) 0xFFFFEB44  |= ((1<<26)|(1<<18)|(1<<10)|(1<<2)); //P13[2, 10, 26,18]   //Mux 13 Tx[2],TxEn,Tx[1],Tx[0]

	*(int *) 0xFFFFEB48  &= 0xFFFF0000; //P14[9,2,11]   //Mux 14 Tx[3],RxClk
	*(int *) 0xFFFFEB48  |= ((1<<9)|(1<<2));    //P14[9,2]   //Mux 14 Tx[3],RxClk

	*(int *) 0xFFFFEB54  &= 0xFF00FF00      ;//P17[17,1,3]   //Mux 17 CRS,TxClk
	*(int *) 0xFFFFEB54  |= ((1<<17)|(1<<1));          //P17[17,1]   //Mux 17 CRS,TxClk

	*(int *) 0xFFFFEB5C  &= 0xFFFF00FF;  //P19[9]   //Mux 19 RxDV
	*(int *) 0xFFFFEB5C  |= (1<<9);      //P19[9]   //Mux 19 RxDV

	*(int *) 0xFFFFEB60  &= 0xFF00FFFF;  //P20[18]   //Mux 20 COL
	*(int *) 0xFFFFEB60  |= (1<<18);     //P20[18]   //Mux 20 COL

	*(int *) 0xFFFFEB84  &= 0x00FFFFFF;//P29[24]  //Mux 29 MII Select pin (24 bit - 0(MII),1(RMII))
	*(int *) 0xFFFFEB84  |= (0<<24);   //P29[24]  //Mux 29 MII Select pin (24 bit - 0(MII),1(RMII))
}


void Gladiator_PINMUX_EMIF()
{
	*(int *) 0xFFFFEB14  = 0x02020101;//P1  //EMIF_DATA[5],EMIF_DATA[4]
	*(int *) 0xFFFFEB18  = 0x01010201;//P2  //EMIF_DATA[6]
	*(int *) 0xFFFFEB1C  = 0x02010102;//P3  //EMIF_DATA[8],EMIF_DATA[7]
	*(int *) 0xFFFFEB20  = 0x01010201;//P4  //EMIF_DATA[9]
	*(int *) 0xFFFFEB24  = 0x02010101;//P5  //EMIF_DATA[10]
	*(int *) 0xFFFFEB28  = 0x02010201;//P6  //EMIF_DATA[12],EMIF_DATA[11]
	*(int *) 0xFFFFEB2C  = 0x02010102;//P7  //EMIF_DATA[14],EMIF_DATA[13]
	*(int *) 0xFFFFEB30  = 0x02010101;//P8  //EMIF_DATA[15]
	*(int *) 0xFFFFEB38  = 0x02010201;//P10 //EMIF_DATA[2],EMIF_DATA[3]
	*(int *) 0xFFFFEB3C  = 0x01020101;//P11 //EMIF_DATA[1]
	*(int *) 0xFFFFEB40  = 0x01010201;//P12 //EMIF_DATA[0]
	*(int *) 0xFFFFEB50  = 0x02010102;//P16 //EMIF_nOE,EMIF_BA[0]
	*(int *) 0xFFFFEB54  = 0x02010201;//P17 //EMIF_ADDR[5],EMIF_nDQM[1]
	*(int *) 0xFFFFEB5C  = 0x02020102;//P19 //EMIF_ADDR[3],EMIF_nDQM[0],EMIF_ADDR[4]
	*(int *) 0xFFFFEB68  = 0x02010101;//P22	//EMIF_ADDR[2]
	*(int *) 0xFFFFEB84  = 0x01010001;//P29	//EMIF_CLK
 
}


void Gladiator_PINMUX_RTP_DMM()
{
// Added for RTP[0-15],RTP Sync,RTP Ena,RTP Clk 
	*(int *) 0xFFFFEB38  = 0x01020101; //P10 //RTP_DATA[15]
	*(int *) 0xFFFFEB3C  = 0x01010202; //P11 //RTP_DATA[7],RTP_DATA[14]
	*(int *) 0xFFFFEB4C  = 0x02020202; //P15 //RTP_DATA[0],RTP_nENA,RTP_SYNC,RTP_CLK
	*(int *) 0xFFFFEB50  = 0x01020201; //P16 //RTP_DATA[2],RTP_DATA[1]
	*(int *) 0xFFFFEB58  = 0x01020102; //P18 //RTP_DATA[4],RTP_DATA[3]
	*(int *) 0xFFFFEB60  = 0x02010202; //P20 //RTP_DATA[8],RTP_DATA[6],RTP_DATA[5]
	*(int *) 0xFFFFEB64  = 0x02020101; //P21 //RTP_DATA[10],RTP_DATA[9]
	*(int *) 0xFFFFEB68  = 0x01020201; //P22 //RTP_DATA[13],RTP_DATA[12]
	*(int *) 0xFFFFEB6C  = 0x01010102; //P23 //RTP_DATA[11]

// Added for DMM[2-15] other DMM pins are default
	*(int *) 0xFFFFEB40  = 0x02010101; //P12 //DMM_DATA[7]
	*(int *) 0xFFFFEB44  = 0x01020202; //P13 //DMM_DATA[4],DMM_DATA[8],DMM_DATA[12]
	*(int *) 0xFFFFEB78  = 0x02020101; //P26 //DMM_DATA[2],DMM_DATA[3]
	*(int *) 0xFFFFEB7C  = 0x02020202; //P27 //DMM_DATA[10],DMM_DATA[9],DMM_DATA[6],DMM_DATA[5]
	*(int *) 0xFFFFEB80  = 0x02020202; //P28 //DMM_DATA[15],DMM_DATA[14],DMM_DATA[13],DMM_DATA[11]

}

void Gladiator_PINMUX_SCI2()
{
	*(int *) 0xFFFFEB30 = 0x01010102;//P8  //SCITX
	*(int *) 0xFFFFEB2C = 0x01020101;//P7  //SCIRX
}


void Gladiator_PINMUX_NHET2()
{
   *(int *) 0xFFFFEB14  = 0x01010401;//p1 //NHET2[18]
   *(int *) 0xFFFFEB18  = 0x01020108;//p2 //NHET2[2],NHET2[0]
   *(int *) 0xFFFFEB1C  = 0x01020101;//p3 //NHET2[4]
   *(int *) 0xFFFFEB20  = 0x10100102;//p4 //NHET2[10],NHET2[8],NHET2[6]
   *(int *) 0xFFFFEB24  = 0x01040101;//p5 //NHET2[12]
   *(int *) 0xFFFFEB28  = 0x01020108;//p6 //NHET2[16],NHET2[3]
   *(int *) 0xFFFFEB38  = 0x01040101;//p10 //NHET2[7]
   *(int *) 0xFFFFEB3C  = 0x01010104;//p11 //NHET2[9]
   *(int *) 0xFFFFEB48  = 0x02010101;//p14 //NHET2[5] 
   *(int *) 0xFFFFEB64  = 0x01010102;//p21 //NHET2[3]
   *(int *) 0xFFFFEB68  = 0x01040402;//p22 //NHET2[11],NHET2[13],NHET2[1]
   *(int *) 0xFFFFEB6C  = 0x01010104;//p23 //NHET2[15]
}


void Gladiator_PINMUX_USB()
{
	/* write to IOMM register to demux USB Slave pins */
	*(int *) 0xFFFFEB10 = 0x01010404;//P0  //W2FC_RXDPI,W2FC_RXDI 
	*(int *) 0xFFFFEB14 = 0x01011004;//P1  //W2FC_VBUSI,W2FC_RXDMI	
	*(int *) 0xFFFFEB18 = 0x00000004;//P2  //W2FC_TXDO  
	*(int *) 0xFFFFEB1C = 0x00000400;//P3  //W2FC_SE0O
	*(int *) 0xFFFFEB20 = 0x08080000;//P4  //W2FC_PUENON,W2FC_PUENO
	*(int *) 0xFFFFEB28 = 0x00080004;//P6  //W2FC_SUSPENDO,W2FC_GZO

}


void Gladiator_PINMUX_OHCI_PORT_0()
{
	*(int *) 0xFFFFEB40 = 0x08080101;//P12 //OHCI_PRT_RcvDmns[0],OHCI_PRT_PcvDpls[0]
	*(int *) 0xFFFFEB44 = 0x08010101;//P13 //OHCI_PRT_RcvData[0]	
	*(int *) 0xFFFFEB48 = 0x01010108;//P14 //OHCI_PRT_OvrCurrent[0]	
	*(int *) 0xFFFFEB54 = 0x01010104;//P17 //OHCI_RCFG_txEnL[0]	
	*(int *) 0xFFFFEB58 = 0x02010201;//P18 //OHCI_RCFG_txDpls[0],OHCI_RCFG_txSe0[0]	
	*(int *) 0xFFFFEB5C = 0x01010401;//P19 //OHCI_RCFG_speed[0]	
	*(int *) 0xFFFFEB60 = 0x01080101;//P20 //OHCI_RCFG_suspend[0]	
	*(int *) 0xFFFFEB64 = 0x01010201;//P21 //OHCI_RCFG_PrtPower[0]
}

void Gladiator_PINMUX_OHCI_PORT_1()
{
	*(int *) 0xFFFFEB10 = 0x01010202;//P0 //OHCI_PRT_RcvDpls[1],OHCI_PRT_RcvData[1]
	*(int *) 0xFFFFEB14 = 0x01010802;//P1 //OHCI_PRT_OvrCurrent[1],OHCI_PRT_RcvDmns[1]	
	*(int *) 0xFFFFEB18 = 0x01010102;//P2 //OHCI_RCFG_txDpls[1]	
	*(int *) 0xFFFFEB1C = 0x01010201;//P3 //OHCI_RCFG_txSe0[1]	
	*(int *) 0xFFFFEB20 = 0x04040101;//P4 //OHCI_RCFG_speed[1],OHCI_RCFG_txEnL[1]	
	*(int *) 0xFFFFEB28 = 0x01040102;//P6 //OHCI_RCFG_suspend[1],OHCI_RCFG_PrtPower[1]	

}

void Gladiator_PINMUX_I2C()
{
 *(int *) 0xFFFFEB10  = 0x02020101; // I2C_SDA, I2C_SCL

}

void Gladiator_PINMUX_MIBSPI1_MUX_PINS()
{
 *(int *) 0xFFFFEB44  = 0x00010101;   //P13   //CS0
 *(int *) 0xFFFFEB60  = 0xFF01FFFF;   //P20   //CS1, 16th
 *(int *) 0xFFFFEB30  = 0x01020101;   //P8    //CS2, 8th; CS4, 16th
 *(int *) 0xFFFFEB34  = 0x01010201;   //P9    //CS3, 24th
 *(int *) 0xFFFFEB40  = 0x01010101;   //P12   //ENA, 16th
}

void Gladiator_PINMUX_SPI2_MUX_PINS()
{
 *(int *) 0xFFFFEB84  = 0x01010102; //P29 //SPI2NCS[1]

}

void Gladiator_PINMUX_MIBSPI3_MUX_PINS()
{
 *(int *) 0xFFFFEB34  = 0x01010201; //P9 //MIBSPI3NCS[5]
 *(int *) 0xFFFFEB14  = 0x01010201; //P1 //MIBSPI3NCS[4]

}

void Gladiator_PINMUX_SPI4_MUX_PINS()
{
/*Default pins also available*/
 *(int *) 0xFFFFEB24  = 0x01020202; //P5 //SPI4SOMI,SPI4SIMO,SPI4CLK
 *(int *) 0xFFFFEB20  = 0x02020101; //P4 //SPI4NCS[0],SPI4NENA
 *(int *) 0xFFFFEB6C  = 0x01010101; //P23 //INPUT Control
}

void Gladiator_PINMUX_MIBSPI5_MUX_PINS()
{
	*(int *) 0xFFFFEB40  = 0x01010101; //P12 //ENA (24th bit)
	*(int *) 0xFFFFEB44  = 0x01010101; //P13 //SIMO[0]; SOMI[0], CLK
	*(int *) 0xFFFFEB78  = 0x01010101; //P26 //CS[2,3]
	*(int *) 0xFFFFEB7C  = 0x01010101; //P27 //CS[0,1], SIMO1/2
	*(int *) 0xFFFFEB80  = 0x01010101; //P28 //SIMO3, SOMI1/2/3

}


