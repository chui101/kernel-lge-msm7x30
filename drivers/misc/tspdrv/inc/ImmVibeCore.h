/*
** =========================================================================
** Copyright (c) 2003-2010  Immersion Corporation.  All rights reserved.
**                          Immersion Corporation Confidential and Proprietary
**
** File:
**     ImmVibeCore.h
**
** Description: 
**     Constant and type definitions and macros for the Immersion
**     TSP API.
**
** =========================================================================
*/
#ifndef _IMMVIBECORE_H
#define _IMMVIBECORE_H
 
#include "ImmVibeOS.h"

#ifdef IN
    #undef IN
#endif
#define IN

#ifdef OUT
    #undef OUT
#endif
#define OUT

/****************************************************************************
 *
 *  Constants and Definitions
 *
 ****************************************************************************/
#define VIBE_MAX_MAGNITUDE                          10000 /*!< Maximum Force Magnitude */
#define VIBE_MIN_MAGNITUDE                          0     /*!< Minimum Force Magnitude */   
#define VIBE_MAX_EFFECT_NAME_LENGTH                 128   /*!< Maximum effect name length */
#define VIBE_INVALID_INDEX                          -1    /*!< Invalid Index */

/* DevicePropertyType */
#define VIBE_DEVPROPTYPE_LICENSE_KEY                0 /*!< Property type constant to set Application licence key */
#define VIBE_DEVPROPTYPE_PRIORITY                   1 /*!< Property type constant to set device priority */
#define VIBE_DEVPROPTYPE_DISABLE_EFFECTS            2 /*!< Property type constant to enable/disable effects on a device */
#define VIBE_DEVPROPTYPE_STRENGTH                   3 /*!< Property type constant to set the strength (volume) on a device */
#define VIBE_DEVPROPTYPE_MASTERSTRENGTH             4 /*!< Property type constant to set the strength (volume) on ALL devices */

/* Device type returned by ImmVibeGetDeviceCapabilityInt32 in 'xxxx' field of
   'yyyy' struct for 'deviceCapabilityType' equal to
   VIBE_DEVCAPTYPE_DEVICE_CATEGORY */
#define VIBE_DEVICECATEGORY_IFC                     0 /*!< Device category constant for IFC Devices */
#define VIBE_DEVICECATEGORY_IMPULSE                 1 /*!< Device category constant for Impulse Devices */
#define VIBE_DEVICECATEGORY_VIRTUAL                 2 /*!< Device category constant for Virtual Devices */
#define VIBE_DEVICECATEGORY_EMBEDDED                3 /*!< Device category constant for Embedded Devices */
#define VIBE_DEVICECATEGORY_TETHERED                4 /*!< Device category constant for Tethered Devices */
#define VIBE_DEVICECATEGORY_IMMERSION_USB           5 /*!< Device category constant for Immersion USB Devices */
#define VIBE_DEVICECATEGORY_COMPOSITE               6 /*!< Device category constant for Composite Devices */

/* Effect type returned by ImmVibeGetIVTEffectType */
#define VIBE_EFFECT_TYPE_PERIODIC                   0 /*!< Periodic Effect type constant */
#define VIBE_EFFECT_TYPE_MAGSWEEP                   1 /*!< Magsweep Effect type constant */
#define VIBE_EFFECT_TYPE_TIMELINE                   2 /*!< Timeline Effect type constant */
#define VIBE_EFFECT_TYPE_STREAMING                  3 /*!< Streaming Effect type constant */
#define VIBE_EFFECT_TYPE_WAVEFORM                   4 /*!< Waveform Effect type constant */

