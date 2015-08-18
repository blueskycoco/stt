//***************************************************************************
//!file     si_edid.c
//!brief    CP 9287 Starter Kit EDID functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <si_regio.h>
#include "si_api9x87.h"
#include "si_edid.h"
#if (API_DEBUG_CODE==1)
    #include <stdio.h>
#endif

//------------------------------------------------------------------------------
//  Module Data
//------------------------------------------------------------------------------

static uint8_t XDATA l_localData [EDID_TABLE_LEN];

//------------------------------------------------------------------------------
//  Function:       SI_EdidValidHdmi
//  Description:    Validate the passed EDID as an HDMI EDID.
//------------------------------------------------------------------------------

BOOL SI_EdidValidHdmi ( uint8_t *pData )
{
    uint16_t    i, edidOffset;
    uint8_t     accum = 0;

    /* Check the checksum for the first EDID block. */

    accum       = 0;
    for ( i = 0; i < (EDID_TABLE_LEN / 2); i++ )
    {
        accum += pData[ i];             // Gather checksum info
    }
    if ( accum != 0 )
    {
        return( false );
    }

        /* Locate 24-bit IEEE Registration number for Vendor-specific data. */
        /* For this search, I am not looking for CEA Extension or a Vendor- */
        /* specific data block.  Rather, I am assuming that the 24-bit      */
        /* IEEE code will be unique enough.  This may not hold up.          */

    edidOffset  = 0xFF;
    accum       = 0;
    for ( i = (EDID_TABLE_LEN / 2); i < EDID_TABLE_LEN; i++ )
    {
        accum += pData[ i];             // Gather checksum info

        if ( pData[ i] == 0x03 )        // 0x000C03 belongs to HDMI Licensing, LLC
        {
            if ( pData[ i + 1] == 0x0C )
            {
                if ( pData[ i + 2] == 0x00 )
                {
                    edidOffset = i + 3; // Point to CEC Phys Address field.
                }
            }
        }
    }

    return( (( edidOffset != 0xFF ) && ( accum == 0 )) );
}

//------------------------------------------------------------------------------
//  Function:       SI_EdidUpdateHdmiData
//  Description:    Update the passed EDID array physical address (HDMI ext)
//                  and checksum.
//
//  Returns: Eight bit offset within EDID of physical address field, or 0xFF
//           if a failure occurs.
//
//  NOTE:   This function assumes that the HDMI physical address
//          of each port is 0x1000, 0x2000, 0x3000, 0x4000,
//          respectively
//------------------------------------------------------------------------------

uint8_t SI_EdidUpdateHdmiData ( uint8_t *pData, uint8_t port )
{
    uint16_t    i, edidOffset;
    uint8_t     accum;

    /* Generate the checksum for the first EDID block.  */

    accum       = 0;
    for ( i = 0; i < ((EDID_TABLE_LEN / 2) - 1); i++ )
    {
        accum += pData[ i];             // Gather checksum info
    }
    pData[ i] = 0 - accum;

        /* Locate 24-bit IEEE Registration number for Vendor-specific data. */
        /* For this search, I am not looking for CEA Extension or a Vendor- */
        /* specific data block.  Rather, I am assuming that the 24-bit      */
        /* IEEE code will be unique enough.  This may not hold up.          */

    edidOffset  = 0xFF;
    accum       = 0;
    for ( i = (EDID_TABLE_LEN / 2); i < (EDID_TABLE_LEN - 1); i++ )
    {
        accum += pData[ i];             // Gather checksum info

        /* Add the physical address if signature is found,  */
        /* otherwise write the data without modification.   */

        if ( pData[ i] == 0x03 )        // 0x000C03 belongs to HDMI Licensing, LLC
        {
            if ( pData[ i + 1] == 0x0C )
            {
                if ( pData[ i + 2] == 0x00 )
                {
                    edidOffset              = i + 3;            // Point to CEC Phys Address field.
                    pData[ edidOffset]      = (port + 1) << 4;
                    pData[ edidOffset + 1]  = 0;
                }
            }
        }
    }
    pData[ i] = 0 - accum;

    return( (uint8_t)edidOffset );
}

//------------------------------------------------------------------------------
//  Function:       si_EdidReadPortRam
//  Description:    Read the port EDID ram from the specified index.
//------------------------------------------------------------------------------

