#ifndef __LIBDCM_H__
#define __LIBDCM_H__

// 2014: Raphael, Kim //
// ------------------
// 1. Added "new **" for  DCM and element.
// 2. Modifed for supporting unicode models.

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef BYTE
    #define BYTE    unsigned char
#endif // BYTE

#ifndef WORD
    #define WORD    unsigned short
#endif // WORD

#ifndef DWORD
    #define DWORD   unsigned long
#endif // DWORD

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

typedef struct _DCMTagElement
{
    DWORD   id;
    char    VRtype[2];
    DWORD   size;
    bool    alloced;
    char    staticbuffer[ 64 ];
    void*   dynamicbuffer;
}DCMTagElement;

typedef struct _ImageInformation
{
    int         width;
    int         height;
    int         bpp;        /// bits per a pixel.
    float       spacing_w;
    float       spacing_h;
    void*       pixels;
}ImageInformation;

#ifdef LIB_EXPORT_ENABLE
    #define LIB_EXPORT __attribute__ ((visibility ("default")))
#else
    #define LIB_EXPORT
#endif /// of LIB_EXPORT_ENABLE

LIB_EXPORT bool         NewDCMW( const wchar_t* pFilePath );
LIB_EXPORT bool         NewDCMA( const char* pFilePath );
LIB_EXPORT bool         OpenDCMW( const wchar_t* pFilePath );
LIB_EXPORT bool         OpenDCMA( const char* pFilePath );
LIB_EXPORT bool         CloseDCM(void);
LIB_EXPORT bool         IsDCMOpened(void);
LIB_EXPORT int          GetElementCount();
LIB_EXPORT int          GetElement(int index, DCMTagElement** pElement);
LIB_EXPORT int          FindElementIndex(DWORD tagID);
LIB_EXPORT DCMTagElement* FindElement(DWORD tagID);
LIB_EXPORT bool         AddElement(DCMTagElement* pElement);
LIB_EXPORT bool         AddElementEx(DWORD tagID, char *data, int dataSize);
LIB_EXPORT bool         SaveDCM( const wchar_t* newName);
LIB_EXPORT wchar_t*     GetLastErrMsg();

// -- dictionaries
LIB_EXPORT WORD         GetVR( DWORD tagID );
LIB_EXPORT wchar_t*     GetDicomMeaning( DWORD tagID );

// -- tag create tool
LIB_EXPORT bool         NewElement( DWORD tagID, DCMTagElement** pElement );

// -- tag information tool.
LIB_EXPORT int          ReadInt( DCMTagElement* pElem );
LIB_EXPORT char*        ReadAnsiString( DCMTagElement* pElem );
LIB_EXPORT wchar_t*     ReadWideString( DCMTagElement* pElem );
LIB_EXPORT bool         ReadPixelData( ImageInformation* pII );

LIB_EXPORT int          WriteInt( DCMTagElement* pElem, const int iv );
LIB_EXPORT int          WriteAnsiString( DCMTagElement* pElem, const char* as );
LIB_EXPORT int          WriteWideString( DCMTagElement* pElem, const wchar_t* ws );

// -- related in images.
LIB_EXPORT bool         AddImage( ImageInformation* pII );

#ifndef LIB_EXPORT_ENABLE
#ifdef UNICODE
    #define     NewDCM( _s_ )   NewDCMW( _s_ )
    #define     OpenDCM( _s_ )  OpenDCMW( _s_ )
#else
    #define     NewDCM( _s_ )   NewDCMA( _s_ )
    #define     OpenDCM( _s_ )  OpenDCMA( _s_ )
#endif /// of UNICODE
#endif /// of LIB_EXPORT_ENABLE

#ifdef __cplusplus
}
#endif

#endif // __LIBDCM_H__
