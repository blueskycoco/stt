//***************************************************************************
//!file     si_9287regs.h
//!brief    SiI9287 Device Register Manifest Constants.
//
// No part of this work may be reproduced, modified, distributed, 
// transmitted, transcribed, or translated into any language or computer 
// format, in any form or by any means without written permission of 
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2007-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_9287REGS_H__
#define __SI_9287REGS_H__

enum 
{
    SI_DT_9187      = 0x9187,
    SI_DT_9287      = 0x9287,
    SI_DT_9289      = 0x9287
};

enum 
{
    SI_REV_ES       = 0x02,
    SI_REV_A,
    SI_REV_B
};

#define SET_BITS    0xFF
#define CLEAR_BITS  0x00

//------------------------------------------------------------------------------
// NOTE: Register addresses are 16 bit values with page and offset combined.
//
// Examples:  0x005 = page 0, offset 0x05
//            0x1B6 = page 1, offset 0xB6
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Registers in Page 0  (0xB0/0xB2)
//------------------------------------------------------------------------------

#define REG_DEV_IDL_RX          0x002
#define REG_DEV_IDH_RX          0x003
#define REG_DEV_REV             0x004

// Software Reset Register
#define REG_SRST                0x005
#define BIT_SWRST_AUTO                  0x10    // Auto SW Reset
#define BIT_SWRST                       0x01


// System Status Register
#define REG_C0_STATE            0x006
#define BIT_PWR5V                       0x08
#define BIT_SCDT                        0x01
#define BIT_CKDT                        0x02
#define BIT_PWRON_STANDBY               0x20    // Set if NOT in standby
#define BIT_NOT_STANDBY                 0x20    // Set if NOT in standby
#define BIT_MHL                         0x80

// System Control register

#define REG_HDCP_RST            0x007           
#define BIT_NO_USE_SCDT                 0x04    // ES0.2 and above
#define BIT_HDCP_RST                    0x01

// System Control Register #1
#define REG_SYS_CTRL1           0x008
#define BIT_MASTER_INT_EN               0x01


// Port Switch Register
#define REG_SYS_SWTCHC          0x009
#define BIT_DDC0_EN                     0x10
#define BIT_DDC1_EN                     0x20
#define BIT_DDC2_EN                     0x40
#define BIT_DDC3_EN                     0x80
#define MSK_DDC_EN                      0xF0
#define VAL_DDC_DISABLED                0x00

// Port Switch Register	2
#define REG_SYS_SWTCHC2         0x00A
#define MSK_ALL_EN                      0x03
#define BIT_20MHZ_PWRDN_EN              0x20

#define REG_C0_SRST2            0x00B
#define BIT_CEC_SRST                    0x40
#define BIT_DMATCH_UPDT_EN              0x01    // ES0.2 and above

//Hot plug Control Register
#define REG_HP_CTRL             0x010
#define VAL_HP_PORT0_HI                 0x01
#define VAL_HP_PORT1_HI                 0x04
#define VAL_HP_PORT2_HI                 0x10
#define VAL_HP_PORT3_HI                 0x40
#define VAL_HP_PORT_ALL_HI              0x55
#define VAL_HP_PORT_ALL_LO              0x00

#define REG_SLAVE_ADDR0         0x016
#define REG_SLAVE_ADDR1         0x017
#define REG_SLAVE_ADDR2         0x018
#define REG_SLAVE_ADDR3         0x019
#define REG_SLAVE_ADDR4         0x015
#define REG_SLAVE_ADDR5         0x014
#define REG_SLAVE_ADDR6         0x011
#define REG_SLAVE_ADDR7         0x012
#define REG_SLAVE_ADDR8         0x013


#define REG_HDCP0_SHD_AKSV1     0x021
#define REG_HDCP0_SHD_AKSV2     0x022
#define REG_HDCP0_SHD_AKSV3     0x023
#define REG_HDCP0_SHD_AKSV4     0x024
#define REG_HDCP0_SHD_AKSV5     0x025

#define REG_HDCP_SHD_AN1        0x026

#define REG_HDCP0_BCAPS_SET     0x02E
#define BIT_HDMI_CAPABLE                0x80
#define BIT_I2C_FAST                    0x10

// HDCP Debug Register
#define REG_BSTATUS2            0x030
#define BIT_HDMI_MODE                   0x10

#define REG_HDCPCTRL            0x031
#define BIT_CLEAR_RI                    0x80

