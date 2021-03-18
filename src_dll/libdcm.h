#if !defined(UNICODE) || !defined(_UNICODE)
    #error "Please use this library with UNICODE."
#endif

#ifndef __LIBDCM_H__
#define __LIBDCM_H__

// 2014: Raphael, Kim //
// ------------------
// 1. Added "new **" for  DCM and element.
// 2. Modifed for supporting unicode models.

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

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
    int         planes;
    int         bpp;        /// bits per a pixel.
    float       spacing_w;  
    float       spacing_h;
    float       spacing_d;
    void*       pixels;
}ImageInformation;

DLL_EXPORT void         GetTinyDicomLibVersion( int* versions /* must be int[4] */ );
DLL_EXPORT bool         NewDCMA( const char* pFilePath );
DLL_EXPORT bool         OpenDCMA( const char* pFilePath );
DLL_EXPORT bool         NewDCMW( const wchar_t* pFilePath );
DLL_EXPORT bool         OpenDCMW( const wchar_t* pFilePath );
DLL_EXPORT bool         CloseDCM(void);
DLL_EXPORT bool         IsDCMOpened(void);
DLL_EXPORT int          GetElementCount();
DLL_EXPORT int          GetElement(int index, DCMTagElement** pElement);
DLL_EXPORT DCMTagElement* FindElement(DWORD tagID);
DLL_EXPORT bool         AddElement(DCMTagElement* pElement);
DLL_EXPORT bool         AddElementEx(DWORD tagID, char *data, int dataSize);
DLL_EXPORT bool         SaveDCM( const wchar_t* newName);
DLL_EXPORT wchar_t*     GetLastErrMsg();

// -- dictionaries
DLL_EXPORT WORD         GetVR( DWORD tagID );
DLL_EXPORT wchar_t*     GetDicomMeaning( DWORD tagID );

// -- tag create tool
DLL_EXPORT bool         NewElement( DWORD tagID, DCMTagElement** pElement );

// -- tag information tool.
DLL_EXPORT int          ReadInt( DCMTagElement* pElem );
DLL_EXPORT char*        ReadAnsiString( DCMTagElement* pElem );
DLL_EXPORT wchar_t*     ReadWideString( DCMTagElement* pElem );
DLL_EXPORT bool         ReadPixelData( ImageInformation** pII );

DLL_EXPORT int          WriteInt( DCMTagElement* pElem, const int iv );
DLL_EXPORT int          WriteAnsiString( DCMTagElement* pElem, const char* as );
DLL_EXPORT int          WriteWideString( DCMTagElement* pElem, const wchar_t* ws );

// -- related in images.
DLL_EXPORT bool         AddImage( ImageInformation* pII );
#ifdef __cplusplus

#ifdef UNICODE
    #define     NewDCM( _s_ )   NewDCMW( _s_ )
    #define     OpenDCM( _s_ )  OpenDCMW( _s_ )
#else
    #define     NewDCM( _s_ )   NewDCMA( _s_ )
    #define     OpenDCM( _s_ )  OpenDCMA( _s_ )
#endif /// of UNICODE

}
#endif

#endif // __MAIN_H__
