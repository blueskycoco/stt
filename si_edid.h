//***************************************************************************
//!file     si_edid.h
//!brief    Silicon Image EDID functions header.
//
// No part of this work may be reproduced, modified, distributed, 
// transmitted, transcribed, or translated into any language or computer 
// format, in any form or by any means without written permission of 
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_EDID_H__
#define __SI_EDID_H__

#include <si_datatypes.h>
#include <string.h>         // For memcpy

//------------------------------------------------------------------------------
//  EDID Manifest Constants
//------------------------------------------------------------------------------

#define EDID_TABLE_LEN          256
#define EDID_VGA_TABLE_LEN      128
#define EDID_DEVBOOT_LEN        64

enum EDIDType
{
    EDID_RAM_0 = 0,
    EDID_RAM_1 = 1,
    EDID_RAM_2 = 2,
    EDID_RAM_3 = 3,
    EDID_RAM_4 = 4,
    EDID_RAM_VGA = 5,
    EDID_RAM_BOOT,
    EDID_NVRAM,
    EDID_NVRAM_BOOT,
    EDID_NVRAM_VGA,
    EDID_BUFFER
};

//------------------------------------------------------------------------------
//
//  EDID typedefs
//
//------------------------------------------------------------------------------

typedef struct
{
    uint8_t     nvm_config;
    uint8_t     edid_valid;
    uint8_t     nvm_version;
    uint8_t     edid_copy_dest;
    uint8_t     hpd_hw_ctrl;
    uint8_t     ddc_filter_sel;
    uint8_t     wakeup_source;
    uint8_t     spare1[3];
    uint8_t     cec_pa_addr;
    uint8_t     spare2;
    uint8_t     cec_pad_h_ch0;
    uint8_t     cec_pad_l_ch0;
    uint8_t     cec_pad_h_ch1;
    uint8_t     cec_pad_l_ch1;
    uint8_t     cec_pad_h_ch2;
    uint8_t     cec_pad_l_ch2;
    uint8_t     cec_pad_h_ch3;
    uint8_t     cec_pad_l_ch3;
    uint8_t     cec_pad_h_ch4;
    uint8_t     cec_pad_l_ch4;
    uint8_t     spare3[6];
    uint8_t     checksum_ch0;
    uint8_t     checksum_ch1;
    uint8_t     checksum_ch2;
    uint8_t     checksum_ch3;
    uint8_t     checksum_ch4;
    uint8_t     spare4[31];

} SI_DeviceBootData_t;

//------------------------------------------------------------------------------
//
//  EDID Data
//
//------------------------------------------------------------------------------

extern @near const uint8_t g_edidFlashEdidTable [ EDID_TABLE_LEN ];
extern @near const uint8_t g_edidFlashEdidVgaTable [ EDID_VGA_TABLE_LEN ];
extern @near SI_DeviceBootData_t  g_edidFlashDevBootData;
extern @near const uint8_t g_edidFPGAEdidTable [ EDID_TABLE_LEN ];

//------------------------------------------------------------------------------
//
//  Internal use EDID access prototypes
//
//------------------------------------------------------------------------------

void si_EdidReadPortRam(int index, uint8_t ramIndex, uint8_t sourceOffset, uint8_t *pDest, uint16_t length );
void si_EdidReadBootRam(int index, uint8_t *pDest, uint8_t sourceOffset, uint8_t length );
void si_EdidWritePortRam(int index, uint8_t ramIndex, uint8_t *pSource, uint8_t sourceOffset, uint16_t length );
void si_EdidWriteBootRam(int index, uint8_t *pSource, uint8_t sourceOffset, uint8_t length );

BOOL si_NvramCommand(int index, uint8_t command );

//------------------------------------------------------------------------------
//
//  EDID Function Prototypes
//
//------------------------------------------------------------------------------

BOOL SI_EdidRead(int index, uint8_t source, uint8_t *pDest);
BOOL SI_EdidWrite(int index, uint8_t target, uint8_t *pSource );
BOOL SI_EdidLoadPortRam(int index, uint8_t source, uint8_t port );
uint8_t SI_EdidUpdateHdmiData (uint8_t *pData, uint8_t port );
BOOL SI_EdidValidHdmi(uint8_t *pData );
BOOL SI_EdidInitialize(int index);

#endif  // __SI_EDID_H__

