//***************************************************************************
//!file     si_hal.h
//!brief    Silicon Image HAL function library header.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __HAL_H__
#define __HAL_H__
#include "si_datatypes.h"
//#include "at89c51xd2.h"

//-------------------------------------------------------------------------------
//  HAL Macros
//-------------------------------------------------------------------------------

#define CONF__DEBUG_PRINT               (DISABLE)    //print debug messages

    //   Note: DEBUG_PRINT Requires double parenthesis
    //   Example:  DEBUG_PRINT(MSG_ALWAYS,("hello, world!\n"));

#if ( CONF__DEBUG_PRINT == ENABLE)
#define DEBUG_PRINT(l,x)      if (l<=g_halMsgLevel) printf x
#else
    #define DEBUG_PRINT(l,x)
#endif

//-------------------------------------------------------------------------------
//  HAL Constants
//-------------------------------------------------------------------------------

#define I2C_BUS0        0
#define I2C_BUS1        1
#define I2C_BUS2        2
#define I2C_BUS3        3

    /* Rotary switch values.    */

#define RSW_POS_0           0x00
#define RSW_POS_1           0x01
#define RSW_POS_2           0x02
#define RSW_POS_3           0x03
#define RSW_POS_4           0x04
#define RSW_POS_5           0x05
#define RSW_POS_6           0x06
#define RSW_POS_7           0x07
#define RSW_NO_CHANGE       0xFF    // Rotary switch has not changed

//-------------------------------------------------------------------------------
//  Required before use of some HAL modules
//-------------------------------------------------------------------------------

BOOL    HalInitialize( void );
uint8_t HalVersion( BOOL wantMajor );
uint8_t HalVersionFPGA( BOOL wantMajor );

//-------------------------------------------------------------------------------
//  I2C Master BUS Interface
//-------------------------------------------------------------------------------

