;-------------------------------------------------------------------------------
;  sys_intvecs.asm
;  Author      : QJ Wang. qjwang@ti.com
;  Date        : 9-19-2012
;
;  Copyright (c) 2008-2011 Texas Instruments Incorporated.  All rights reserved.
;  Software License Agreement
;
;  Texas Instruments (TI) is supplying this software for use solely and
;  exclusively on TI's microcontroller products. The software is owned by
;  TI and/or its suppliers, and is protected under applicable copyright
;  laws. You may not combine this software with "viral" open-source
;  software in order to form a larger program.
;
;  THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
;  NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
;  NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;  A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
;  CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL;
;  DAMAGES, FOR ANY REASON WHATSOEVER.
;
;*****************************************************************************
    .sect ".intvecs"


;-------------------------------------------------------------------------------
; import reference for interrupt routines

    .ref _c_int00

;-------------------------------------------------------------------------------
; interrupt vectors
; Please change the #0x???? for your specified image location defined in bl_config.h

        b   _c_int00               ;0x00
        b   #0x1FFF8               ;0x04
        b   #0x1FFF8               ;0x08, Software interrupt
        b   #0x1FFF8               ;0x0C, Abort (prefetch)
        b   #0x1FFF8               ;0x10, Abort (data)
reservedEntry
        b   reservedEntry          ;0x14
        ldr pc,[pc, #-0x1b0]       ;0x18
        ldr pc,[pc, #-0x1b0]       ;0x1C

