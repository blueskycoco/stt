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

#ifdef SII_9289
    ledInit();
#endif
    //EA  = 1;                // Enable MCU global interrupts (HAL Timer, UART, IR, EX0)

    DEBUG_PRINT( MSG_ALWAYS,( "\n\n%s - %s\n", g_signonMsg, g_strDeviceID ));
    DEBUG_PRINT( MSG_ALWAYS,( g_buildStr ));

#if (FPGA_BUILD == 1)
    {
        char  i;
        DEBUG_PRINT( MSG_ALWAYS, ("\n" ));
        for ( i = 6; i >= 0; i-- )
        {
            DEBUG_PRINT( MSG_ALWAYS, ("%d...", (int)i ));
            HalTimerWait( 1000 );
        }
        DEBUG_PRINT( MSG_ALWAYS, ("\n" ));
    }

    DEBUG_PRINT( MSG_ALWAYS, ("\nFPGA HAL Version: %d.%02d\n", (int)HalVersionFPGA( 0 ), (int)HalVersionFPGA( 1 )));
#else
    DEBUG_PRINT( MSG_ALWAYS, ("\nHAL Version: %d.%02d\n", (int)HalVersion( 0 ), (int)HalVersion( 1 )));
#endif

    /* Perform a hard reset on the device to ensure that it is in a known   */
    /* state (also downloads a fresh copy of EDID from NVRAM).              */

    DEBUG_PRINT(MSG_ALWAYS,("\nPower up Initialize..."));
    u8Data = SI_DevicePowerUpBoot();
    if ( u8Data <= 0x02 )
    {
        DEBUG_PRINT( MSG_ALWAYS, ( "FAILED - " ));
    }
    DEBUG_PRINT(MSG_ALWAYS,("\nBase Address: %02X  BSM Status: %02X\n", (int)u8Data, (int)SiIRegioRead( REG_BSM_STAT )));

    CpDisplayChipInfo();

    HalTimerSet( TIMER_POLLING, DEM_POLLING_DELAY );
    while ( 1 )
    {
        /* Check Simon or HDMIGear requests.    */

        if ( CpCheckExternalRequests())
        {
            externalAccess = true;
            continue;
        }

        /* Poll the device at DEM_POLLING_DELAY ms intervals.   */

        if ( !HalTimerExpired( TIMER_POLLING ))
        {
            continue;
        }
        HalTimerSet( TIMER_POLLING, DEM_POLLING_DELAY );     // Reset poll timer

        rotarySwitch = CpReadRotarySwitch( false );

        /* Update port selection (Simon/HDMIGear/User may have changed it). */

        if ( externalAccess )
        {
            externalAccess = false;
            rotarySwitch = SiIRegioRead( REG_SYS_SWTCHC2 ) & MSK_ALL_EN;
        }

        /* Display the current main pipe and secondary pipe ports.   */

        u8Data = (SiIRegioRead( REG_PAUTH_STAT0 ) >> 4) & 0x0F;
        CpSetPortLEDs( u8Data | (SiIRegioRead( REG_PAUTH_STAT0 ) & 0x0F) );

        if ( rotarySwitch != RSW_NO_CHANGE )
        {
            static uint8_t  lastPort    = 0;
            uint8_t         newPort     = rotarySwitch & 0x03;

#if (USE_INTERNAL_MUTE == 0)
            SI_DeviceMute( true );
#endif
            SI_PortSelectSource( newPort );
            DEBUG_PRINT(MSG_ALWAYS,("\n[%d] Port Switch %d to %d", (int)g_pass, (int)lastPort, (int)newPort ));
            lastPort = newPort;
        }

        /* Call SI_DeviceEventMonitor API to process 9287 events,   */
        /* and mute or unmute as required.                          */

        monitorState = SI_DeviceEventMonitor();
#if (USE_INTERNAL_MUTE == 0)
        if ( oldMute != (monitorState & DEM_MUTE_ACTIVE) )
        {
            SI_DeviceMute( (monitorState & DEM_MUTE_ACTIVE) );
            oldMute = (monitorState & DEM_MUTE_ACTIVE);
        }
#endif
    }
}