uint8_t HalI2cBus0ReadByte( uint8_t device_id, uint8_t addr );
void HalI2cBus0WriteByte( uint8_t deviceID, uint8_t offset, uint8_t value );
BOOL HalI2cBus0ReadBlock( uint8_t deviceID, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
BOOL HalI2cBus0WriteBlock( uint8_t device_id, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
BOOL HalI2cBus016ReadBlock( uint8_t device_id, uint16_t addr, uint8_t *p_data, uint16_t nbytes );
BOOL HalI2cBus016WriteBlock( uint8_t device_id, uint16_t addr, uint8_t *p_data, uint16_t nbytes );
BOOL HalI2CBus0WaitForAck( uint8_t device_id );
uint8_t HalI2cBus0GetStatus( void );

uint8_t HalI2cBus1ReadByte( uint8_t device_id, uint8_t addr );
void HalI2cBus1WriteByte( uint8_t deviceID, uint8_t offset, uint8_t value );
BOOL HalI2cBus1ReadBlock( uint8_t deviceID, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
BOOL HalI2cBus1WriteBlock( uint8_t device_id, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
uint8_t HalI2cBus1GetStatus( void );

uint8_t HalI2cBus2ReadByte( uint8_t device_id, uint8_t addr );
void HalI2cBus2WriteByte( uint8_t deviceID, uint8_t offset, uint8_t value );
BOOL HalI2cBus2ReadBlock( uint8_t deviceID, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
BOOL HalI2cBus2WriteBlock( uint8_t device_id, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
uint8_t HalI2cBus2GetStatus( void );

uint8_t HalI2cBus3ReadByte( uint8_t device_id, uint8_t addr );
void HalI2cBus3WriteByte( uint8_t deviceID, uint8_t offset, uint8_t value );
BOOL HalI2cBus3ReadBlock( uint8_t deviceID, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
BOOL HalI2cBus3WriteBlock( uint8_t device_id, uint8_t addr, uint8_t *p_data, uint16_t nbytes );
uint8_t HalI2cBus3GetStatus( void );

//-------------------------------------------------------------------------------
//  I2C Local Bus
//  Macros to select the I2C bus that is used by this board's local I2C bus.
//-------------------------------------------------------------------------------

#define HalI2cReadByte( device_id, addr )                       HalI2cBus0ReadByte( device_id, addr )
#define HalI2cWriteByte( device_id, addr, value )               HalI2cBus0WriteByte( device_id, offset, value )
#define HalI2cReadBlock( device_id, addr, p_data, nbytes )      HalI2cBus0ReadBlock( device_id, addr, p_data, nbytes )
#define HalI2cWriteBlock( device_id, addr, p_data, nbytes )     HalI2cBus0WriteBlock( device_id, addr, p_data, nbytes )
#define HalI2c16ReadBlock( device_id, addr, p_data, nbytes )    HalI2cBus016ReadBlock( device_id, addr, p_data, nbytes )
#define HalI2c16WriteBlock( device_id, addr, p_data, nbytes )   HalI2cBus016WriteBlock( device_id, addr, p_data, nbytes )
#define HalI2CWaitForAck( device_id )                           HalI2CBus0WaitForAck( device_id )
#define HalI2cGetStatus()                                       HalI2cBus0GetStatus()

//-------------------------------------------------------------------------------
//  UART functions
//-------------------------------------------------------------------------------

#define MSG_ALWAYS              0x00
#define MSG_STAT                0x01
#define MSG_DBG                 0x02
#define MSG_PRINT_ALL           0xFF

extern uint8_t     g_halMsgLevel;

void    HalUartInit( void );
//uint8_t putchar( uint8_t c );
//uint8_t _getkey( void );

//-------------------------------------------------------------------------------
//  REMOTE functions
//-------------------------------------------------------------------------------

BOOL HAL_RemoteRequestHandler( void );

//-------------------------------------------------------------------------------
//  Timer Functions
//-------------------------------------------------------------------------------

#define ELAPSED_TIMER               0xFF
#define TIMER_0                     0   // DO NOT USE - reserved for TimerWait()
#define TIMER_POLLING               1   // Reserved for main polling loop
#define TIMER_2                     2   // Available
#define TIMER_3                     2   // Available

void    HalTimerInit( void );
void    HalTimerSet( uint8_t timer, uint16_t m_sec );
uint8_t HalTimerExpired( uint8_t timer );
void    HalTimerWait( uint16_t m_sec );
uint16_t HalTimerElapsed ( void );

//-------------------------------------------------------------------------------
//  GPIO Functions
//-------------------------------------------------------------------------------

uint8_t HalGpioReadRotarySwitch ( uint8_t i_want_it_now );

//-------------------------------------------------------------------------------
//  Internal EEPROM functions
//-------------------------------------------------------------------------------

uint8_t HalEEintReadByte( uint16_t addr );
void    HalEEintWriteByte( uint16_t addr, uint8_t value );
BOOL    HalEEintReadBlock( uint16_t addr, uint8_t *p_data, uint16_t nbytes );
BOOL    HalEEintWriteBlock( uint16_t addr, uint8_t *p_data, uint16_t nbytes );

//-------------------------------------------------------------------------------
//  External EEPROM functions
//-------------------------------------------------------------------------------

uint8_t HalEEextReadByte( uint16_t addr );
void    HalEEextWriteByte( uint16_t addr, uint8_t value );
BOOL    HalEEextReadBlock( uint16_t addr, uint8_t *p_data, uint16_t nbytes );
BOOL    HalEEextWriteBlock( uint16_t addr, uint8_t *p_data, uint16_t nbytes );

//-------------------------------------------------------------------------------
//  IR Interface functions
//-------------------------------------------------------------------------------

typedef enum
{
    NO_EVENT,  //!< nothing happened
    KEY_DOWN,  //!< remote control key pressed down
    KEY_UP    //!< remote control key released
};

typedef struct
{
    uint8_t eventType;  //!< UI event type
    uint8_t command;    //!< UI command
} SI_IrCommand_t;

typedef struct 
{
    uint8_t rc5Command;         //!< rc protocol command code
    uint8_t uiCommand;          //!< UI command code
} SI_UiRc5Conversion_t;

typedef struct
{
    uint8_t rc5address;
    uint8_t uiRc5convTableLength;
    const SI_UiRc5Conversion_t* uiRc5convTable;

} SI_UiRC5CommandMap_t;

// user will provide this for the lib( no need to change lib when customizing applications ).

extern const SI_UiRC5CommandMap_t si_irCommandMap;
                                 
SI_IrCommand_t SI_IRGetCommand( void );
void SI_IRInit( void );

#endif  // _HAL_H__
