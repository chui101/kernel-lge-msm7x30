/*
** =========================================================================
** Copyright (c) 2003-2009  Immersion Corporation.  All rights reserved.
**                          Immersion Corporation Confidential and Proprietary
**
** File:
**     ImmVibe.h
**
** Description: 
**     Main header file of the Immersion TSP API. It defines 
**     function prototypes and constants.
**
** =========================================================================
*/
#ifndef _IMMVIBE_H
#define _IMMVIBE_H

#include "ImmVibeCore.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef IMMVIBEAPI
    #if defined(WIN32)
        #define IMMVIBEAPI __declspec(dllimport)
    #else
        #define IMMVIBEAPI extern
    #endif
#endif
	
/** Pointer to the API's internal IVT file that contains builtIn effects */
IMMVIBEAPI VibeUInt8 g_pVibeIVTBuiltInEffects[];

/** Effect indexes in the API's internal IVT file */
#define VIBE_BUILTINEFFECT_SHORT_ON_SHORT_OFF	0
#define VIBE_BUILTINEFFECT_SHORT_ON_MEDIUM_OFF	1
#define VIBE_BUILTINEFFECT_SHORT_ON_LONG_OFF	2
#define VIBE_BUILTINEFFECT_MEDIUM_ON_SHORT_OFF	3
#define VIBE_BUILTINEFFECT_MEDIUM_ON_MEDIUM_OFF 4
#define VIBE_BUILTINEFFECT_MEDIUM_ON_LONG_OFF	5
#define VIBE_BUILTINEFFECT_LONG_ON_SHORT_OFF	6
#define VIBE_BUILTINEFFECT_LONG_ON_MEDIUM_OFF	7
#define VIBE_BUILTINEFFECT_LONG_ON_LONG_OFF		8
#define VIBE_BUILTINEFFECT_SHORT				9
#define VIBE_BUILTINEFFECT_MEDIUM				10
#define VIBE_BUILTINEFFECT_LONG					11


/*****************************************************************************
 *
 *  API Constants
 *
 *****************************************************************************/
#define   VIBE_CURRENT_VERSION_NUMBER  0x3040537


/*****************************************************************************
 *
 *  Function Prototypes
 *
 *****************************************************************************/
