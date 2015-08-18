/******************************************************************************
 * Copyright © 2008, Silicon Image, Inc.  All rights reserved.
 *
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of: Silicon Image, Inc.,
 * 1060 East Arques Avenue, Sunnyvale, California 94085
 ******************************************************************************/
/***************************************************************************//**
 * @file siInputHdmi.h
 * @brief Silicon Image Input Processor hdmi receiver interface
 ******************************************************************************/
#ifndef __SI_RX_LED_H__
#define __SI_RX_LED_H__

#include <si_datatypes.h>
#include <Si_hal.h>


#define LED_0 	P3^4        //
#define LED_1   P3^5        //
#define LED_2   P1^4        //

//sbit gpioLedArc   = LED_0;
//sbit gpioLedeHdmi  = LED_1;
//sbit LED_CTRL = LED_2;


#define LED_REG_INPUT_PORT        0
#define LED_REG_OUTPUT_PORT      1
#define LED_REG_POLARITY_PORT     2
#define LED_REG_CONFIG_PORT       3


#define LED_I2C_SLAVE_ADDRESS   0x38
#define ledI2cWriteByte(cmd, data)              HalI2cBus0WriteByte(LED_I2C_SLAVE_ADDRESS, cmd, data);


void ledInit(void);
void ledControl(uint8_t ledPort);
void ArcLed_On();
void ArcLed_Off();
void eHdmiLed_On();
void eHdmiLed_Off();


#endif  // __SI_RX_H__

