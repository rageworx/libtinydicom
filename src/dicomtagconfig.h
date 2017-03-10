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

#ifndef CURRENT_SYSTEM_MODEL
    #define CURRENT_SYSTEM_MODEL    SYSTEM_WIN32_X86
#endif /// of CURRENT_SYSTEM_MODEL

#ifndef USING_DCM_ISE_DLL
    #define USING_DCM_ISE_DLL       1
#endif /// of USING_DCM_ISE_DLL

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef DATA_ARRANGE_LITTLE_ENDIAN
    #if ((CURRENT_SYSTEM_MODEL == SYSTEM_WIN32_X86) || (CURRENT_SYSTEM_MODEL == SYSTEM_WIN32_X86_64))
        #define DATA_ARRANGE_LITTLE_ENDIAN      1
    #else
        #define DATA_ARRANGE_LITTLE_ENDIAN      0
    #endif
#endif /// of DATA_ARRANGE_LITTLE_ENDIAN

#if (USING_DCM_ISE_DLL == 1)
    #define DICOMTAG_OPT_USING_DCMISE
#else
    #define DICOMTAG_OPT_USING_INTERNALVOLUMEENGINE
#endif

#ifdef _WIN32
    #ifndef BYTE
        #define BYTE            unsigned char
    #endif // BYTE
    #ifndef  WORD
        #define WORD            unsigned short
    #endif // WORD
    #ifndef DWORD
        #define DWORD           unsigned int
    #endif // DWORD
#else
    typedef unsigned char		BYTE;
    typedef unsigned short		WORD;
    typedef unsigned long		DWORD;
#endif // _WIN32

#ifndef NULL
    #ifdef __cplusplus
        #ifndef _WIN64
            #define NULL 0
        #else
            #define NULL 0LL
        #endif
    #else
        #define NULL ((void *)0)
    #endif
#endif // NULL

#ifndef FALSE
    #define FALSE 0
#endif // FALSE

#ifndef TRUE
    #define TRUE 1
#endif // TRUE

#endif //__DICOMTAGCONFIG_H__
