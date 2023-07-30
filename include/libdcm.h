#ifndef __LIBDCM_H__
#define __LIBDCM_H__

// 2023: Raphael, Kim //
// 1. Updated some more features by -1 sized pixel data.
// 2. Following standard integer types, instead MSVC type.
//
// 2014: Raphael, Kim //
// ------------------
// 1. Added "new **" for  DCM and element.
// 2. Modifed for supporting unicode models.

#ifdef __cplusplus
extern "C"
{
namespace tinydicom
{
#endif

#ifndef _WIN32
	#define TEXT
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
    #define FALSE   false
#endif // FALSE

#ifndef TRUE
    #define TRUE    true
#endif // TRUE

// version is 0.5.5.153
#define LIBTINYDCM_VERSION      0x00050599

typedef struct _DCMTagElement
{
    uint32_t   id;
    uint8_t    VRtype[2];
    uint32_t   size;
    bool       alloced;
    uint8_t    staticbuffer[ 64 ];
    uint8_t*   dynamicbuffer;
}DCMTagElement;

typedef struct _ImageInformation
{
    // default information
    uint32_t    width;
    uint32_t    height;
    uint32_t    planes;
    uint32_t    bpp;        /// bits per a pixel.

    // pixel spacing --
    float       spacing_w;
    float       spacing_h;
    float       spacing_d;

    // pixel data
    uint32_t    pixels_size;
    uint8_t*    pixels;
}ImageInformation;

#ifdef LIB_EXPORT_ENABLE
    #define LIB_EXPORT __attribute__ ((visibility ("default")))
#else
    #define LIB_EXPORT
#endif /// of LIB_EXPORT_ENABLE

LIB_EXPORT void             GetTinyDicomLibVersion( int* versions /* must be int[4] */ );
LIB_EXPORT bool             NewDCMW( const wchar_t* pFilePath );
LIB_EXPORT bool             NewDCMA( const char* pFilePath );
LIB_EXPORT bool             OpenDCMW( const wchar_t* pFilePath );
LIB_EXPORT bool             OpenDCMA( const char* pFilePath );
LIB_EXPORT bool             CloseDCM(void);
LIB_EXPORT bool             IsDCMOpened(void);
LIB_EXPORT int32_t          GetElementCount();
LIB_EXPORT int32_t          GetElement(uint32_t index, DCMTagElement** pElement);
LIB_EXPORT int32_t          FindElementIndex(uint32_t tagID);
LIB_EXPORT DCMTagElement*   FindElement(uint32_t tagID);
LIB_EXPORT bool             AddElement(DCMTagElement* pElement);
LIB_EXPORT bool             AddElementEx(uint32_t tagID, const uint8_t* data, size_t dataSize);
LIB_EXPORT void             DiscardElement( DCMTagElement** pElement );
LIB_EXPORT bool             SaveDCMW( const wchar_t* newName);
LIB_EXPORT bool             SaveDCMA( const char* newName);
LIB_EXPORT wchar_t*         GetLastErrMsg();

// -- dictionaries
LIB_EXPORT uint16_t         GetVR( uint32_t tagID );
LIB_EXPORT const wchar_t*   GetDicomMeaningW( uint32_t tagID );
LIB_EXPORT const char*      GetDicomMeaningA( uint32_t tagID );
LIB_EXPORT const wchar_t*   GetMediaMimeW( const wchar_t* UID );
LIB_EXPORT const char*      GetMediaMimeA( const char* UID );

// -- tag create tool
LIB_EXPORT bool             NewElement( uint32_t tagID, DCMTagElement** pElement );

// -- tag information tool.
LIB_EXPORT int32_t          ReadInt( DCMTagElement* pElem );
LIB_EXPORT const char*      ReadAnsiString( DCMTagElement* pElem );
LIB_EXPORT const wchar_t*   ReadWideString( DCMTagElement* pElem );
LIB_EXPORT bool             ReadPixelData( ImageInformation* pII );

LIB_EXPORT int32_t          WriteInt( DCMTagElement* pElem, const int32_t intv );
LIB_EXPORT int32_t          WriteAnsiString( DCMTagElement* pElem, const char* as );
LIB_EXPORT int32_t          WriteWideString( DCMTagElement* pElem, const wchar_t* ws );

// -- related in images.
LIB_EXPORT bool             AddImage( ImageInformation* pII );

#ifndef LIB_EXPORT_ENABLE
#ifdef UNICODE
    #define     NewDCM( _s_ )   NewDCMW( _s_ )
    #define     OpenDCM( _s_ )  OpenDCMW( _s_ )
    #define     SaveDCM( _s_ )  SaveDCMW( _s_ )
#else
    #define     NewDCM( _s_ )   NewDCMA( _s_ )
    #define     OpenDCM( _s_ )  OpenDCMA( _s_ )
    #define     SaveDCM( _s_ )  SaveDCMA( _s_ )
#endif /// of UNICODE
#endif /// of LIB_EXPORT_ENABLE

#ifdef __cplusplus
} /// of namespace tinydicom
} /// of extern "C".
#endif

#endif // __LIBDCM_H__
