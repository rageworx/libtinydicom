#pragma once
#ifndef __DICOMTAGCONFIG_H__
#define __DICOMTAGCONFIG_H__

////////////////////////////////////////////////////////////////////////////////

#define SYSTEM_WIN32_X86        0
#define SYSTEM_WIN32_X86_64     1

#define SYSTEM_WIN32_ARM        10
#define SYSTEM_WIN32_ARM_C8     11


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// if U change build system, check and modify here:

#define CURRENT_SYSTEM_MODEL    SYSTEM_WIN32_X86
#define USING_DCM_ISE_DLL       1

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if ((CURRENT_SYSTEM_MODEL == SYSTEM_WIN32_X86) || (CURRENT_SYSTEM_MODEL == SYSTEM_WIN32_X86_64))
    #define DATA_ARRANGE_LITTLE_ENDIAN      1
#else
    #define DATA_ARRANGE_LITTLE_ENDIAN      0
#endif

#if (USING_DCM_ISE_DLL == 1)
    #define DICOMTAG_OPT_USING_DCMISE
#else
    #define DICOMTAG_OPT_USING_INTERNALVOLUMEENGINE
#endif

#endif //__DICOMTAGCONFIG_H__