/* Device capability type passed as input 'deviceCapabilityType' argument to ImmVibeGetDeviceCapability... */
#define VIBE_DEVCAPTYPE_DEVICE_CATEGORY             0  /*!< Use ImmVibeGetDeviceCapabilityInt32 >*/
#define VIBE_DEVCAPTYPE_MAX_NESTED_REPEATS          1 /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_NUM_ACTUATORS               2  /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_ACTUATOR_TYPE               3 /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_NUM_EFFECT_SLOTS            4  /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_SUPPORTED_STYLES            5  /*!< Use ImmVibeGetDeviceCapabilityInt32 */
/* VIBE_DEVCAPTYPE_SUPPORTED_CONTROL_MODES is deprecated and will not be an available constant 
** in future versions of this software.  Please use VIBE_DEVCAPTYPE_SUPPORTED_STYLES instead. */
#define VIBE_DEVCAPTYPE_SUPPORTED_CONTROL_MODES     VIBE_DEVCAPTYPE_SUPPORTED_STYLES
#define VIBE_DEVCAPTYPE_MIN_PERIOD                  6  /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_MAX_PERIOD                  7  /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_MAX_EFFECT_DURATION         8  /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_SUPPORTED_EFFECTS           9  /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_DEVICE_NAME                 10 /*!< Use ImmVibeGetDeviceCapabilityString */
#define VIBE_DEVCAPTYPE_MAX_ENVELOPE_TIME           11 /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_APIVERSIONNUMBER            12 /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_MAX_IVT_SIZE_TETHERED       13 /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_MAX_IVT_SIZE                14 /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_EDITION_LEVEL               15 /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_SUPPORTED_WAVE_TYPES        16 /*!< Use ImmVibeGetDeviceCapabilityInt32 */
#define VIBE_DEVCAPTYPE_HANDSET_INDEX               17 /*!< Use ImmVibeGetDeviceCapabilityInt32 */

/* Handles */
#define VIBE_INVALID_EFFECT_HANDLE_VALUE            -1 /*!< Invalid Effect Handle */
#define VIBE_INVALID_DEVICE_HANDLE_VALUE            -1 /*!< Invalid Device Handle */

/* Periodic, MagSweep effect Styles are only 4 bits and may be combined with other flags */
#define VIBE_STYLE_MASK                             0x0F    /*!< Style mask */

/* Periodic, MagSweep effect Styles */
#define VIBE_STYLE_SMOOTH                           0   /*!< "Smooth" style */
#define VIBE_STYLE_STRONG                           1   /*!< "Strong" style */
#define VIBE_STYLE_SHARP                            2   /*!< "Sharp" style  */

#define VIBE_DEFAULT_STYLE                          VIBE_STYLE_STRONG

/* VIBE_CONTROLMODE_ constants are deprecated and will not be available 
** in future versions of this software.  Please use the VIBE_STYLE_ constants instead. */
#define VIBE_CONTROLMODE_SMOOTH                     VIBE_STYLE_SMOOTH
#define VIBE_CONTROLMODE_STRONG                     VIBE_STYLE_STRONG
#define VIBE_CONTROLMODE_SHARP                      VIBE_STYLE_SHARP

#define VIBE_DEFAULT_CONTROLMODE                    VIBE_DEFAULT_STYLE

/* Effect period resolution */
#define VIBE_PERIOD_RESOLUTION_MICROSECOND          0x80000000

/* Periodic effect Wave Types are only 4 bits and may be combined with other flags */
#define VIBE_WAVETYPE_SHIFT                         4       /*!< Wave type shift */
#define VIBE_WAVETYPE_MASK                          0xF0    /*!< Wave type mask */

/* Periodic effect Wave Types */
#define VIBE_WAVETYPE_SQUARE                        (1 << VIBE_WAVETYPE_SHIFT)  /*!< "Square" wave type */
#define VIBE_WAVETYPE_TRIANGLE                      (2 << VIBE_WAVETYPE_SHIFT)  /*!< "Triangle" wave type */
#define VIBE_WAVETYPE_SINE                          (3 << VIBE_WAVETYPE_SHIFT)  /*!< "Sine" wave type */
#define VIBE_WAVETYPE_SAWTOOTHUP                    (4 << VIBE_WAVETYPE_SHIFT)  /*!< "Sawtooth Up" wave type */
#define VIBE_WAVETYPE_SAWTOOTHDOWN                  (5 << VIBE_WAVETYPE_SHIFT)  /*!< "Sawtooth Down" wave type */

#define VIBE_DEFAULT_WAVETYPE                       VIBE_WAVETYPE_SQUARE

/* String length constants */
#define VIBE_MAX_DEVICE_NAME_LENGTH                 64 /*!<Maximum device name length */
#define VIBE_MAX_CAPABILITY_STRING_LENGTH           64 /*!<Maximum string length use by ImmVibeGetDeviceCapabilityString/ ImmVibeSetDeviceCapabilityString*/
#define VIBE_MAX_PROPERTY_STRING_LENGTH             64 /*!<Maximum string length use by ImmVibeGetDevicePropertyString/ ImmVibeSetDevicePropertyString */

