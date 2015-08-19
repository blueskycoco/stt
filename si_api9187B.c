//**************************************************************************
//!file     si_api.c
//!brief    SiI9287 API main functions.
//
// Note:    This file supports 9187B 
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_api9x87.h"
#include "si_edid.h"
#include "si_regio.h"
#if (API_DEBUG_CODE==1)
    #include <stdio.h>
#endif

//------------------------------------------------------------------------------
// Target System specific
//------------------------------------------------------------------------------

#if (FPGA_DEBUG_CODE == 1)
    char g_strDeviceID []   = "9187B(FPGA)";
#else
    char g_strDeviceID []   = "9187B";
#endif

uint8_t g_devRev                    = SI_REV_B;
uint16_t g_devType                  = SI_DT_9187;

//------------------------------------------------------------------------------
// Debug Trace
//------------------------------------------------------------------------------

#if (API_DEBUG_CODE==1)
    uint8_t     g_demFlags1           = 0;
    uint8_t     g_demFlags2           = DF2_FIRST_PASS | DF2_MUTE_ON;
#endif

//------------------------------------------------------------------------------
// Device Event Monitor variables
//------------------------------------------------------------------------------

uint8_t         g_pass          = 0;


BOOL            g_stableNonHdcp = false;
BOOL            g_stableHdcp    = false;
static BOOL     l_stableHdcp1   = false;
static BOOL     l_firstPass     = true;
static BOOL     l_hdcpFound     = false;

uint8_t         g_outputMuted   = DEM_MUTE_LENGTH;      // If non-zero, the output is muted

static uint8_t  l_userPortSelect    = 0;    			// Index of port selected by user.

static uint8_t  l_hdcpGood      = 0x0F;     // If bit is set, corresponding port does not need
                                            // to be reauthenticated upon a switch to main pipe
static uint8_t  l_hpdToggle     = 0;

uint8_t g_mpSel0                = 0x00;
uint8_t g_mpSel1                = 0x00;












uint8_t         g_decrypt0      = 0;
uint8_t         g_decrypt1      = 0;
uint8_t         g_auth0         = 0;
static uint8_t  l_standbyOff0   = 0;
static uint8_t  l_scdtCount     = 0;
static uint8_t  l_scdt          = 0;
static uint8_t  l_scdtInt       = 0;
static uint8_t  l_ckdt          = 0;

uint8_t         g_portPwr5v     = 0;
uint8_t         g_pipePwr5v     = 0;

static uint8_t  l_hpdTimer      = DEM_MAX_POWERUP_HOTPLUG;

//------------------------------------------------------------------------------
//  Function:       SI_DeviceBootComplete 
//  Description:    Wait for EDID Boot Done status.
//------------------------------------------------------------------------------