void si_EdidReadPortRam ( uint8_t ramIndex, uint8_t sourceOffset, uint8_t *pDest, uint16_t length )
{
    uint16_t i;
    uint8_t     ramBitSelect;

    if ( ramIndex == EDID_RAM_VGA )
    {
        ramBitSelect = BIT_SEL_EDID_VGA; 
    }
    else
    {
        ramBitSelect = (BIT_SEL_EDID0 << ramIndex);
    }

        /* Point to beginning of selected port EDID ram.    */

    SiIRegioWrite( REG_EDID_FIFO_SEL, ramBitSelect );
    SiIRegioWrite( REG_EDID_FIFO_ADDR, VAL_FIFO_ADDR_00 + sourceOffset );

        /* Read it into the passed array.    */

    for ( i = 0; i < length; i++ )
    {
        pDest[ i ] = SiIRegioRead( REG_EDID_FIFO_DATA );
    }
}

//------------------------------------------------------------------------------
//  Function:       EdidWritePortRam
//  Description:    Write the selected EDID ram from the passed data
//------------------------------------------------------------------------------

void si_EdidWritePortRam ( uint8_t ramIndex, uint8_t *pSource, uint8_t sourceOffset, uint16_t length )
{
    uint16_t i;
    uint8_t     ramBitSelect;

    if ( ramIndex == EDID_RAM_VGA )
    {
        ramBitSelect = BIT_SEL_EDID_VGA; 
    }
    else
    {
        ramBitSelect = (BIT_SEL_EDID0 << ramIndex);
    }

        /* Point to beginning of selected port EDID ram.    */

    SiIRegioWrite( REG_EDID_FIFO_SEL, ramBitSelect );
    SiIRegioWrite( REG_EDID_FIFO_ADDR, VAL_FIFO_ADDR_00 + sourceOffset );

        /* Load it from the passed array. */

    for ( i = 0; i < length; i++ )
    {
        SiIRegioWrite( REG_EDID_FIFO_DATA, pSource[ i] );
    }
}

//------------------------------------------------------------------------------
//  Function:       si_EdidReadBootRam
//  Description:    Read the device boot data ram.
//------------------------------------------------------------------------------

void si_EdidReadBootRam ( uint8_t *pDest, uint8_t sourceOffset, uint8_t length )
{
    uint16_t i;

        /* Point to beginning of selected port EDID ram.    */

    SiIRegioWrite( REG_EDID_FIFO_SEL, BIT_SEL_DEVBOOT );
    SiIRegioWrite( REG_EDID_FIFO_ADDR, VAL_FIFO_ADDR_00 + sourceOffset );

        /* Read it into the passed array.    */

    for ( i = 0; i < length; i++ )
    {
        pDest[ i ] = SiIRegioRead( REG_EDID_FIFO_DATA );
    }
}

//------------------------------------------------------------------------------
//  Function:       EdidWriteBootRam
//  Description:    Write the Device Boot ram @ EDID 0 from the passed array
//------------------------------------------------------------------------------

void si_EdidWriteBootRam ( uint8_t *pSource, uint8_t sourceOffset, uint8_t length )
{
    uint8_t i;

        /* Point to beginning of EDID Device Boot ram.    */

    SiIRegioWrite( REG_EDID_FIFO_SEL, BIT_SEL_DEVBOOT );
    SiIRegioWrite( REG_EDID_FIFO_ADDR, VAL_FIFO_ADDR_00 + sourceOffset );

        /* Load it from the passed array. */

    for ( i = 0; i < length; i++ )
    {
        SiIRegioWrite( REG_EDID_FIFO_DATA, pSource[ i] );
    }
}

//------------------------------------------------------------------------------
//  Function:       si_NvramCommand
//  Description:    Execute the passed NVRAM command and wait for done bit.
//------------------------------------------------------------------------------

BOOL si_NvramCommand ( uint8_t command )
{
    uint8_t test;
    BOOL    success = true;

        /* Start the NVRAM program operation and wait for it to finish. */

    SiIRegioWrite( REG_NVM_COMMAND, command );
    HalTimerSet( ELAPSED_TIMER, 1 );
    for ( ;; )
    {
        test = SiIRegioRead( REG_NVM_COMMAND_DONE );
        if ( test & BIT_NVM_COMMAND_DONE )
            break;
        if ( HalTimerElapsed() >= 4000 )    // 4 second timeout
        {
            success = false;
            break;
        }
    }

    return( success );
}