/* Effect type support bit masks */
#define VIBE_PERIODIC_EFFECT_SUPPORT                (1 << VIBE_EFFECT_TYPE_PERIODIC)  /*!< Bitmask for Periodic effect support */
#define VIBE_MAGSWEEP_EFFECT_SUPPORT                (1 << VIBE_EFFECT_TYPE_MAGSWEEP)  /*!< Bitmask for Magsweep effect support */
#define VIBE_TIMELINE_EFFECT_SUPPORT                (1 << VIBE_EFFECT_TYPE_TIMELINE)  /*!< Bitmask for Timeline effect support */
#define VIBE_STREAMING_EFFECT_SUPPORT               (1 << VIBE_EFFECT_TYPE_STREAMING) /*!< Bitmask for Streaming effect support */
#define VIBE_WAVEFORM_EFFECT_SUPPORT                (1 << VIBE_EFFECT_TYPE_WAVEFORM)  /*!< Bitmask for Waveform effect support */

/* Effect Style support bit masks */
#define VIBE_STYLE_SUPPORT_MASK                     0x0000FFFF              /*!< Effect style support mask */
#define VIBE_STYLE_SMOOTH_SUPPORT                   (1 << VIBE_STYLE_SMOOTH) /*!< Bitmask for "Smooth" style support */
#define VIBE_STYLE_STRONG_SUPPORT                   (1 << VIBE_STYLE_STRONG) /*!< Bitmask for "Strong" style support */
#define VIBE_STYLE_SHARP_SUPPORT                    (1 << VIBE_STYLE_SHARP)  /*!< Bitmask for "Sharp" style support  */

/* Wave type support bit masks */
/* Starts at 0x10000 to allow combining the flag with the supported style (nControlMode) 32 bits flag */
#define VIBE_WAVETYPE_SUPPORT_MASK                  0xFFFF0000                                                       /*!< Wave type support mask */
#define VIBE_WAVETYPE_SQUARE_SUPPORT                (0x10000 << (VIBE_WAVETYPE_SQUARE >> VIBE_WAVETYPE_SHIFT))       /*!< Bitmask for "Square" wave type support */
#define VIBE_WAVETYPE_TRIANGLE_SUPPORT              (0x10000 << (VIBE_WAVETYPE_TRIANGLE >> VIBE_WAVETYPE_SHIFT))     /*!< Bitmask for "Triangle" wave type support */
#define VIBE_WAVETYPE_SINE_SUPPORT                  (0x10000 << (VIBE_WAVETYPE_SINE >> VIBE_WAVETYPE_SHIFT))         /*!< Bitmask for "Sine" wave type support  */
#define VIBE_WAVETYPE_SAWTOOTHUP_SUPPORT            (0x10000 << (VIBE_WAVETYPE_SAWTOOTHUP >> VIBE_WAVETYPE_SHIFT))   /*!< Bitmask for "Saw tooth up" wave type support  */
#define VIBE_WAVETYPE_SAWTOOTHDOWN_SUPPORT          (0x10000 << (VIBE_WAVETYPE_SAWTOOTHDOWN >> VIBE_WAVETYPE_SHIFT)) /*!< Bitmask for "Saw tooth down" wave type support  */

/* VIBE_CONTROLMODE_*_SUPPORT constants are deprecated and will not be available 
** in future versions of this software.  Please use the VIBE_STYLE_*_SUPPORT constants instead. */
#define VIBE_CONTROLMODE_SMOOTH_SUPPORT             VIBE_STYLE_SMOOTH_SUPPORT
#define VIBE_CONTROLMODE_STRONG_SUPPORT             VIBE_STYLE_STRONG_SUPPORT
#define VIBE_CONTROLMODE_SHARP_SUPPORT              VIBE_STYLE_SHARP_SUPPORT

/* Device State constants */
#define VIBE_DEVICESTATE_ATTACHED                   (1 << 0) /*!< Device is attached to the system */
#define VIBE_DEVICESTATE_BUSY                       (1 << 1) /*!< Device is busy (playing effects) */

/* Time in milliseconds */
#define VIBE_TIME_INFINITE                          VIBE_INT32_MAX /*!< Infinite time */

/* Effect priority */
#define VIBE_MIN_DEVICE_PRIORITY                    0x0 /*!< Minimum Effect priority */
#define VIBE_MAX_DEV_DEVICE_PRIORITY                0x7 /*!< Maximum Effect priority for developers */
#define VIBE_MAX_OEM_DEVICE_PRIORITY                0xF /*!< Maximum Effect priority for OEMs */
#define VIBE_MAX_DEVICE_PRIORITY                    VIBE_MAX_OEM_DEVICE_PRIORITY    /*!< FOR BACKWARD COMPATIBILITY ONLY;
                                                                                         new applications should use VIBE_MAX_DEV_DEVICE_PRIORITY
                                                                                         or VIBE_MAX_OEM_DEVICE_PRIORITY */