//HDCP Status
#define REG_HDCP0_STAT          0x032
#define BIT_AUTH_DONE                   0x10
#define BIT_DECRYPTING                  0x20

//Pre-authentication Control Register
#define REG_PAUTH_CTRL          0x03A
#define BIT_PORT_FSM_EN                 0x01
#define BIT_SKIP_NON_HDCP               0x02
#define BIT_IGNORE_PAUTH_HPD            0x08
#define BIT_USE_AV_MUTE                 0x10
#define BIT_PAUTH_HPD_CON_EN            0x40
#define BIT_MP_SWAP                     0x80

#define REG_PAUTH_STAT0         0x03B
#define MAIN_PIPE_MASK                  0x0F
#define ROVE_PIPE_MASK                  0xF0

//Pre-authentication Status Register
#define REG_PAUTH_STAT1         0x03C
#define BIT_DECRYPT_MASK                0xF0
#define BIT_AUTH_MASK                   0x0F

#define REG_PAUTH_MPOVR         0x03E       // Main pipe port select register (one-hot)
#define REG_PAUTH_RPOVR         0x03F       // Rove pipe port select register (one-hot)
#define BIT_PORT_0                      0x01
#define BIT_PORT_1                      0x02
#define BIT_PORT_2                      0x04
#define BIT_PORT_3                      0x08
#define BIT_PORT_MASK                   0x0F
#define BIT_ENABLE_OVR                  0x10

//Pre-authentication Reset Length Register
#define REG_PAUTH_RSTLEN0       0x040
#define REG_PAUTH_RSTLEN1       0x041
#define REG_PAUTH_RSTLEN2       0x042
#define REG_PAUTH_RSTLEN3       0x043

//Pre-Authentication Reset On/Off Difference between Analog/Digital Register
#define REG_PAUTH_RSTDIFF0      0x044
#define REG_PAUTH_RSTDIFF1      0x045
#define REG_PAUTH_RSTDIFF2      0x046
#define REG_PAUTH_RSTDIFF3      0x047

//Pre-Authentication Time Out Limit Register
#define REG_PAUTH_TMOUT0        0x048

//Pre-Authentication Port Kept Authenticated Time Register
#define REG_PAUTH_KEEPLEN0      0x04C

//Pre-Authentication Enough Limit Registers
#define REG_PAUTH_ENGHLIM0      0x050
#define REG_PAUTH_ENGHLIM1      0x051
#define REG_PAUTH_ENGHLIM2      0x052
#define REG_PAUTH_ENGHLIM3      0x053

#define PAUTH_HDMI_MODE         0x056
#define REG_HDMI_DETECTED               0x01

#define REG_PAUTH_ECC_CTRL      0x057
#define REG_EN_ECC                      0x10
#define BIT_EN_ECC                      0x10

#define REG_PAUTH_ECC_CHKTIME   0x058

#define REG_PAUTH_INV_CTRL      0x059
#define TX_BIT_INV                      0x80

#define REG_PAUTH_MP_AOVR       0x05A
#define REG_PAUTH_RP_AOVR       0x05B

#define REG_MP_DLY_STAT         0x05C
#define REG_P0_DLY_STAT         0x05D
#define REG_P1_DLY_STAT         0x05E
#define REG_P2_DLY_STAT         0x05F
#define REG_P3_DLY_STAT         0x060

#define REG_PAUTH_ECC_THRES0    0x061
#define REG_PAUTH_ECC_THRES1    0x062

#define REG_PAUTH_MISC_CTRL0    0x063
#define BIT_MATCH_IND_SEL               0x02
#define BIT_DIS_GEN_VS_CTL3             0x04
#define BIT_FIX_DELAY                   0x08
#define BIT_VIDEO_MUTE_SYNC             0x10
#define BIT_AUDIO_MUTE_SYNC             0x20
#define BIT_RECOV_EN                    0x40
#define BIT_USE_FRAME_ECC               0x80

#define REG_FRAME_ECC_THR		0x065
#define REG_PAUTH_ECC_ERR0		0x068
#define REG_PAUTH_ECC_ERR1		0x069

    /* Interrupt Control/Status */

#define REG_CH0_INTR_STATE      0x070
#define BIT_INTR                        0x01
#define BIT_INTR_GRP0                   0x02    // INTR1, INTR2
#define BIT_INTR_GRP1                   0x04    // INTR6, INTR7

#define REG_CH0_INT_CTRL        0x079
#define BIT_PHYS_INT_POL                0x02
#define BIT_DO_SW_INT                   0x08

