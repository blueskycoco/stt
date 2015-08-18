//***************************************************************************
//!file     si_datatypes.h
//!brief    Silicon Image data type header (conforms to C99).
//
// No part of this work may be reproduced, modified, distributed, 
// transmitted, transcribed, or translated into any language or computer 
// format, in any form or by any means without written permission of 
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DATATYPES_H__
#define __SI_DATATYPES_H__

    /* C99 defined data types.  */

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;

typedef signed char    int8_t;
typedef signed short   int16_t;
typedef signed long    int32_t;

    /* Emulate C99/C++ bool type    */

typedef enum
{
	false   = 0,
	true    = !(false)
} bool_t;

typedef char BOOL;

#define ROM @near            // 8051 type of ROM memory
#define XDATA    @near      // 8051 type of external memory

//------------------------------------------------------------------------------
// Configuration defines used by hal_config.h
//------------------------------------------------------------------------------

#define ENABLE      (0xFF)
#define DISABLE     (0x00)

#endif  // __SI_DATATYPES_H__
