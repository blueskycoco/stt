//***************************************************************************
//!file     si_basic9287.c
//!brief    9287 Basic Firmware main module.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <si_cp9287.h>
#include <si_regio.h>
#include <si_api9x87.h>
#ifdef SII_9289
    #include <si_LedDriver.h>
#endif

//------------------------------------------------------------------------------
// Module data
//------------------------------------------------------------------------------

#if (FPGA_BUILD == 1)
char g_signonMsg [] = "CP 9287 Basic Firmware v2.1.11 (FPGA)";
#else
char g_signonMsg [] = "CP 9287 Basic Firmware v2.1.11";
#endif

char g_buildStr [] = "Build:  __DATE2__ - __TIME__ \n";

void process(int index)
{
	uint8_t 		monitorState;
	static uint8_t	oldMute 			= 0x00;
	oldMute 			= true;

	/* Call SI_DeviceEventMonitor API to process 9287 events,   */
	/* and mute or unmute as required.                          */

	monitorState = SI_DeviceEventMonitor(index);
	if ( oldMute != (monitorState & DEM_MUTE_ACTIVE) )
	{
		SI_DeviceMute(index, (monitorState & DEM_MUTE_ACTIVE) );
		oldMute = (monitorState & DEM_MUTE_ACTIVE);
	}
}
//------------------------------------------------------------------------------
// Function:    main
// Description: 9287 startup and supervisor control loop
//------------------------------------------------------------------------------

void main ( void )
{
    uint8_t         monitorState;
    static uint8_t  oldMute             = 0x00;
    BOOL            externalAccess;
    uint8_t         u8Data, rotarySwitch;

    externalAccess      = false;
    oldMute             = true;

    HalInitialize();
    HalTimerInit();
    HalUartInit();
	HalTimerWait(100);
   GPIOC->DDR |=  0x30;            
   GPIOC->CR1 |=  0x30;
   GPIOC->ODR |= 0x30;
    /* Perform a hard reset on the device to ensure that it is in a known   */
    /* state (also downloads a fresh copy of EDID from NVRAM).              */

    DEBUG_PRINT(MSG_ALWAYS,("\nPower up Initialize..."));
    u8Data = SI_DevicePowerUpBoot(0);
    if ( u8Data <= 0x02 )
    {
        DEBUG_PRINT( MSG_ALWAYS, ( "0 FAILED - " ));
    }
    DEBUG_PRINT(MSG_ALWAYS,("\n0 Base Address: %02X  BSM Status: %02X\n", (int)u8Data, (int)SiIRegioRead(0, REG_BSM_STAT )));
    /*u8Data = SI_DevicePowerUpBoot(1);
    if ( u8Data <= 0x02 )
    {
        DEBUG_PRINT( MSG_ALWAYS, ( "1 FAILED - " ));
    }
    DEBUG_PRINT(MSG_ALWAYS,("\n1 Base Address: %02X  BSM Status: %02X\n", (int)u8Data, (int)SiIRegioRead(1, REG_BSM_STAT )));
*/

    CpDisplayChipInfo(0);
	//CpDisplayChipInfo(1);
    //HalTimerSet( TIMER_POLLING, DEM_POLLING_DELAY );
    while ( 1 )
    {
   		HalTimerWait(100);
       process(0);
	  // process(1);
    }
}