#define REG_CH0_INTR1           0x071
#define REG_CH0_INTR1_MASK      0x075
#define BIT_PWR_STATUS_CHANGED          0x08

#define REG_CH0_INTR2           0x072
#define REG_CH0_INTR2_MASK      0x076
#define BIT_SCDT_CHG                    0x08
#define BIT_CKDT_CHG                    0x10
#define BIT_SW_INT                      0x20
#define BIT_HDMI_DETECT_INT             0x80

#define REG_CH0_INTR5           0x07B           // ES0.2 only
#define REG_CH0_INTR5_MASK      0x07D           // ES0.2 only

#define REG_CH0_INTR6           0x07C
#define REG_CH0_INTR6_MASK      0x07E
#define BIT_MP_DLY_CHG                  0x02    // ES0.2 only
#define BIT_5VPWR_CHANGE                0x01

#define REG_CH0_INTR7           0x090
#define REG_CH0_INTR7_MASK      0x092
#define BIT_CEC_FIFO_FULL               0x10
#define BIT_BOOT_DONE_INT               0x40


// TMDS Analog Control #2 Register
#define REG_TMDS_CCTRL2         0x081
#define BIT_OFFSET_COMP_EN              0x20
#define MSK_DC_CTL                      0x0F
#define VAL_DC_CTL_8BPP_1X              0x00
#define VAL_DC_CTL_8BPP_2X              0x02
#define VAL_DC_CTL_10BPP_1X             0x04
#define VAL_DC_CTL_12BPP_1X             0x05
#define VAL_DC_CTL_10BPP_2X             0x06
#define VAL_DC_CTL_12BPP_2X             0x07

#define REG_TMDS_TERMCTRL0      0x082
#define VAL_TERM_ON                     0x00
#define VAL_TERM_MHL                    0x01
#define VAL_TERM_SURGE                  0x02
#define VAL_TERM_OFF                    0x03
#define VAL_TERM_MASK                   0x03


#define REG_TMDS_TERMCTRL1      0x083

#define CH_PD_SYS2              0x086
#define BIT_EN_TMDSRX0_MASK             0x0F    // Enable TMDS RX Ch0 mask (ports 3:0)
#define BIT_OVRHW_TMDSRX0_MASK          0xF0    // Overide h/w control of TMDS RX Ch0 mask (ports 3:0)

#define CH_PD_SYS3              0x087
#define BIT_EN_TMDSRX12_MASK            0x0F    // Enable TMDS RX Ch1 and Ch2 mask (ports 3:0)
#define BIT_OVRHW_TMDSRX12_MASK         0xF0    // Overide h/w control of TMDS RX Ch1 and Ch2 mask (ports 3:0)

#define CH_PD_SYS4              0x088
#define BIT_EN_TMDSRXC_MASK             0x0F    // Enable TMDS RX Clock Channel mask (ports 3:0)
#define BIT_OVRHW_TMDSRXC_MASK          0xF0    // Overide h/w control of TMDS RX Clock Channel mask (ports 3:0)


// ACR Configuration Registers
#define REG__AACR_CFG1          0x088
#define REG__AACR_CFG2          0x089

//------------------------------------------------------------------------------
// Registers in Page 9      (0xE0)
//------------------------------------------------------------------------------

#define REG_EN_EDID             0x901
#define VAL_EN_DDC0                     0x01
#define VAL_EN_DDC1                     0x02
#define VAL_EN_DDC2                     0x04
#define VAL_EN_DDC3                     0x08
#define VAL_EN_DDC4                     0x10
#define VAL_EN_DDC_VGA                  0x10
#define VAL_EN_DDC_NONE		            0x00 
#define VAL_EN_DDC_ALL                  0x1F

#define REG_EDID_FIFO_ADDR      0x902
#define VAL_FIFO_ADDR_00                0x00

#define REG_EDID_FIFO_DATA      0x903
#define REG_EDID_FIFO_SEL       0x904
#define BIT_SEL_DEVBOOT                 0x20
#define BIT_SEL_EDID0                   0x01
#define BIT_SEL_EDID_VGA                0x10
#define BIT_SEL_EDID1                   0x02

#define REG_NVM_COMMAND         0x905
#define VAL_PRG_EDID                    0x03
#define VAL_PRG_DEVBOOT                 0x04
#define VAL_COPY_EDID                   0x05
#define VAL_COPY_DEVBOOT                0x06