BOOL SI_DeviceBootComplete (int index )
{
    uint8_t test;
    BOOL    success = true;

    /* Wait for boot loading to be done.    */

    HalTimerSet( ELAPSED_TIMER, 1 );
    for ( ;; )
    {
        test = SiIRegioRead(index, REG_BSM_STAT );
        if ( test & BIT_BOOT_DONE )
            break;
        if (( test & BIT_BOOT_ERROR ) || ( HalTimerElapsed() >= 4000 ))    // 4 second timeout
        {
            success = false;
            break;
        }
    }

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SI_DevicePowerUpBoot
// Description: This function should be called at Always On Domain powerup to
//              assure that the device is properly initialized.
//------------------------------------------------------------------------------

uint8_t SI_DevicePowerUpBoot ( int index )
{
    uint8_t u8Data;
    uint8_t baseAddress = 0;

    u8Data = SiIRegioRead(index, REG_DEV_IDH_RX );    // Throw away first I2C read (chip issue)

    /* Determine SiI9287 page 0 device address. */

    u8Data = SiIRegioRead(index, REG_DEV_IDH_RX );
    if (( u8Data != 0x92 ) && ( u8Data != 0x91 ))
    {
        /* If page 0 access failed at 0xB0 default, try it at device ID 0xB2.   */

        SiIRegioSetBase(index, 0, 0xB2 );
        u8Data = SiIRegioRead(index, REG_DEV_IDH_RX );
        if (( u8Data == 0x92 ) || ( u8Data == 0x91 ))
        {
            baseAddress = 0xB2;
        }
    }
    else
    {
        baseAddress = 0xB0;
    }

    if ( baseAddress != 0 )
    {
        /* Initialize page 9 address in case chip reset did not happen properly.    */

        SiIRegioWrite(index, REG_SLAVE_ADDR3, 0xE0 );

        /* Toggle 1.2V logic power supply enable to force reset of Power Down Domain logic. */

        SiIRegioModify(index, REG_REGUL_PWR_ENABLE, BIT_PEN_LOGIC12V, CLEAR_BITS );
        SiIRegioModify(index, REG_REGUL_PWR_ENABLE, BIT_PEN_LOGIC12V, SET_BITS );

        /* Force a soft Hard Reset to ensure that the Always On Domain logic is reset.  */

        SiIRegioBitToggle(index, REG_SPECIAL_PURPOSE, BIT_HARDRESET );

        /* Wait for boot loading to be done.    */

        if ( SI_DeviceBootComplete(index) )
        {
            /* Check NVRAM status to determine if it must be initialized (first time boot). */

#if (DO_EDID_INIT == 1)
            if ( !SI_EdidInitialize(index))
            {
                baseAddress = 0x02;     // NVRAM initialize failed.
            }
            else
#endif
            {
                /* Initialize registers to the Programmers Reference power-up defaults. */

                SI_DeviceInitRegisters(index);
            }
            }
        else
            baseAddress = 0x01;
    }

    return( baseAddress );
}

//------------------------------------------------------------------------------
// Function:    SI_DeviceInitRegisters
// Description: Initialize registers that need to be set to non-default values
//              at startup.  In general, these registers are not changed
//              after startup.
//
// NOTE: 9287 Output is muted at the exit of this function.
//       If MonitorMute() is not used, this function must 
//       be changed to un-mute on exit.
//
//------------------------------------------------------------------------------

void SI_DeviceInitRegisters ( int index )
{
#if (FPGA_DEBUG_CODE == 1)
    SiIRegioSetBase(index, 10, 0x68 );
#endif

    /* Set up 9287B special registers and mute whilst doing the rest.   */

    SiIRegioWrite(index, 
        REG_PAUTH_MISC_CTRL0, 
        BIT_USE_FRAME_ECC | BIT_MATCH_IND_SEL | BIT_DIS_GEN_VS_CTL3 | BIT_FIX_DELAY | BIT_RECOV_EN | BIT_VIDEO_MUTE_SYNC | BIT_AUDIO_MUTE_SYNC 
        );

    /* Turn off port finite state machine and hold device in    */
    /* software reset until finished update.                    */

    SiIRegioWrite(index, REG_PAUTH_CTRL, BIT_MP_SWAP | BIT_IGNORE_PAUTH_HPD | BIT_SKIP_NON_HDCP  );
    SiIRegioModify(index, REG_SRST, BIT_SWRST, SET_BITS );

    SiIRegioModify(index, REG_HDCP_RST, BIT_HDCP_RST, SET_BITS   ); // HDCP arbitration and OTP reset
    SiIRegioModify(index, REG_HDCP_RST, BIT_HDCP_RST, CLEAR_BITS ); // Release

    /* Change default CBUS link bit time.   */

    SiIRegioWrite(index, 0xC3A, 0x1A );
    SiIRegioWrite(index, 0xC7A, 0x1A );
    SiIRegioWrite(index, 0xCBA, 0x1A );
    SiIRegioWrite(index, 0xCFA, 0x1A );

    SiIRegioModify(index, REG_TMDS0_CCTRL1, VAL_CLK_MASK, VAL_CLK_INVERTED );
    SiIRegioModify(index, REG_TMDS1_CCTRL1, VAL_CLK_MASK, VAL_CLK_INVERTED );

    SiIRegioWrite(index, REG_SYS_CTRL2, CLEAR_BITS );

    SiIRegioWrite(index, REG_FRAME_ECC_THR,    0x08 ); // Consecutive 8 frames
    SiIRegioWrite(index, REG_PAUTH_ECC_THRES0, 0x0F ); // 15 ECC errors in one frame
    SiIRegioWrite(index, REG_PAUTH_ECC_THRES1, 0x00 ); // 
    
    SiIRegioWrite(index, REG_PAUTH_RSTLEN0,  0x81 ); // 120ms since there is no FW involvement
    SiIRegioWrite(index, REG_PAUTH_RSTLEN1,  0xa9 ); // 120ms is 7.5 frames for 60Hz and 3 frames for 25Hz
    SiIRegioWrite(index, REG_PAUTH_RSTLEN2,  0x03 );
    SiIRegioWrite(index, REG_PAUTH_RSTLEN3,  0x00 );

    SiIRegioWrite(index, REG_PAUTH_RSTDIFF0, 0x80 );
    SiIRegioWrite(index, REG_PAUTH_RSTDIFF1, 0xa9 );
    SiIRegioWrite(index, REG_PAUTH_RSTDIFF2, 0x03 );
    SiIRegioWrite(index, REG_PAUTH_RSTDIFF3, 0x00 );

    /* Enable EDID and HPD manually so that when we release Auto-HPD,   */
    /* the power-up state is maintained.                                */

    SiIRegioWrite(index, REG_EN_EDID, VAL_EN_DDC_ALL );           // Enable all EDID
    SiIRegioWrite(index, REG_HP_CTRL, VAL_HP_PORT_ALL_HI );       // Enable all HPD
    SiIRegioWrite(index, REG_HPD_HW_CTRL, MSK_INVALIDATE_ALL );   // Disable Auto-HPD control

    /* Turn off CEC auto response to <Abort> command.   */

    SiIRegioWrite(index, 0x8DF, CLEAR_BITS );

    SiIRegioWrite(index, REG_TMDST_CTRL1, MSK_TMDS_EN_ALL );  	    // Enable TX.

#if (FPGA_DEBUG_CODE==1)
    {
        int i;

        SI_EdidWrite(index, EDID_BUFFER, g_edidFPGAEdidTable );
        for ( i = 0; i < 4; i++ )
        {
            SI_EdidLoadPortRam(index, EDID_BUFFER, i );
        }
    }
#endif

#if (DO_VGA_EDID_INIT == 1)

    /* The VGA EDID memory must be loaded manually. */

    SI_EdidWrite(index, EDID_BUFFER, g_edidFlashEdidVgaTable );
    SI_EdidLoadPortRam(index, EDID_BUFFER, EDID_RAM_VGA );
#endif

    /* Release software reset and finite state machine. */

    SiIRegioModify(index, REG_SRST, BIT_SWRST, CLEAR_BITS );
    HalTimerWait( 120 );    // Allow software reset to settle before enabling FSM
    SiIRegioWrite(index, REG_PAUTH_CTRL, BIT_MP_SWAP | BIT_IGNORE_PAUTH_HPD | BIT_SKIP_NON_HDCP | BIT_PORT_FSM_EN  );
    
    l_firstPass = true;
    l_hpdTimer    = DEM_MAX_POWERUP_HOTPLUG;
}

//------------------------------------------------------------------------------
// Function:    MonitorHPDState
// Description: For the port connected to the main pipe, turn on HPD when 
//              required, based on the results of the latest HDCP-Good testing 
//              OR if HPD and termination are off, turn them back on after the 
//              HPD low period has expired.
//------------------------------------------------------------------------------

static void MonitorHPDState ( int index )
{
    uint8_t         selectEqual0, selectOneHot;
    static uint8_t  hdcpPass        = 0;
    static BOOL     selectEqual1    = false;

    /* Keep track of the number of polling periods since a port switch  */
    /* or loss of SCDT.  Don't let it wrap around to zero because that  */
    /* indicates to the code that a re-auth may be needed.              */

    if ( hdcpPass < DEM_AUTH_TIME)
        hdcpPass++;
    if ( !g_pipePwr5v || ( g_mpSel0 != g_mpSel1 ) || !l_standbyOff0 )
        hdcpPass = 0;

    /* This section of code is executed when the HPD signal has already been forced */
    /* low by the firmware.  Basically, it sequences the RX termination, HDCP DDC   */
    /* access, and HPD signal to an active state.                                   */

    if ( l_hpdToggle )
    {
        l_hpdToggle--;
        if ( l_hpdToggle == DEM_RXTERM_TOGGLE_TIME )
        {
            si_DeviceRXTermControl(index, l_userPortSelect, true );   // RX Termination ON on main port
        }
        if ( l_hpdToggle == 0 )
        {
            si_DeviceHpdControl(index, 0xFF, true );                  // HPD --> 1 for all ports
            si_DeviceHdcpControl(index, l_userPortSelect, true );     // HDCP DDC Access enabled on main port

            DEBUG_PRINT( MSG_DBG, ( "\n[%d] HPD HI", (int)g_pass ));
        }
    }

    /* This section of the code tests to see if an HPD toggle is required.*/

    else
    {
        selectOneHot = (1 << l_userPortSelect);
        selectEqual0 = (g_mpSel0 == selectOneHot);      // Is current USER_SEL == MAINPIPE_SEL?

        if ( g_mpSel0 != 0)
        {
            /* If USEL == MPSEL and either first pass through or during the last pass USEL != MPSEL,    */
            /* check to see if we need to toggle HPD.                                                   */

            if ( selectEqual0 && ((hdcpPass == 1) || !selectEqual1 ))
            {
                /* Initiate a HPD toggle.  */

                if ( g_portPwr5v & g_mpSel0 )
                {
                    si_DeviceHpdControl(index, l_userPortSelect, false );         // HPD --> 0 for specified port
                    si_DeviceRXTermControl(index, SI_PORT_ALL, false );           // RX Termination OFF for all ports
                    si_DeviceHdcpControl(index, SI_PORT_ALL, false );             // HDCP DDC Access disabled for all ports

                    l_hpdToggle = DEM_HPD_TOGGLE_TIME;                      // Make a pulse
                    DEBUG_PRINT( MSG_DBG, ( "\n[%d] MPSEL %d HPD LOW", (int)g_pass, (int)g_mpSel0) );
                }
            }
        }
        selectEqual1 = selectEqual0;
    }
}

//------------------------------------------------------------------------------
// Function:    MonitorECC
// Description: 
//------------------------------------------------------------------------------

static void MonitorECC ( int index )
{
    static uint8_t  mpDecrypt = 0;
    static BOOL     riClearEnabled = false;

    /* When SCDT becomes stable in HDCP mode, wait another short period before enabling ECC     */
    /* RI-Clear functionality. This avoids unnecessary re-authentication attempts during        */
    /* unstable SCDT periods.  Don't need this for non-HDCP sources, since there is no RI.      */

    if ( g_stableHdcp && !riClearEnabled )
    {
        mpDecrypt++;
        if ( mpDecrypt >= DEM_DECRYPT_ECC_THRESHOLD )
        {
            mpDecrypt = DEM_DECRYPT_ECC_THRESHOLD;
            SiIRegioModify(index, REG_PAUTH_ECC_CTRL, REG_EN_ECC, SET_BITS ); // Enable ECC RI-Clear
            riClearEnabled = true;
        }
    }
    else
        mpDecrypt = 0;

    /* If SCDT has changed, or we have lost CKDT, or CTL3, disable ECC checking.   */

    if ( riClearEnabled && (l_scdtInt || !(l_ckdt & g_auth0 & g_decrypt0 & g_mpSel0) || (g_mpSel0 != g_mpSel1) ))
    {
        SiIRegioModify(index, REG_PAUTH_ECC_CTRL, BIT_EN_ECC, CLEAR_BITS );
        riClearEnabled = false;
    }
}   

//------------------------------------------------------------------------------
// Function:    MonitorMute
// Description: Determine when to mute or unmute the AV output
//------------------------------------------------------------------------------

static void MonitorMute ( int index )
{
static uint8_t  dly4nohdcp      = 0;
static uint8_t  no_ctl3tout     = 0;

    /* Mute the output if the situation calls for it.   */

    if ( g_outputMuted == 0 )
    {
        /* Determine if a mute is required. */

        if ( g_mpSel0 != g_mpSel1 && DEM_MUTE_LEN_PORTCHANGE != 0 )    // port change
            g_outputMuted = DEM_MUTE_LEN_PORTCHANGE;
	    else if ( !g_pipePwr5v || (!l_scdt || l_scdtInt) && (g_mpSel0 & g_mpSel1))    // non-port change mute situations
            g_outputMuted = DEM_MUTE_LENGTH;
	    else if ( l_hdcpFound && !g_stableHdcp )
            g_outputMuted = DEM_MUTE_LENGTH;

#if (USE_INTERNAL_MUTE == 1)
        if ( g_outputMuted )
        {
            SI_DeviceMute(index, true );
        }
#endif
    }

    /* If output muted, turn it back on after the delay period (assuming we're active). */

    else
    {
        if (( g_outputMuted == 1 ) && ( g_stableHdcp || g_stableNonHdcp ))
        {
            if ( l_hpdToggle == 0 )
            {
#if (USE_INTERNAL_MUTE == 1)
                SI_DeviceMute(index, false );
#endif
                DEBUG_PRINT( MSG_DBG, ( "\n[%d] MUTE OFF: scdt:%02X AUTH: %02X DECRYPT: %02X", (int)g_pass, (int)l_scdt, (int)g_auth0, (int)g_decrypt0 ) );
                g_outputMuted--;
                no_ctl3tout = 0;
                dly4nohdcp = 0;
            }
        }
        else if ( g_outputMuted )
        {
            /* If HDCP and stable, just decrement the count.    */

            if ( g_stableHdcp ) 
            {
                DEBUG_PRINT( MSG_DBG, ( "\n[%d] MUTE--", (int)g_pass ) );
                g_outputMuted--;
                no_ctl3tout = 0;
            }

            /* If not HDCP, but stable output, multiply the delay by 4, because */
            /* this may turn out to be a slow-authenicating HDCP source.  If    */
            /* that is the case, g_stableNonHdcp will change to g_stableHdcp    */
            /* and the other case will take over.                               */

            else if ( g_stableNonHdcp )
            {
                if ( dly4nohdcp == DEM_MULTIPLIER_FOR_NONHDCP )
                {
                    DEBUG_PRINT( MSG_DBG, ( "\n[%d] NONHDCP MUTE--", (int)g_pass ) );
                    g_outputMuted--;
                    dly4nohdcp = 0;
                }
                else
                    dly4nohdcp++;
            }

            /* There is at least one HDCP set top box that does not send out    */
            /* the CTL3, but does send the AKSV and encrypted data.  This is    */
            /* where we check for that case.  This time we only extend the      */
            /* mute delay by 500ms.                                             */

            else if ( g_auth0 & g_mpSel0 )
            {
                if ( no_ctl3tout == DEM_EXTENDED_MUTE )
                {
                    if ( g_outputMuted > 1 )
                    {
                        DEBUG_PRINT( MSG_DBG, ( "\n[%d] AO MUTE--", (int)g_pass ) );
                        g_outputMuted--;
                    }
                }
                else
                    no_ctl3tout++;
            }
            else
                g_outputMuted = DEM_MUTE_LENGTH;
        }
    }
}

//------------------------------------------------------------------------------
// Function:    UpdateDeviceStatus
// Description: Update the snapshot shift register to maintain a record of
//              the state of these registers for the past X poll periods.
//              Get a snapshot of most registers we'll need for this
//              pass through the DeviceEventMonitor.
//
//------------------------------------------------------------------------------

static void UpdateDeviceStatus ( int index )
{
    uint8_t         uData;

    /* Update the snapshot shift register to maintain a record of   */
    /* the state of these registers for the past X poll periods.    */

    g_mpSel1      = g_mpSel0;
    g_decrypt1    = g_decrypt0;     l_stableHdcp1 = g_stableHdcp;

    /* Get a snapshot of most registers we'll need for this */
    /* pass through the DeviceEventMonitor.                 */

    g_portPwr5v         = SiIRegioRead(index, REG_PWR5V_STATUS ) & 0x0F;
    g_portPwr5v         |= SiIRegioRead(index, REG_HDMIM_CP_PAD_STAT ) & 0x0F;
    uData = SiIRegioRead(index, REG_PAUTH_STAT0  );     
        g_mpSel0        = uData & MAIN_PIPE_MASK;        
    uData = SiIRegioRead(index, REG_PAUTH_STAT1  ); 
        g_auth0         = uData & BIT_AUTH_MASK;         
        g_decrypt0      = (uData & BIT_DECRYPT_MASK) >> 4;
    uData = SiIRegioRead(index, REG_C0_STATE   );   
        l_standbyOff0   = uData & BIT_NOT_STANDBY;  
        g_pipePwr5v     = uData & ( BIT_PWR5V | BIT_MHL );
        l_scdt          = uData & BIT_SCDT;    
    l_ckdt              = SiIRegioRead(index, REG_CLKDETECT_STATUS ) & VAL_CKDT_MASK;
    l_scdtInt           = SiIRegioRead(index, REG_CH0_INTR2 ) & BIT_SCDT_CHG;
    if ( l_scdtInt )
    {
        SiIRegioWrite(index, REG_CH0_INTR2, BIT_SCDT_CHG );
    }

#if (API_DEBUG_CODE==1)
    g_demFlags1   &=  ~(DF1_SCDT_INT | DF1_SCDT_HI | DF1_MP_AUTH | DF1_MP_DECRYPT );
    g_demFlags1   |=  l_scdtInt ? DF1_SCDT_INT : 0x00;
    g_demFlags1   |=  l_scdt ? DF1_SCDT_HI  : 0x00;

    uData   = SiIRegioRead(index, REG_HDCP0_STAT );
    g_demFlags1   |=  (uData & BIT_AUTH_DONE)   ? DF1_MP_AUTH     : 0x00;
    g_demFlags1   |=  (uData & BIT_DECRYPTING)  ? DF1_MP_DECRYPT  : 0x00;
    uData   = SiIRegioRead(index, REG_BSTATUS2 );
    g_demFlags2   &=  ~DF2_HDMI_MODE;
    g_demFlags2   |=  (uData & BIT_HDMI_MODE) ? DF2_HDMI_MODE : 0x00;
#endif

}

//------------------------------------------------------------------------------
// Function:    SI_DeviceEventMonitor
// Description: Monitors important events in the life of the Main pipe and
//              intervenes where necessary to keep it running clean.
//
// NOTE:        This function is designed to be called at 100 millisecond
//              intervals. If the calling interval is changed, the
//              DEM_POLLING_DELAY value must be changed also to maintain the
//              proper intervals.
//
//------------------------------------------------------------------------------

uint8_t SI_DeviceEventMonitor ( int index )
{
    uint8_t monitorState;

    g_pass++;

    UpdateDeviceStatus(index);       // Gather info for this pass

    if ( l_firstPass )
    {
        /* If a source is attached and powered when the TV is turned off, the   */
        /* state of the HDCP and RX termination enables can be in the wrong     */
        /* state. If this is the first pass after power on, we turn off every   */
        /* port control to fool any attached sources into thinking that they    */
        /* are not plugged into anything.  Then, after a short delay, we return */
        /* to normal to emulate turning on the TV.                              */

        if ( l_hpdTimer == 0 )
        {
            DEBUG_PRINT( MSG_ALWAYS, ( "\n[%d] firstpass hotplug off\n", (int)g_pass ));
            SiIRegioWrite(index, REG_EN_EDID, VAL_EN_DDC_ALL );           // Enable all EDID
            si_DeviceHpdControl(index, SI_PORT_ALL, true );               // Allow HPD to go high
            _DEBUG_( g_demFlags2   &=  ~DF2_FULL_HOTPLUG );
        }
        else 
        {
            if ( l_hpdTimer == DEM_MAX_POWERUP_HOTPLUG )
            {
                DEBUG_PRINT( MSG_ALWAYS, ( "\n[%d] firstpass hotplug ON\n", (int)g_pass ));
                si_DeviceHdcpControl(index, SI_PORT_ALL, false );         // Disable HDCP access
                si_DeviceRXTermControl(index, SI_PORT_ALL, false );       // Turn off RX termination
                SiIRegioWrite(index, REG_EN_EDID, VAL_EN_DDC_NONE );      // Disable all EDID
                si_DeviceHpdControl(index, SI_PORT_ALL, false );          // Drive HPD low
                _DEBUG_( g_demFlags2   |=  DF2_FULL_HOTPLUG );
            }
            l_hpdTimer--;
            return( true );
        }

        /* Turn on HDCP Access and RX termination for the selected port on the 9187 */

        si_DeviceHdcpControl(index, l_userPortSelect, true );
        si_DeviceRXTermControl(index, l_userPortSelect, true );

        l_firstPass = false;
        _DEBUG_( g_demFlags2   &=  ~DF2_FIRST_PASS);
    }

    /* Determine if SCDT is stable. */

    if ( l_scdt )
    {
        l_scdtCount++;
        if ( l_scdtCount >= DEM_SCDT_STABLE )
        {
            l_scdtCount = DEM_SCDT_STABLE;    // Keep from overflowing
            _DEBUG_( g_demFlags1   |=  DF1_SCDT_STABLE);
        }
    }
        /* Start SCDT stable check over again if SCDT interrupted or a port switch happened.   */

    if ( !l_scdt || ( g_mpSel0 != g_mpSel1 ) || l_scdtInt )
    {
        l_scdtCount = 0;
        _DEBUG_( g_demFlags1 &=  ~DF1_SCDT_STABLE );
        _DEBUG_( g_demFlags2 &=  ~DF2_HDCP_FOUND );

        l_hdcpFound = false;
    }

    /* Determine when input is stable for both HDCP and non-HDCP    */
    /* In can be difficult to tell the difference between stable    */
    /* non-HDCP sources and a HDCP source that has CKDT and SCDT,   */
    /* but not yet either AUTH or DECRYPT.                          */

	g_stableNonHdcp =  (!( g_auth0 & g_mpSel0) && g_mpSel0  && (l_scdtCount == DEM_SCDT_STABLE) && (l_hpdToggle == 0));
	g_stableHdcp    =  (( g_auth0 & g_decrypt0 & g_mpSel0 ) && (l_scdtCount == DEM_SCDT_STABLE) && (l_hpdToggle == 0));
	if ( g_stableHdcp )
    {
        l_hdcpFound = true;
        _DEBUG_( g_demFlags2 |= DF2_HDCP_FOUND );
    }

    _DEBUG_( g_demFlags1   &=  ~(DF1_HDCP_STABLE | DF1_NON_HDCP_STABLE | DF1_HPD));
    _DEBUG_( g_demFlags1   |=  g_stableHdcp ? DF1_HDCP_STABLE : 0x00);
    _DEBUG_( g_demFlags1   |=  g_stableNonHdcp ? DF1_NON_HDCP_STABLE : 0x00);

    /* The following functions must be called in the listed order.  */

    MonitorHPDState(index);
	MonitorMute(index);
	MonitorECC(index);

    /* The above functions must be called in the listed order.      */

    _DEBUG_( g_demFlags1   |=  (l_hpdToggle == 0) ? DF1_HPD  : 0x00 );

    /* Return mute and HPD state.   */

    monitorState = (l_hpdTimer != 0) ? DEM_HPD_ACTIVE : 0;
    monitorState |= (g_outputMuted != 0) ? DEM_MUTE_ACTIVE : 0;
    monitorState |= (g_pipePwr5v != 0) ? DEM_PWR5V_PRESENT : 0;
    return( monitorState );
}

//------------------------------------------------------------------------------
// Function:    SI_DeviceMute
// Description: Mute the device output.  The OEM can replace the guts of this
//              function with a call to the main mute function of the SOC
//              if desired.
//------------------------------------------------------------------------------

void SI_DeviceMute (int index, BOOL doMute )
{

    if ( doMute )
    {
        SiIRegioModify(index, REG_PAUTH_MISC_CTRL0, BIT_VIDEO_MUTE_SYNC | BIT_AUDIO_MUTE_SYNC, SET_BITS );

        DEBUG_PRINT( MSG_DBG, ( "\n[%d] ------------------ MUTE ON", (int)g_pass ));
        _DEBUG_( g_demFlags2   |=  DF2_MUTE_ON );
    }
    else
    {
        SiIRegioModify(index, REG_PAUTH_MISC_CTRL0, BIT_VIDEO_MUTE_SYNC | BIT_AUDIO_MUTE_SYNC, CLEAR_BITS );

        DEBUG_PRINT( MSG_DBG, ( "\n[%d] ------------------ MUTE OFF", (int)g_pass ));
        _DEBUG_( g_demFlags2   &=  ~DF2_MUTE_ON );
    }
}

//------------------------------------------------------------------------------
// Function:    SI_PortSelectSource
// Description: Connect the passed source port to the output.
//
//------------------------------------------------------------------------------

void SI_PortSelectSource (int index, uint8_t portIndex )
{
    /* Mute before port switch to prevent possible funny images.    */

#if ((USE_INTERNAL_MUTE == 1) && (DEM_MUTE_LEN_PORTCHANGE != 0))
    SI_DeviceMute(index, true );
#endif

    /* Switch user select to the requested port.  The HPD stuff will be handled in DEM if needed.   */

    SiIRegioModify(index, REG_SYS_SWTCHC2, MSK_ALL_EN, portIndex );
    l_userPortSelect = portIndex;
    g_demFlags2 |= DF2_PORT_SWITCH_REQ;
}

//------------------------------------------------------------------------------
// Function:    si_DeviceRXTermControl
// Description: Enable or disable RX termination for the selected port(s)
//------------------------------------------------------------------------------

void si_DeviceRXTermControl (int index, uint8_t portIndex, BOOL enableRX )
{
    uint8_t enableVal;

    enableVal = enableRX ? CLEAR_BITS : SET_BITS;     // RX Termination enabled == 0x00

    /* If caller wants all ports controlled at once...  */

    if ( portIndex == SI_PORT_ALL )
    {
        SiIRegioModify(index, REG_TMDS_TERMCTRL0, SET_BITS, enableVal );
    }
    else 
    {
        SiIRegioModify(index, REG_TMDS_TERMCTRL0, (VAL_TERM_MASK << ((portIndex % 4) * 2)), enableVal );
    }
}

//------------------------------------------------------------------------------
// Function:    si_DeviceHpdControl
// Description: Enable or disable HPD for the selected port(s)
//------------------------------------------------------------------------------

void si_DeviceHpdControl (int index, uint8_t portIndex, BOOL enableHPD )
{
    uint8_t enableVal;

    enableVal = enableHPD ? VAL_HP_PORT_ALL_HI : CLEAR_BITS;

    /* If caller wants all ports controlled at once...  */

    if ( portIndex == SI_PORT_ALL )
    {
        SiIRegioWrite(index, REG_HP_CTRL, enableVal );
    }
    else 
    {
        SiIRegioModify(index, REG_HP_CTRL, (0x03 << ((portIndex % 4) * 2)), enableVal );
    }
}

//------------------------------------------------------------------------------
// Function:    DeviceHdcpControl
// Description: Enable or disable HDCP access for the selected port(s)
//------------------------------------------------------------------------------

void si_DeviceHdcpControl (int index, uint8_t portIndex, BOOL enableHDCP )
{
    uint8_t enableVal;

    enableVal = enableHDCP ? SET_BITS : CLEAR_BITS;

    /* If caller wants all ports controlled at once...  */

    if ( portIndex == SI_PORT_ALL )
    {
        SiIRegioModify(index, REG_SYS_SWTCHC, MSK_DDC_EN, enableVal );
    }
    else 
    {
        SiIRegioModify(index, REG_SYS_SWTCHC, (BIT_DDC0_EN << portIndex), enableVal );
    }
}

//------------------------------------------------------------------------------
// Function:    SI_DeviceStandbyMode
// Description: Set standby mode.  To exit, reboot using SI_DevicePowerUpBoot()
//------------------------------------------------------------------------------

void SI_DeviceStandbyMode ( int index )
{
    si_DeviceRXTermControl(index, SI_PORT_ALL, false );   // Disable RX termination
    si_DeviceHdcpControl(index, SI_PORT_ALL, false );     // Disable HDCP DDC access
    si_DeviceHpdControl(index, SI_PORT_ALL, true );       // Enable HPD
    SiIRegioWrite(index, REG_EN_EDID, SET_BITS );         // Enable EDID DDC access
}
