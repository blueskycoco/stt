//***************************************************************************
//!file     si_cp9287_ext.c
//!brief    CP 9287 Start Kit extension functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <si_cp9287.h>
#include <si_api9x87.h>
#include <si_regio.h>
#ifdef SII_9289
    #include <si_LedDriver.h>
#endif

//-------------------------------------------------------------------------------
//  Global data
//-------------------------------------------------------------------------------

BOOL g_deviceInterrupt = false;

//------------------------------------------------------------------------------
// Function:    DeviceIntHandler
// Description: Silicon Image device interrupt handler.  This function only
//              sets the global interrupt flag, since accessing the I2C port
//              in an ISR is not a good thing.
//------------------------------------------------------------------------------

@far @interrupt void DeviceIntHandler ( void )
{

    g_deviceInterrupt = true;
}

//------------------------------------------------------------------------------
// Function:    CpCheckExternalRequests
// Description: Returns true if HDMIGear or Simon wants control
//------------------------------------------------------------------------------

BOOL CpCheckExternalRequests ( void )
{
    static uint8_t counter = 0;
    static uint8_t simonLeds = 0x06;

    if ( HAL_RemoteRequestHandler() )
    {
        if ( counter > 50 )     // Blink every second that Simon is in operation
        {
            counter = 0;
            CpSetPortLEDs( simonLeds );
            simonLeds ^= 0x0F;
        }
        return( true );
    }

    return( false );
}

//------------------------------------------------------------------------------
// Function: CpSetPortLEDs
// Description: Turn the PORT LEDs on or off depending on the new state.
//------------------------------------------------------------------------------

void CpSetPortLEDs ( uint8_t newState )
{
    static uint8_t oldPortLEDs = 0xFF;

#if (FPGA_BUILD == 1)
    return;
#endif

#ifdef SII_9289
    if ( newState != oldPortLEDs )
    {
   		oldPortLEDs     = newState;
    	ledControl(newState);
    }
#else
    if ( newState != oldPortLEDs )
    {
        oldPortLEDs     = newState;
        //gpioPortLED0    = ((newState & 1) == 0);
        //gpioPortLED1    = ((newState & 2) == 0);
        //gpioPortLED2    = ((newState & 4) == 0);
        //gpioPortLED3    = ((newState & 8) == 0);
    }
#endif
}

//------------------------------------------------------------------------------
// Function:    CpBlinkTilReset
// Description: Blink 'til reset
//------------------------------------------------------------------------------

void CpBlinkTilReset ( uint8_t leds )
{
    while ( 1 )
    {
        CpSetPortLEDs( leds );
        leds ^= 0x0F;
        HalTimerWait( 200 );
    }
}

//------------------------------------------------------------------------------
// Function: CpDisplayChipInfo
// Description: Read and display chip model and revision.
//------------------------------------------------------------------------------

void CpDisplayChipInfo( int index )
{
    uint16_t    temp;
    uint8_t     rev;

    temp = ((uint16_t)SiIRegioRead(index, REG_DEV_IDH_RX)) << 8;
    temp |= SiIRegioRead(index, REG_DEV_IDL_RX );
    rev = SiIRegioRead(index, REG_DEV_REV );
    DEBUG_PRINT( MSG_ALWAYS,( "Silicon Image Device: %04X, rev %02X\n\n", temp, (int)rev ));

    if (( temp != 0x9287 ) && ( temp != 0x9285 ) && ( temp != 0x9187 ))
    {
        DEBUG_PRINT( MSG_ALWAYS, ( "\n!!!!Unable to access or wrong device, halting...\n" ));
        CpBlinkTilReset( 0x06 );
    }
}

//------------------------------------------------------------------------------
// Function:    CpReadRotarySwitch
// Description: 
//------------------------------------------------------------------------------

uint8_t CpReadRotarySwitch ( uint8_t iWantItNow )
{
    uint8_t temp;

    temp = HalGpioReadRotarySwitch( iWantItNow );

    return( temp );
}

//------------------------------------------------------------------------------
// Function:    CpDisplayData
// Description: Display the passed buffer in ASCII-HEX block format
//------------------------------------------------------------------------------

void CpDisplayData ( uint8_t msgLevel, uint8_t *pData, uint16_t length )
{
    uint16_t    i = 0;
    uint8_t     x, y;

    for ( y = 0; i < length; y++ )
    {
        DEBUG_PRINT( msgLevel, ( "\n%02X: ", i ));
        for ( x = 0; (i < length) && (x < 16); x++ )
        {
            DEBUG_PRINT( msgLevel, ( " %02X", (uint16_t)pData[ i++] ));
        }
    }
    DEBUG_PRINT( msgLevel, ( "\n" ));
}