/* Device Actuator Type constants */
#define VIBE_DEVACTUATORTYPE_ERM                    0
#define VIBE_DEVACTUATORTYPE_BLDC                   1
#define VIBE_DEVACTUATORTYPE_LRA                    2
#define VIBE_DEVACTUATORTYPE_PIEZO                  4
#define VIBE_DEVACTUATORTYPE_PIEZO_WAVE             4

/* Device Default priority value */
#define VIBE_DEVPRIORITY_DEFAULT                    0

/* Repeat count */
#define VIBE_REPEAT_COUNT_INFINITE                  255 /*!< Infinite repeat count */

/* Streaming Sample */
#define VIBE_MAX_STREAMING_SAMPLE_SIZE              255 /*!< Maximum size for streaming sample */

/* Effect state returned by ImmVibeGetEffectState */
#define VIBE_EFFECT_STATE_NOT_PLAYING               0 /*!< Not Playing and not paused */
#define VIBE_EFFECT_STATE_PLAYING                   1 /*!< Playing */
#define VIBE_EFFECT_STATE_PAUSED                    2 /*!< Paused */

/* Edition levels */
#define VIBE_EDITION_3000                           3000
#define VIBE_EDITION_4000                           4000
#define VIBE_EDITION_5000                           5000

/* Element type for IVTElement structure that is used by ImmVibeInsertIVTElement, ImmVibeReadIVTElement and ImmVibeRemoveIVTElement */
#define VIBE_ELEMTYPE_PERIODIC                      0
#define VIBE_ELEMTYPE_MAGSWEEP                      1
#define VIBE_ELEMTYPE_REPEAT                        2
/* New in API v3.4 */
#define VIBE_ELEMTYPE_WAVEFORM                      3

/* Composite device */
#define VIBE_MAX_LOGICAL_DEVICE_COUNT               16 /*!< Maximum number of device indices that can be passed to ImmVibeOpenCompositeDevice */

/****************************************************************************
 *
 *  General macros
 *
 ****************************************************************************/
#define VIBE_SUCCEEDED(n)                           ((n) >= 0)
#define VIBE_FAILED(n)                              ((n) < 0)
#define VIBE_IS_INVALID_DEVICE_HANDLE(n)            (((n) == 0) || ((n) == VIBE_INVALID_DEVICE_HANDLE_VALUE))
#define VIBE_IS_INVALID_EFFECT_HANDLE(n)            (((n) == 0) || ((n) == VIBE_INVALID_EFFECT_HANDLE_VALUE))
#define VIBE_IS_VALID_DEVICE_HANDLE(n)              (((n) != 0) && ((n) != VIBE_INVALID_DEVICE_HANDLE_VALUE))
#define VIBE_IS_VALID_EFFECT_HANDLE(n)              (((n) != 0) && ((n) != VIBE_INVALID_EFFECT_HANDLE_VALUE))

/****************************************************************************
 *
 *  Error and Return value codes.
 *
 ****************************************************************************/
#define VIBE_S_SUCCESS                               0  /*!< Success */
#define VIBE_S_FALSE                                 0  /*!< False */
#define VIBE_S_TRUE                                  1  /*!< True */
#define VIBE_W_NOT_PLAYING                           1  /*!< Effect is not playing */
#define VIBE_W_INSUFFICIENT_PRIORITY                 2  /*!< Effect doesn't have enough priority to play: higher priority effect is playing on the device */
#define VIBE_W_EFFECTS_DISABLED                      3  /*!< Effects are disabled on the device */
#define VIBE_W_NOT_PAUSED                            4  /*!< The ImmVibeResumePausedEffect function cannot resume an effect that is not paused */
#define VIBE_E_ALREADY_INITIALIZED                  -1  /*!< The API is already initialized (NOT USED) */
#define VIBE_E_NOT_INITIALIZED                      -2  /*!< The API is already is not initialized */
#define VIBE_E_INVALID_ARGUMENT                     -3  /*!< Invalid argument was used in a API function call */
#define VIBE_E_FAIL                                 -4  /*!< Generic error */
#define VIBE_E_INCOMPATIBLE_EFFECT_TYPE             -5  /*!< Incompatible Effect type has been passed into  API function call */
#define VIBE_E_INCOMPATIBLE_CAPABILITY_TYPE         -6  /*!< Incompatible Capability type was used into one of the following functions */
#define VIBE_E_INCOMPATIBLE_PROPERTY_TYPE           -7  /*!< Incompatible Property type was used into one of the following functions */
#define VIBE_E_DEVICE_NEEDS_LICENSE                 -8  /*!< Access to the instance of the device is locked until a valid license key is provided. */
#define VIBE_E_NOT_ENOUGH_MEMORY                    -9  /*!< The API function cannot allocate memory to complete the process */
#define VIBE_E_SERVICE_NOT_RUNNING                  -10 /*!< ImmVibeService is not running */
#define VIBE_E_INSUFFICIENT_PRIORITY                -11 /*!< Not enough priority to achieve the request (insufficient license key priority) */
#define VIBE_E_SERVICE_BUSY                         -12 /*!< ImmVibeService is busy and failed to complete the request */
#define VIBE_E_NOT_SUPPORTED                        -13 /*!< The API function is not supported by this version of the API */