//------------------------------------------------------------------------------
//  Function:       EdidReadNvram
//  Description:    Read NVRAM EDID into the passed array.
//------------------------------------------------------------------------------

static BOOL si_EdidReadNvram ( uint8_t *pDest )
{
    uint16_t    i;
    BOOL        success = true;

        /* Save EDID 0 ram. */

    si_EdidReadPortRam( EDID_RAM_0, 0, l_localData, EDID_TABLE_LEN );

        /* Copy the NVRAM data into EDID ram 0. */

    SiIRegioWrite( REG_NVM_COPYTO, VAL_NVM_COPYTO_PORT0 );
    success = si_NvramCommand( VAL_COPY_EDID );

        /* Read the copied data into caller's array. */

    SiIRegioWrite( REG_EDID_FIFO_SEL, BIT_SEL_EDID0 );
    SiIRegioWrite( REG_EDID_FIFO_ADDR, VAL_FIFO_ADDR_00 );
    for ( i = 0; i < EDID_TABLE_LEN; i++ )
    {
       pDest[ i] = SiIRegioRead( REG_EDID_FIFO_DATA );
    }

        /* Replace the saved EDID 0 sram data.  */

    si_EdidWritePortRam( EDID_RAM_0, l_localData, 0, EDID_TABLE_LEN );
    return( success );
}

//------------------------------------------------------------------------------
//  Function:       si_EdidReadNvramBoot 
//  Description:    Read NVRAM Boot data into the passed array.
//------------------------------------------------------------------------------

static BOOL si_EdidReadNvramBoot ( uint8_t *pDest )
{
    uint16_t    i;
    BOOL        success;

        /* Save current boot ram.   */

    si_EdidReadBootRam( l_localData, 0, EDID_DEVBOOT_LEN );

        /* Copy the NVRAM data into EDID ram 0. */

    SiIRegioWrite( REG_NVM_COPYTO, VAL_NVM_COPYTO_PORT0 );
    success = si_NvramCommand( VAL_COPY_DEVBOOT );

        /* Read the copied data into caller's array. */

    SiIRegioWrite( REG_EDID_FIFO_SEL, BIT_SEL_DEVBOOT );
    SiIRegioWrite( REG_EDID_FIFO_ADDR, VAL_FIFO_ADDR_00 );
    for ( i = 0; i < EDID_DEVBOOT_LEN; i++ )
    {
       pDest[ i] = SiIRegioRead( REG_EDID_FIFO_DATA );
    }

        /* Replace the saved boot sram data.    */

    si_EdidWriteBootRam( l_localData, 0, EDID_DEVBOOT_LEN );
    return( success );
}

//------------------------------------------------------------------------------
//  Function:       EdidProgramNvramBoot
//  Description:    Program the 'DevBootInfo' 64 bytes of NVRAM from the EEPROM,
//                  FLASH, or the extended part of the g_edidTable array.
//------------------------------------------------------------------------------

static BOOL EdidProgramNvramBoot ( uint8_t *pSource )
{
    /* Program data for NVRAM boot data always comes from boot data ram, so */
    /* load boot data ram with boot data from passed array.                 */

    si_EdidWriteBootRam( pSource, 0, EDID_DEVBOOT_LEN );

        /* Start the NVRAM program operation and wait for it to finish. */

    return( si_NvramCommand( VAL_PRG_DEVBOOT ));
}

//------------------------------------------------------------------------------
//  Function:       EdidProgramNvram
//  Description:    Program the SiI9287 EDID NVRAM from passed source data
//------------------------------------------------------------------------------

static BOOL EdidProgramNvram ( uint8_t *pSource )
{
    BOOL    success = true;

    si_EdidReadPortRam( EDID_RAM_0, 0, l_localData, EDID_TABLE_LEN );   // Save current EDID 0 ram.

        /* Program data for NVRAM always comes from  EDID ram 0, so */
        /* load ram 0 with EDID table from requested source.        */

    si_EdidWritePortRam( EDID_RAM_0, pSource, 0, EDID_TABLE_LEN );

        /* Start the NVRAM program operation and wait for it to finish. */

    success = si_NvramCommand( VAL_PRG_EDID );

    si_EdidWritePortRam( EDID_RAM_0, l_localData, 0, EDID_TABLE_LEN );
    return( success );
}

