//***************************************************************************
//!file     si_regio.h
//!brief    Silicon Image Device register I/O header.
//
// No part of this work may be reproduced, modified, distributed, 
// transmitted, transcribed, or translated into any language or computer 
// format, in any form or by any means without written permission of 
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_datatypes.h"
#include "si_9287regs.h"

#define REGIO_SLAVE_PAGE_0          (0xB0)
#define REGIO_SLAVE_PAGE_1          (0x68)
#define REGIO_SLAVE_PAGE_2          (0xB0)
#define REGIO_SLAVE_PAGE_3          (0xB0)
#define REGIO_SLAVE_PAGE_4          (0xB0)
#define REGIO_SLAVE_PAGE_5          (0xB0)
#define REGIO_SLAVE_PAGE_6          (0xB0)
#define REGIO_SLAVE_PAGE_7          (0xB0)
#define REGIO_SLAVE_PAGE_8          (0xC0)      // CEC
#define REGIO_SLAVE_PAGE_9          (0xE0)      // EDID
#define REGIO_SLAVE_PAGE_A          (0x64)      // xvYCC
#define REGIO_SLAVE_PAGE_B			(0x90)      // RPI
#define REGIO_SLAVE_PAGE_C          (0xE6)      // CBUS
#define REGIO_SLAVE_PAGE_D          (0xB0)
#define REGIO_SLAVE_PAGE_E          (0xB0)
#define REGIO_SLAVE_PAGE_F          (0xB2)

//-------------------------------------------------------------------------------
//  Silicon Image Device Register I/O Function Prototypes
//-------------------------------------------------------------------------------

uint8_t SiIRegioRead(int index, uint16_t regAddr );
void    SiIRegioWrite(int index, uint16_t regAddr, uint8_t value);
void    SiIRegioModify(int index, uint16_t regAddr, uint8_t mask, uint8_t value);
void    SiIRegioBitToggle(int index, uint16_t regAddr, uint8_t mask);
void    SiIRegioReadBlock( uint16_t regAddr, uint8_t* buffer, uint16_t length);
void    SiIRegioWriteBlock( uint16_t regAddr, uint8_t* buffer, uint16_t length);

void    SiIRegioSetBase(int index, uint8_t page, uint8_t newID );

