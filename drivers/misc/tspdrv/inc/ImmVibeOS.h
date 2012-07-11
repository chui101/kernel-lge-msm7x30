/*
** =========================================================================
** Copyright (c) 2007-2009  Immersion Corporation.  All rights reserved.
**                          Immersion Corporation Confidential and Proprietary
**
** File:
**     ImmVibeOS.h
**
** Description: 
**     OS dependant constants and type definitions for the 
**     Immersion TSP API. 
**
** =========================================================================
*/
#ifndef _IMMVIBEOS_H
#define _IMMVIBEOS_H

#include <sys/types.h>
#include <limits.h>

#define VIBE_INT8_MIN					SCHAR_MIN
#define VIBE_INT8_MAX					SCHAR_MAX
#define VIBE_UINT8_MAX					UCHAR_MAX
#define VIBE_INT16_MIN					SHRT_MIN
#define VIBE_INT16_MAX					SHRT_MAX
#define VIBE_UINT16_MAX					USHRT_MAX
#define VIBE_INT32_MIN					LONG_MIN
#define VIBE_INT32_MAX					LONG_MAX
#define VIBE_UINT32_MAX					ULONG_MAX
#define VIBE_TRUE						1
#define VIBE_FALSE						0

typedef int8_t						    VibeInt8;
typedef u_int8_t					    VibeUInt8;
typedef int16_t					        VibeInt16;
typedef u_int16_t					    VibeUInt16;
typedef int32_t						    VibeInt32;
typedef u_int32_t					    VibeUInt32;
typedef u_int8_t					    VibeBool;
typedef unsigned short                  VibeWChar;

#endif /* _IMMVIBEOS_H */
