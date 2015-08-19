//***************************************************************************
//!file     si_regio.c
//!brief    Silicon Image Device register I/O support.
//
// No part of this work may be reproduced, modified, distributed, 
// transmitted, transcribed, or translated into any language or computer 
// format, in any form or by any means without written permission of 
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_regio.h"
#include "si_hal.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

// Register Slave Address page map

static uint8_t  XDATA l_regioDecodePage [16] =
{
    REGIO_SLAVE_PAGE_0, REGIO_SLAVE_PAGE_1, REGIO_SLAVE_PAGE_2, REGIO_SLAVE_PAGE_3,
    REGIO_SLAVE_PAGE_4, REGIO_SLAVE_PAGE_5, REGIO_SLAVE_PAGE_6, REGIO_SLAVE_PAGE_7,
    REGIO_SLAVE_PAGE_8, REGIO_SLAVE_PAGE_9, REGIO_SLAVE_PAGE_A, REGIO_SLAVE_PAGE_B,
    REGIO_SLAVE_PAGE_C, REGIO_SLAVE_PAGE_D, REGIO_SLAVE_PAGE_E, REGIO_SLAVE_PAGE_F
};

//------------------------------------------------------------------------------
// Function:    SiIRegioSetBase
// Description: Set the device ID of the passed page.
//              Used to change from the default value if necessary.
//------------------------------------------------------------------------------

void SiIRegioSetBase (int index, uint8_t page, uint8_t newID )
{

    if ( page < sizeof( l_regioDecodePage ))
    {
        /* Change look-up table.    */

        l_regioDecodePage[ page] = newID;

        /* Change programmable base address register.   */

        switch ( page )
        {
        case  2:    SiIRegioWrite(index, REG_SLAVE_ADDR6, newID );    break;
        case  4:    SiIRegioWrite(index, REG_SLAVE_ADDR7, newID );    break;
        case  8:    SiIRegioWrite(index, REG_SLAVE_ADDR2, newID );    break;
        case  9:    SiIRegioWrite(index, REG_SLAVE_ADDR3, newID );    break;
        case 10:    SiIRegioWrite(index, REG_SLAVE_ADDR4, newID );    break;
        case 11:    SiIRegioWrite(index, REG_SLAVE_ADDR5, newID );    break;
        case 12:    SiIRegioWrite(index, REG_SLAVE_ADDR1, newID );    break;
        case 13:    SiIRegioWrite(index, REG_SLAVE_ADDR0, newID );    break;
        case 14:    SiIRegioWrite(index, REG_SLAVE_ADDR5, newID );    break;
        default:                                                break;
        }
    }
    
}

//------------------------------------------------------------------------------
// Function:    SiIRegioRead
// Description: Read a one byte register.
//              The register address parameter is translated into an I2C slave
//              address and offset. The I2C slave address and offset are used 
//              to perform an I2C read operation.
//------------------------------------------------------------------------------

uint8_t SiIRegioRead (int index, uint16_t regAddr )
{

    return( HalI2cBus0ReadByte(index, l_regioDecodePage[ regAddr >> 8], (uint8_t)regAddr ));
}

//------------------------------------------------------------------------------
// Function:    SiIRegioWrite
// Description: Write a one byte register.
//              The register address parameter is translated into an I2C
//              slave address and offset. The I2C slave address and offset 
//              are used to perform an I2C write operation.
//------------------------------------------------------------------------------

void SiIRegioWrite (int index, uint16_t regAddr, uint8_t value )
{

    HalI2cBus0WriteByte(index, l_regioDecodePage[ regAddr >> 8], (uint8_t)regAddr, value );
}

//------------------------------------------------------------------------------
// Function:    SiIRegioReadBlock
// Description: Read a block of registers starting with the specified register.
//              The register address parameter is translated into an I2C 
//              slave address and offset.
//              The block of data bytes is read from the I2C slave address 
//              and offset.
//------------------------------------------------------------------------------

void SiIRegioReadBlock ( uint16_t regAddr, uint8_t* buffer, uint16_t length)
{

    HalI2cBus0ReadBlock( l_regioDecodePage[ regAddr >> 8], (uint8_t)regAddr, buffer, length);
}

//------------------------------------------------------------------------------
// Function:    SiIRegioWriteBlock
// Description: Write a block of registers starting with the specified register.
//              The register address parameter is translated into an I2C slave
//              address and offset.
//              The block of data bytes is written to the I2C slave address 
//              and offset.
//------------------------------------------------------------------------------

void SiIRegioWriteBlock ( uint16_t regAddr, uint8_t* buffer, uint16_t length)
{

    HalI2cBus0WriteBlock( l_regioDecodePage[ regAddr >> 8], (uint8_t)regAddr, buffer, length);
}

//------------------------------------------------------------------------------
// Function:    SiIRegioBitToggle
// Description: Toggle a bit or bits in a register
//              The register address parameter is translated into an I2C 
//              slave address and offset.
//              The I2C slave address and offset are used to perform I2C 
//              read and write operations.
//
//              All bits specified in the mask are first set and then cleared 
//              in the register.
//              This is a common operation for toggling a bit manually.
//------------------------------------------------------------------------------

void SiIRegioBitToggle (int index, uint16_t regAddr, uint8_t mask)
{
    uint8_t slaveID, abyte;

    slaveID = l_regioDecodePage[ regAddr >> 8];

    abyte = HalI2cBus0ReadByte(index, slaveID, (uint8_t)regAddr );
    abyte |=  mask;                                         //first set the bits in mask
    HalI2cBus0WriteByte(index, slaveID, (uint8_t)regAddr, abyte);        //write register with bits set
    abyte &= ~mask;                                         //then clear the bits in mask
    HalI2cBus0WriteByte(index, slaveID, (uint8_t)regAddr, abyte);        //write register with bits clear
}

//------------------------------------------------------------------------------
// Function:    SiIRegioModify
// Description: Modify a one byte register under mask.
//              The register address parameter is translated into an I2C 
//              slave address and offset. The I2C slave address and offset are 
//              used to perform I2C read and write operations.
//
//              All bits specified in the mask are set in the register 
//              according to the value specified.
//              A mask of 0x00 does not change any bits.
//              A mask of 0xFF is the same a writing a byte - all bits 
//              are set to the value given.
//              When only some bits in the mask are set, only those bits are 
//              changed to the values given.
//------------------------------------------------------------------------------

void SiIRegioModify (int index, uint16_t regAddr, uint8_t mask, uint8_t value)
{
    uint8_t slaveID, abyte;

    slaveID = l_regioDecodePage[ regAddr >> 8];

    abyte = HalI2cBus0ReadByte(index, slaveID, (uint8_t)regAddr );
    abyte &= (~mask);                                       //first clear all bits in mask
    abyte |= (mask & value);                                //then set bits from value
    HalI2cBus0WriteByte(index, slaveID, (uint8_t)regAddr, abyte);
}