#define REG_NVM_COPYTO          0x906
#define VAL_NVM_COPYTO_MASK             0x1F
#define VAL_NVM_COPYTO_PORT0            0x01

#define REG_NVM_COMMAND_DONE    0x907
#define BIT_NVM_COMMAND_DONE            0x01

#define REG_BSM_INIT            0x908
#define BIT_BSM_INIT                    0x01

#define REG_BSM_STAT            0x909
#define	BIT_BOOT_DONE                   0x04
#define	BIT_BOOT_ERROR                  0x03

#define REG_NVM_STAT            0x910
#define VAL_NVM_EDID_VALID              0x01
#define VAL_NVM_DEVBOOT_VALID           0x02
#define VAL_NVM_VALID                   ( VAL_NVM_EDID_VALID | VAL_NVM_DEVBOOT_VALID )

#define REG__NVM_BOOT_SEL       0x912
#define VAL__NVM_BOOT_SEL_MASK          0x1F

#define REG_HPD_HW_CTRL         0x913
#define MSK_INVALIDATE_ALL              0xF0

#define REG_CECPA_ADDR          0x91A

#define REG_CECPAD_L_CH0        0x91C
#define REG_CECPAD_H_CH0        0x91D
#define REG_CHECKSUM_CH0        0x92C

#define REG_AUTO_CONFIG         0x940
#define BIT_AUTO_TERM_EN                0x01

#define REG_POR_CTRL            0x975
#define MSK_PDD_THRESHOLD               0xF0
#define MSK_AON_THRESHOLD               0x0F

#define REG_REGUL_PWR_ENABLE    0x978
#define BIT_PEN_RX12V                   0x01
#define BIT_NEN_RX12V                   0x02
#define BIT_PEN_TX12V                   0x04
#define BIT_NEN_TX12V                   0x08
#define BIT_PEN_LOGIC12V                0x10
#define BIT_NEN_LOGIC12V                0x20

#define REG_REGUL_VOLT_CTRL     0x97A
#define MSK_VOLT_CTRLB                  0x38
#define MSK_VOLT_CTRL                   0x07

#define REG_REGUL_BIAS_CTRL     0x97B
#define MSK_BIAS_CTRL                   0x07

#define REG_CLKDETECT_STATUS    0x9D0
#define BIT_CKDT_0                      0x01
#define BIT_CKDT_1                      0x02
#define BIT_CKDT_2                      0x04
#define BIT_CKDT_3                      0x08
#define VAL_CKDT_MASK                   0x0F

#define REG_PWR5V_STATUS        0x9D1
#define BIT_PWR5V_0                     0x01
#define BIT_PWR5V_1                     0x02
#define BIT_PWR5V_2                     0x04
#define BIT_PWR5V_3                     0x08

#define REG_HDMIM_CP_CTRL       0x9D2
#define BIT_MHL_FORCE0                  0x01
#define BIT_MHL_FORCE1                  0x02
#define BIT_MHL_FORCE2                  0x04
#define BIT_MHL_FORCE3                  0x08

#define REG_HDMIM_CP_INTR       0x9D3
#define BIT_BUS_0_INT                   0x01
#define BIT_BUS_1_INT                   0x02
#define BIT_BUS_2_INT                   0x04
#define BIT_BUS_3_INT                   0x08
#define VAL_BUS_INT_MASK                0x0F

#define REG_HDMIM_CP_PAD_STAT   0x9D7
#define BIT_MHL_PORT0                   0x01
#define BIT_MHL_PORT1                   0x02
#define BIT_MHL_PORT2                   0x04
#define BIT_MHL_PORT3                   0x08
#define BIT_20MHZ_OSC_PWR_SAVE          0x10

#define REG_SPECIAL_PURPOSE     0x9FF
#define BIT_HARDRESET                   0x80

//------------------------------------------------------------------------------
// Registers in Page 10         (0x64)
//------------------------------------------------------------------------------

#define REG_TMDS1_CCTRL1        0xA00

#define REG_PLL1_ZONECTL        0xA01
#define DIV20_CTRL_MASK                 0x30
#define DIV20_CTRL_VAL                  0x10

#define REG_TMDS1_CLKDETECT_CTL 0xA04

#define REG_PLL1_CALREFSEL      0xA07
#define CALREFSEL_MASK                  0x0F
#define CALREFSEL_VAL                   0x03

#define REG_PLL1_ICPCNT         0xA08
#define PLL_SZONE_MASK                  0xC0
#define PLL_SZONE_VAL                   0x40

