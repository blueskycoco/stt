//***************************************************************************
//!file     si_api9x87.h
//!brief    SiI9287 API functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_API9287_H__
#define __SI_API9287_H__
#include <si_datatypes.h>

//------------------------------------------------------------------------------
// Target System specific
//------------------------------------------------------------------------------

extern char g_strDeviceID [];

#define USE_INTERNAL_MUTE       0           // Set to 1 to use SiI9287 muting
#define DO_EDID_INIT            1           // Set to 1 to perform EDID initialization during boot process
#define DO_VGA_EDID_INIT        1           // Set to 1 to write a VGA EDID to the VGA port memory during boot process
#define API_DEBUG_CODE          1           // Set to 0 to eliminate debug print messages from code
                                            // Set to 1 to allow debug print messages in code
#define FPGA_DEBUG_CODE         0           // Set to 0 to for normal operation
                                            // Set to 1 to to work with FPGA test board

    // Debug Macros

#if (API_DEBUG_CODE==1)
    extern  uint8_t     g_halMsgLevel;

    #ifndef DEBUG_PRINT
        #define DEBUG_PRINT(l,x)        if (l<=g_halMsgLevel) printf x
    #endif
    #define _DEBUG_(x)              x

#else
    #ifndef DEBUG_PRINT
        #define DEBUG_PRINT(l,x)
    #endif
    #define _DEBUG_(x)
#endif

    // MSG API   - For Debug purposes

#define MSG_ALWAYS              0x00
#define MSG_STAT                0x01
#define MSG_DBG                 0x02

    // Timer API - Target system must create these functions

#define ELAPSED_TIMER               0xFF

void    HalTimerSet( uint8_t timer, uint16_t m_sec );
void    HalTimerWait( uint16_t m_sec );
uint16_t HalTimerElapsed( void );

//------------------------------------------------------------------------------
// Debug Trace
//------------------------------------------------------------------------------

#if (API_DEBUG_CODE==1)
    #define DF1_SCDT_INT            0x01
    #define DF1_SCDT_HI             0x02
    #define DF1_SCDT_STABLE         0x04
    #define DF1_HDCP_STABLE         0x08
    #define DF1_NON_HDCP_STABLE     0x10
    #define DF1_MP_AUTH             0x20
    #define DF1_MP_DECRYPT          0x40
    #define DF1_HPD                 0x80

    #define DF2_HDCP_FOUND          0x01
    #define DF2_FIRST_PASS          0x02
    #define DF2_HDMI_MODE           0x04
    #define DF2_MUTE_ON             0x08
    #define DF2_PORT_SWITCH_REQ     0x10
    #define DF2_FULL_HOTPLUG        0x20

    extern uint8_t g_demFlags1;
    extern uint8_t g_demFlags2;

#endif


//------------------------------------------------------------------------------
// Device Event Monitor definitions
//------------------------------------------------------------------------------

#define DELAY_MATCH_SETTLE_TIME (100  / DEM_POLLING_DELAY   )

#define DEM_MAX_POWERUP_HOTPLUG (1200 / DEM_POLLING_DELAY )     // Length of full hotplug performed after an AC power loss and restore
#define DEM_HPD_TOGGLE_TIME     (1200 / DEM_POLLING_DELAY   )   // Leave HPD low for ~200ms. Has to be larger than DEM_DVI_DET.
                                                                // Otherwise, DVI settle down after HPD comes back
#define DEM_RXTERM_TOGGLE_TIME  (DEM_HPD_TOGGLE_TIME * 3 / 4)   // Leave RXTerm disabled for shorter time.

#define DEM_SCDT_INT_DELAY      (300  / DEM_POLLING_DELAY   )   // Don't check DM range until this time after a SCDT interrupt
#define DEM_SCDT_STABLE         (500  / DEM_POLLING_DELAY   )   // Time that SCDT must remain high to be considered stable.
                                                                // ECC checking turn on time
#define DEM_DVI_DET             (200  / DEM_POLLING_DELAY   )   // Time to decide DVI mode and turn off auto reset
#define DEM_POLL_PERIOD         (2500 / DEM_POLLING_DELAY   )   // Length of polling period to count DM value changes
#define DEM_ENOUGH_CHGS         (800  / DEM_POLLING_DELAY   )   // Number of DM changes in polling period to be
                                                                // considered an oscillating change.
#define DEM_AUTH_TIME           (1000 / DEM_POLLING_DELAY )     // Do not perform HPD on a port switch after this time