IMMVIBEAPI VibeStatus ImmVibeInitialize
(
	IN VibeInt32 nVersion
);
IMMVIBEAPI VibeStatus ImmVibeTerminate(void);
IMMVIBEAPI VibeStatus ImmVibeGetDeviceCount(void);
IMMVIBEAPI VibeStatus ImmVibeGetDeviceState
(
	IN VibeInt32 nDeviceIndex,
    OUT VibeInt32 *pnState
);
IMMVIBEAPI VibeStatus ImmVibeGetDeviceCapabilityBool
(
    IN VibeInt32 nDeviceIndex,
    IN VibeInt32 nDevCapType,
    OUT VibeBool *pbDevCapVal
);
IMMVIBEAPI VibeStatus ImmVibeGetDeviceCapabilityInt32
(
    IN VibeInt32 nDeviceIndex,
    IN VibeInt32 nDevCapType,
    OUT VibeInt32 *pnDevCapVal
);
IMMVIBEAPI VibeStatus ImmVibeGetDeviceCapabilityString
(
    IN VibeInt32 nDeviceIndex,
    IN VibeInt32 nDevCapType,
    IN VibeInt32 nSize,
    OUT char *szDevCapVal
);
IMMVIBEAPI VibeStatus ImmVibeOpenDevice
(
    IN VibeInt32 nDeviceIndex,
    OUT VibeInt32 *phDeviceHandle
);
IMMVIBEAPI VibeStatus ImmVibeOpenCompositeDevice
(
    IN const VibeInt32* pnDeviceIndex,
    IN VibeUInt32 nNumDevice,
    OUT VibeInt32 *phDeviceHandle
);
IMMVIBEAPI VibeStatus ImmVibeCloseDevice
(
	IN VibeInt32 hDeviceHandle
);
IMMVIBEAPI VibeStatus ImmVibeGetDevicePropertyBool
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 nDevPropType,
    OUT VibeBool *pbDevPropVal
);
IMMVIBEAPI VibeStatus ImmVibeSetDevicePropertyBool
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 nDevPropType,
    IN VibeBool bDevPropVal
);
IMMVIBEAPI VibeStatus ImmVibeGetDevicePropertyInt32
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 nDevPropType,
    OUT VibeInt32 *pnDevPropVal
);
IMMVIBEAPI VibeStatus ImmVibeSetDevicePropertyInt32
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 nDevPropType,
    IN VibeInt32 nDevPropVal
);
IMMVIBEAPI VibeStatus ImmVibeGetDevicePropertyString
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 nDevPropType,
    IN VibeInt32 nSize,
    OUT char *szDevPropVal
);
IMMVIBEAPI VibeStatus ImmVibeSetDevicePropertyString
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 nDevPropType,
    IN const char *szDevPropVal
);
IMMVIBEAPI VibeStatus ImmVibeGetIVTEffectCount
(
	IN const VibeUInt8 *pIVT
);
IMMVIBEAPI VibeStatus ImmVibeGetIVTEffectName
(
    IN const VibeUInt8 *pIVT,
    IN VibeInt32 nEffectIndex,
	IN VibeInt32 nSize,
    OUT char *szEffectName
);
IMMVIBEAPI VibeStatus ImmVibeGetIVTEffectNameU
(
    IN const VibeUInt8 *pIVT,
    IN VibeInt32 nEffectIndex,
	IN VibeInt32 nSize,
    OUT VibeWChar *szEffectName
);
IMMVIBEAPI VibeStatus ImmVibeGetIVTEffectIndexFromName
(
    IN const VibeUInt8 *pIVT,
    IN const char *szEffectName,
    OUT VibeInt32 *pnEffectIndex
);
IMMVIBEAPI VibeStatus ImmVibeGetIVTEffectIndexFromNameU
(
    IN const VibeUInt8 *pIVT,
    IN const VibeWChar *szEffectName,
    OUT VibeInt32 *pnEffectIndex
);
IMMVIBEAPI VibeStatus ImmVibePlayIVTEffect
(
    IN VibeInt32 hDeviceHandle,
    IN const VibeUInt8 *pIVT,
    IN VibeInt32 nEffectIndex,
    OUT VibeInt32 *phEffectHandle
);
IMMVIBEAPI VibeStatus ImmVibePlayIVTEffectRepeat
(
    IN VibeInt32 hDeviceHandle,
    IN const VibeUInt8 *pIVT,
    IN VibeInt32 nEffectIndex,
    IN VibeUInt8 nRepeat,
    OUT VibeInt32 *phEffectHandle
);
IMMVIBEAPI VibeStatus ImmVibeStopPlayingEffect
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 hEffectHandle
);
IMMVIBEAPI VibeStatus ImmVibeGetIVTEffectType
(
    IN const VibeUInt8 *pIVT, 
    IN VibeInt32 nEffectIndex,
    OUT VibeInt32 *pnEffectType
);
IMMVIBEAPI VibeStatus ImmVibeGetIVTMagSweepEffectDefinition
(
    IN const VibeUInt8 *pIVT,
    IN VibeInt32 nEffectIndex, 
    OUT VibeInt32 *pnDuration,
    OUT VibeInt32 *pnMagnitude,
	OUT VibeInt32 *pnStyle,
    OUT VibeInt32 *pnAttackTime,
    OUT VibeInt32 *pnAttackLevel,
    OUT VibeInt32 *pnFadeTime,
    OUT VibeInt32 *pnFadeLevel
);
IMMVIBEAPI VibeStatus ImmVibeGetIVTPeriodicEffectDefinition
(
    IN const VibeUInt8 *pIVT,
    IN VibeInt32 nEffectIndex,
    OUT VibeInt32 *pnDuration,
    OUT VibeInt32 *pnMagnitude,
    OUT VibeInt32 *pnPeriod,
    OUT VibeInt32 *pnStyleAndWaveType,
    OUT VibeInt32 *pnAttackTime,
    OUT VibeInt32 *pnAttackLevel,
    OUT VibeInt32 *pnFadeTime,
    OUT VibeInt32 *pnFadeLevel
);
IMMVIBEAPI VibeStatus ImmVibeGetIVTEffectDuration
(
    IN const VibeUInt8 *pIVT, 
    IN VibeInt32 nEffectIndex,
    OUT VibeInt32 *pnEffectDuration
);
IMMVIBEAPI VibeStatus ImmVibePlayMagSweepEffect
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 nDuration,
    IN VibeInt32 nMagnitude,
    IN VibeInt32 nStyle,
    IN VibeInt32 nAttackTime,
    IN VibeInt32 nAttackLevel,
    IN VibeInt32 nFadeTime,
    IN VibeInt32 nFadeLevel,
    OUT VibeInt32 *phEffectHandle
);
IMMVIBEAPI VibeStatus ImmVibePlayPeriodicEffect
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 nDuration,
    IN VibeInt32 nMagnitude,
    IN VibeInt32 nPeriod,
    IN VibeInt32 nStyleAndWaveType,
    IN VibeInt32 nAttackTime,
    IN VibeInt32 nAttackLevel,
    IN VibeInt32 nFadeTime,
    IN VibeInt32 nFadeLevel,
    OUT VibeInt32 *phEffectHandle
);
IMMVIBEAPI VibeStatus ImmVibePlayWaveformEffect
(
    IN VibeInt32 hDeviceHandle,
    IN const VibeUInt8 *pData,
    IN VibeInt32 nDataSize,
    IN VibeInt32 nSamplingRate,
    IN VibeInt32 nBitDepth,
    IN VibeInt32 nMagnitude,
    OUT VibeInt32 *phEffectHandle
);
IMMVIBEAPI VibeStatus ImmVibeModifyPlayingMagSweepEffect
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 hEffectHandle,
    IN VibeInt32 nDuration,
    IN VibeInt32 nMagnitude,
    IN VibeInt32 nStyle,
    IN VibeInt32 nAttackTime,
    IN VibeInt32 nAttackLevel,
    IN VibeInt32 nFadeTime,
    IN VibeInt32 nFadeLevel
);
IMMVIBEAPI VibeStatus ImmVibeModifyPlayingPeriodicEffect
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 hEffectHandle,
    IN VibeInt32 nDuration,
    IN VibeInt32 nMagnitude,
    IN VibeInt32 nPeriod,
    IN VibeInt32 nStyleAndWaveType,
    IN VibeInt32 nAttackTime,
    IN VibeInt32 nAttackLevel,
    IN VibeInt32 nFadeTime,
    IN VibeInt32 nFadeLevel
);
IMMVIBEAPI VibeStatus ImmVibeStopAllPlayingEffects
(
	IN VibeInt32 hDeviceHandle
);
IMMVIBEAPI VibeStatus ImmVibeSaveIVTFile
(
    IN const VibeUInt8 *pIVT,
    IN const char *szPathname
);
IMMVIBEAPI VibeStatus ImmVibeDeleteIVTFile
(
    IN const char *szPathname
);