/****************************************************************************
 *
 *  Type definition
 *
 ****************************************************************************/
typedef VibeInt32   VibeStatus;

/****************************************************************************
 *
 *  Stuctures
 *
 ****************************************************************************/
typedef struct
{
    VibeInt32       m_nDuration;
    VibeInt32       m_nMagnitude;
    VibeInt32       m_nPeriod;
    VibeInt32       m_nStyle;
    VibeInt32       m_nAttackTime;
    VibeInt32       m_nAttackLevel;
    VibeInt32       m_nFadeTime;
    VibeInt32       m_nFadeLevel;
} VibeIVTPeriodic;

typedef struct
{
    VibeInt32       m_nDuration;
    VibeInt32       m_nMagnitude;
    VibeInt32       m_nPeriod;
    VibeInt32       m_nStyle;
    VibeInt32       m_nAttackTime;
    VibeInt32       m_nAttackLevel;
    VibeInt32       m_nFadeTime;
    VibeInt32       m_nFadeLevel;
    /* New in v3.3 */
    VibeInt32       m_nActuatorIndex;
} VibeIVTPeriodic2;

typedef struct
{
    VibeInt32       m_nDuration;
    VibeInt32       m_nMagnitude;
    VibeInt32       m_nStyle;
    VibeInt32       m_nAttackTime;
    VibeInt32       m_nAttackLevel;
    VibeInt32       m_nFadeTime;
    VibeInt32       m_nFadeLevel;
} VibeIVTMagSweep;

typedef struct
{
    VibeInt32       m_nDuration;
    VibeInt32       m_nMagnitude;
    VibeInt32       m_nStyle;
    VibeInt32       m_nAttackTime;
    VibeInt32       m_nAttackLevel;
    VibeInt32       m_nFadeTime;
    VibeInt32       m_nFadeLevel;
    /* New in v3.3 */
    VibeInt32       m_nActuatorIndex;
} VibeIVTMagSweep2;

typedef struct
{
    VibeInt32       m_nCount;
    VibeInt32       m_nDuration;
} VibeIVTRepeat;

/* New in v3.4 */
typedef struct
{
    const VibeUInt8 *   m_pData;
    VibeInt32           m_nDataSize;
    VibeInt32           m_nSamplingRate;
    VibeInt32           m_nBitDepth;
    VibeInt32           m_nMagnitude;
    VibeInt32           m_nActuatorIndex;
} VibeIVTWaveform;

typedef struct
{
    VibeInt32       m_nElementType;
    VibeInt32       m_nTime;
    union
    {
        VibeIVTPeriodic     m_periodic;
        VibeIVTMagSweep     m_magsweep;
        VibeIVTRepeat       m_repeat;
    } TypeSpecific;
} VibeIVTElement;

typedef struct
{
    VibeInt32       m_nElementType;
    VibeInt32       m_nTime;
    union
    {
        VibeIVTPeriodic2    m_periodic;
        VibeIVTMagSweep2    m_magsweep;
        VibeIVTRepeat       m_repeat;
    } TypeSpecific;
} VibeIVTElement2;

typedef struct
{
    VibeInt32       m_nElementType;
    VibeInt32       m_nTime;
    union
    {
        VibeIVTPeriodic2    m_periodic;
        VibeIVTMagSweep2    m_magsweep;
        VibeIVTRepeat       m_repeat;
        /* New in v3.4 */
        VibeIVTWaveform     m_waveform;
    } TypeSpecific;
} VibeIVTElement3;

#endif /* IMMVIBECORE_H */