//------------------------------------------------------------------------------
//  Function:       SI_EdidRead
//  Description:    Read the selected EDID source into the passed array.
//------------------------------------------------------------------------------

BOOL SI_EdidRead( uint8_t source, uint8_t *pDest )
{
    BOOL success = true;

    switch (source)
    {
        case EDID_RAM_0:
        case EDID_RAM_1:
        case EDID_RAM_2:
        case EDID_RAM_3:
        case EDID_RAM_4:
            si_EdidReadPortRam( source, 0, pDest, EDID_TABLE_LEN );
            break;
        case EDID_RAM_VGA:
            si_EdidReadPortRam( source, 0, pDest, EDID_VGA_TABLE_LEN );
            break;
        case EDID_RAM_BOOT:
            si_EdidReadBootRam( pDest, 0, EDID_DEVBOOT_LEN );
            break;
        case EDID_NVRAM:
            success = si_EdidReadNvram( pDest );
            break;
        case EDID_NVRAM_BOOT:
            success = si_EdidReadNvramBoot( pDest );
            break;
        default:
            success = false;
            break;
    }

    return( success );
}

//------------------------------------------------------------------------------
//  Function:       SI_EdidWrite
//  Description:    Write the selected EDID target from the passed array.
//------------------------------------------------------------------------------

BOOL SI_EdidWrite ( uint8_t target, uint8_t *pSource )
{
    BOOL success = true;

    switch (target)
    {
        case EDID_RAM_0:
        case EDID_RAM_1:
        case EDID_RAM_2:
        case EDID_RAM_3:
        case EDID_RAM_4:
            si_EdidWritePortRam( target, pSource, 0, EDID_TABLE_LEN );
            break;
        case EDID_RAM_VGA:
            si_EdidWritePortRam( target, pSource, 0, EDID_VGA_TABLE_LEN );
            break;
        case EDID_RAM_BOOT:
            si_EdidWriteBootRam( pSource, 0, EDID_DEVBOOT_LEN );
            break;
        case EDID_NVRAM:
            success = EdidProgramNvram( pSource );
            break;
        case EDID_NVRAM_BOOT:
            success = EdidProgramNvramBoot( pSource );
            break;
        case EDID_BUFFER:
            memcpy( l_localData, pSource, EDID_TABLE_LEN );
            break;
        default:
            success = false;
            break;
    }

    return( success );
}

//------------------------------------------------------------------------------
//  Function:       SI_EdidLoadPortRam
//  Description:    Load the specified port ram from the specified source.
//
//  NOTE: This function does not lower or raise the Hot plug signal, so that
//        will have to be performed by the caller.
//------------------------------------------------------------------------------

BOOL SI_EdidLoadPortRam ( uint8_t source, uint8_t port )
{
    BOOL success = true;

    SI_EdidRead( source, l_localData );

    if ( port < EDID_RAM_VGA )
    {
        success = (SI_EdidUpdateHdmiData( l_localData, port ) != 0xFF );
    }

        /* Write to selected EDID ram.  */

    if ( success )
    {
        si_EdidWritePortRam( port, l_localData, 0, EDID_TABLE_LEN );
    }
    return( success );
}

//------------------------------------------------------------------------------
//  Function:       SI_EdidInitialize
//  Description:    Check to see if the NVRAM has been initialized and do it if needed.
//------------------------------------------------------------------------------

BOOL SI_EdidInitialize ( void )
{
    BOOL success = true;

    if ((SiIRegioRead( REG_NVM_STAT ) != VAL_NVM_VALID))
    {
        DEBUG_PRINT( MSG_ALWAYS, ( "\nNVRAM NOT INITIALIZED, initializing..." ));
        success = SI_EdidWrite( EDID_NVRAM_BOOT, (uint8_t *)&g_edidFlashDevBootData );
        if ( success )
        {
            success = SI_EdidWrite( EDID_NVRAM, (uint8_t *)g_edidFlashEdidTable );

            /* Force a boot load to get the new EDID data from the NVRAM to the chip.   */

            SiIRegioWrite( REG_BSM_INIT, BIT_BSM_INIT );
            success = SI_DeviceBootComplete();
        }
    }

    if ( success )
    {
        DEBUG_PRINT( MSG_ALWAYS, ( " successful!" ));
    }
    else
    {
        DEBUG_PRINT( MSG_ALWAYS, ( " FAILED" ));
    }
    return( success );
}