#define REG_PLL1_SPLLBIAS       0xA09
#define REG_PLL1_VCOCAL         0xA0A

#define REG_EQ1_I2C             0xA11
#define REG_EQ1_DATA0           0xA12
#define REG_EQ1_DATA1           0xA13
#define REG_EQ1_DATA2           0xA14
#define REG_EQ1_DATA3           0xA15
#define REG_EQ1_DATA4           0xA16
#define REG_EQ1_DATA5           0xA17
#define REG_EQ1_DATA6           0xA18
#define REG_EQ1_DATA7           0xA19

#define REG_BW1_I2C             0xA21

#define REG_TMDS1_CNTL          0xA35
#define BV_SEL_I2C                      0x04
#define EQ_SEL_I2C                      0x01
#define BW_SCAN                         0x40
#define BV_SEL_SCAN                     0x00
#define EQ_SEL_SCAN                     0x00
#define REG_TMDS1_CNTL2         0xA36

#define TMDS1_BIST_TEST_SEL     0xA64

#define REG_ECC_ERRCOUNT        0xA67
#define BIT_ECC_CLR                     0x10

#define REG_TMDS0_CCTRL1        0xA80
#define VAL_CLK_MASK                    0x30
#define VAL_CLK_NORMAL                  0x00
#define VAL_CLK_EARLY                   0x10
#define VAL_CLK_DELAYED                 0x20
#define VAL_CLK_INVERTED                0x30


#define REG_PLL0_ZONECTL        0xA81
#define REG_TMDS0_CLKDETECT_CTL 0xA84
#define REG_PLL0_CALREFSEL      0xA87
#define REG_PLL0_ICPCNT         0xA88
#define REG_PLL0_SPLLBIAS       0xA89
#define REG_PLL0_VCOCAL         0xA8A

#define REG_EQ0_I2C             0xA91
#define REG_EQ0_DATA0           0xA92
#define REG_EQ0_DATA1           0xA93
#define REG_EQ0_DATA2           0xA94
#define REG_EQ0_DATA3           0xA95
#define REG_EQ0_DATA4           0xA96
#define REG_EQ0_DATA5           0xA97
#define REG_EQ0_DATA6           0xA98
#define REG_EQ0_DATA7           0xA99

#define REG_BW0_I2C             0xAA1

#define REG_TMDS0_CNTL          0xAB5
#define REG_TMDS0_CNTL2         0xAB6


#define REG_DPLL_MULTZONE_RDLY0 0xAC8
#define REG_DPLL_MULTZONE_RDLY1 0xAC9


//------------------------------------------------------------------------------
// Registers in Page 11     (0x90)
//------------------------------------------------------------------------------

#define REG_TMDST_CTRL1         0xB00
#define MSK_TMDS_EN_ALL                 0x0F

#define REG_TMDST_CTRL2         0xB01

#define REG_TMDST_CTRL3         0xB02
#define MSK_TMDS_TERM_EN_ALL            0x0F

#define REG_SYS_CTRL2           0xB03           // ES0.2 and above
#define BIT_CHEAP_HPD_ALT               0x80
#define BIT_PWR_AS_USEL                 0x40
#define BIT_DLY_CHG_RST_EN              0x20
#define BIT_DLY_CHG_INTR_CLRB           0x10
#define MSK_OVR_LOW_BW_EN               0x0C
#define MSK_OVR_LOW_BW_VAL              0x03

#define REG_SYS_CTRL3           0xB04           // ES0.2 and above
#define BIT_RST_N_DCKFIFO               0x02
#define BIT_CLR_ONLY_IN_DVI             0x01

//------------------------------------------------------------------------------
// Registers in Page 12     (0xE6)
//------------------------------------------------------------------------------

#define REG_CBUS0_INTR_STATUS   0xC08
#define BIT_CONNECT_CHG                 0x01

#define REG_CBUS0_BUS_STATUS    0xC0A
#define BIT_CONNECTED                   0x01


#define REG_CBUS1_INTR_STATUS   0xC48
#define REG_CBUS1_BUS_STATUS    0xC4A

#define REG_CBUS2_INTR_STATUS   0xC88
#define REG_CBUS2_BUS_STATUS    0xC8A

#define REG_CBUS3_INTR_STATUS   0xCC8
#define REG_CBUS3_BUS_STATUS    0xCCA




#endif  // __SI_9287REGS_H__
