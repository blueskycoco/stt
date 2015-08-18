/******************************************************************************
 * Copyright © 2008, Silicon Image, Inc.  All rights reserved.
 *
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of: Silicon Image, Inc.,
 * 1060 East Arques Avenue, Sunnyvale, California 94085
 ******************************************************************************/
/***************************************************************************//**
 * @file ledDriver.c
 * @brief Silicon Image Input Processor Analog Video Input main implementation file.
 ******************************************************************************/
#include <si_LedDriver.h>
#include <stdio.h>

/*******************************************************************************//**
 * @brief Config the all the ports for PCA9555 chip
 * @param None
 **********************************************************************************/

void ledInit(void)
{
    //Reset the chip
    //LED_CTRL = 0;
    //put some delay here.
    //LED_CTRL = 1;

   //Set Polarity to not inverted
    ledI2cWriteByte(LED_REG_POLARITY_PORT, 0);

    //Set direction of the ports as an output pin.
    ledI2cWriteByte(LED_REG_CONFIG_PORT, 0);

    //Turn off all the LEDs at power up
    ledI2cWriteByte(LED_REG_OUTPUT_PORT, 0xE8);

}


/*******************************************************************************//**
 * @brief Turn on or off the LED
 **********************************************************************************/
void ledControl(uint8_t ledPort)
{

	if (ledPort > 8)
   		ledPort |= 0x10;
   	ledPort &= ~0x08;
    ledPort = ~ledPort;

    //printf("\nledPort: 0x%x", (int)ledPort);
    ledI2cWriteByte(LED_REG_OUTPUT_PORT, ledPort);
}

/*******************************************************************************//**
 * @brief Turn on the ARC LED
 **********************************************************************************/
void ArcLed_On()
{
    //gpioLedArc=0;
}

/*******************************************************************************//**
 * @brief Turn off the ARC LED
 **********************************************************************************/
void ArcLed_Off()
{
    //gpioLedArc=1;
}


/*******************************************************************************//**
 * @brief Turn on the ARC LED
 **********************************************************************************/
void eHdmiLed_On()
{
    //gpioLedeHdmi=0;
}

/*******************************************************************************//**
 * @brief Turn off the ARC LED
 **********************************************************************************/
void eHdmiLed_Off()
{
    //gpioLedeHdmi=1;
}