#define DEM_DECRYPT_ECC_THRESHOLD   (800 / DEM_POLLING_DELAY )  // Turn on ECC checking after decrypting for this period.

#define DEM_MUTE_LENGTH         (300  / DEM_POLLING_DELAY   )   // Duration of Muteduring PWR/SCDT event
#define DEM_MUTE_LEN_PORTCHANGE (300  / DEM_POLLING_DELAY   )   // Duration of Mute during switching
                                                                // 0 means no Muting while switching
#define DEM_MULTIPLIER_FOR_NONHDCP  8                           // Multiply each DEM_TMDS_OFF_xxx delay step by this 
                                                                // value (except the last step, which is always 100ms).
                                                                // This is used because HDCP devices have been known to 
                                                                // take a long time to start authentication, which can
                                                                // make them look like NON-HDCP sources, resulting in snow
#define DEM_EXTENDED_MUTE       (800 / DEM_POLLING_DELAY )      // Extend the MUTE delay for sources that have a long
                                                                // delay between R0 and the first CTL3, or do not send
                                                                // the CTL3 at all (one STB does this)

#define DEM_HPD_ACTIVE          0x80                            // SI_DeviceEventMonitor return bit indicating HPD is low
#define DEM_MUTE_ACTIVE         0x40                            // SI_DeviceEventMonitor return bit indicating MUTE should be asserted
#define DEM_PWR5V_PRESENT       0x20                            // SI_DeviceEventMonitor return bit indicating CABLE PRESENT

//------------------------------------------------------------------------------
//
//  Other Manifest Constants
//
//------------------------------------------------------------------------------

#define DEM_POLLING_DELAY   100         // Main Loop Polling interval (ms)

typedef enum
{
    SI_DEV_INPUT_CONNECTED  = 1,
    SI_DEV_BOOT_STATE_MACHINE,
    SI_DEV_NVRAM,
    SI_DEV_IDH,
    SI_DEV_IDL,
    SI_DEV_REV,
    SI_DEV_ACTIVE_PORT
} SI_DEV_STATUS;

enum 
{
    SI_PORTCONTROL_OFF      = 0x00,
    SI_PORTCONTROL_ON       = 0xFF,
    SI_PORTCONTROL_INACTIVE = 0x02
};

enum 
{
    SI_PORT_0   = 0,
    SI_PORT_1   = 1,
    SI_PORT_2   = 2,
    SI_PORT_3   = 3,
    SI_PORT_4   = 4,
    SI_PORT_VGA = 5,

    SI_PORT_ALL = 0xFF
};


    /* SI_DEV_BOOT_STATE_MACHINE bit definitions.   */

#define	BSM_BOOT_DONE   0x04
#define	BSM_BOOT_ERROR  0x03

//------------------------------------------------------------------------------
// Global Device Event Monitor variables
//------------------------------------------------------------------------------

extern uint8_t g_pass;
extern uint8_t g_outputMuted;

extern BOOL    g_stableNonHdcp;
extern BOOL    g_stableHdcp;

extern uint8_t g_mpSel0;
extern uint8_t g_mpSel1;
extern uint8_t g_decrypt0;
extern uint8_t g_decrypt1;
extern uint8_t g_auth0;
extern uint8_t g_portPwr5v;
extern uint8_t g_pipePwr5v;

extern uint8_t  g_devRev;
extern uint16_t g_devType;

//------------------------------------------------------------------------------
//  Low-level Function Prototypes
//------------------------------------------------------------------------------

void si_DeviceRXTermControl( uint8_t portIndex, BOOL enableRX );
void si_DeviceHpdControl( uint8_t portIndex, BOOL enableHPD );
void si_DeviceHdcpControl( uint8_t portIndex, BOOL enableHDCP );


//-------------------------------------------------------------------------------
//  API Function Prototypes
//-------------------------------------------------------------------------------

uint8_t SI_DeviceStatus( uint8_t statusIndex );
void    SI_DeviceInitRegisters ( void );
void    SI_DeviceMute( BOOL doMute );
void    SI_DeviceEnableOutput( BOOL enable );
void    SI_DeviceStandbyMode( void );
uint8_t SI_DevicePowerUpBoot( void );
uint8_t SI_DeviceEventMonitor( void );
BOOL    SI_DeviceBootComplete( void );

uint8_t SI_PortGetNextPort( uint8_t currentPort );
void    SI_PortSelectSource( uint8_t portIndex );
void    SI_PortEnable( uint8_t portIndex, uint8_t newState );

#endif  // __SI_API9287_H__