IMMVIBEAPI VibeStatus ImmVibeCreateStreamingEffect
(
    IN VibeInt32 hDeviceHandle,
    OUT VibeInt32 *phEffectHandle
);
IMMVIBEAPI VibeStatus ImmVibeDestroyStreamingEffect
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 hEffectHandle
);
IMMVIBEAPI VibeStatus ImmVibePlayStreamingSample
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 hEffectHandle,
    IN const VibeUInt8 *pStreamingSample,
    IN VibeInt32 nSize
);
IMMVIBEAPI VibeStatus ImmVibePlayStreamingSampleWithOffset
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 hEffectHandle,
    IN const VibeUInt8 *pStreamingSample,
    IN VibeInt32 nSize,
    IN VibeInt32 nOffsetTime
);
IMMVIBEAPI VibeStatus ImmVibePausePlayingEffect
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 hEffectHandle
);
IMMVIBEAPI VibeStatus ImmVibeResumePausedEffect
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 hEffectHandle
);
IMMVIBEAPI VibeStatus ImmVibeGetEffectState
(
    IN VibeInt32 hDeviceHandle,
    IN VibeInt32 hEffectHandle,
    OUT VibeInt32 *pnEffectState
);
IMMVIBEAPI VibeStatus ImmVibeGetIVTSize(
    IN const VibeUInt8 *pIVT,
    IN VibeUInt32 nSize
);
IMMVIBEAPI VibeStatus ImmVibeInitializeIVTBuffer(
    IN VibeUInt8 *pIVT,
    IN VibeUInt32 nSize
);
IMMVIBEAPI VibeStatus ImmVibeInsertIVTElement(
    IN VibeUInt8 *pIVT,
    IN VibeUInt32 nSize,
    IN VibeUInt32 nTimelineIndex,
    IN const VibeIVTElement *pElement
);
IMMVIBEAPI VibeStatus ImmVibeInsertIVTElement2(
    IN VibeUInt8 *pIVT,
    IN VibeUInt32 nSize,
    IN VibeUInt32 nTimelineIndex,
    IN const VibeIVTElement2 *pElement
);
IMMVIBEAPI VibeStatus ImmVibeInsertIVTElement3(
    IN VibeUInt8 *pIVT,
    IN VibeUInt32 nSize,
    IN VibeUInt32 nTimelineIndex,
    IN const VibeIVTElement3 *pElement
);
IMMVIBEAPI VibeStatus ImmVibeReadIVTElement(
    IN const VibeUInt8 *pIVT,
    IN VibeUInt32 nSize,
    IN VibeUInt32 nTimelineIndex,
    IN VibeUInt32 nElementIndex,
    IN VibeIVTElement *pElement
);
IMMVIBEAPI VibeStatus ImmVibeReadIVTElement2(
    IN const VibeUInt8 *pIVT,
    IN VibeUInt32 nSize,
    IN VibeUInt32 nTimelineIndex,
    IN VibeUInt32 nElementIndex,
    IN VibeIVTElement2 *pElement
);
IMMVIBEAPI VibeStatus ImmVibeReadIVTElement3(
    IN const VibeUInt8 *pIVT,
    IN VibeUInt32 nSize,
    IN VibeUInt32 nTimelineIndex,
    IN VibeUInt32 nElementIndex,
    IN VibeIVTElement3 *pElement
);
IMMVIBEAPI VibeStatus ImmVibeRemoveIVTElement(
    IN VibeUInt8 *pIVT,
    IN VibeUInt32 nSize,
    IN VibeUInt32 nTimelineIndex,
    IN VibeUInt32 nElementIndex
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _IMMVIBE_H */
