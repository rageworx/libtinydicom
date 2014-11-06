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

typedef struct _TagElement
{
    DWORD   id;
    char    VRtype[2];
    DWORD   size;
    bool    alloced;
    char    staticbuffer[ 64 ];
    void*   dynamicbuffer;
}TagElement;

typedef struct _ImageInformation
{
    int         width;
    int         height;
    int         bpp;        /// bits per a pixel.
    float       spacing_w;
    float       spacing_h;
    void*       pixels;
}ImageInformation;

DLL_EXPORT bool         NewDCM( const wchar_t* pFilePath );
DLL_EXPORT bool         OpenDCM( const wchar_t* pFilePath );
DLL_EXPORT bool         CloseDCM(void);
DLL_EXPORT bool         IsDCMOpened(void);
DLL_EXPORT int          GetElementCount();
DLL_EXPORT int          GetElement(int index, TagElement** pElement);
DLL_EXPORT TagElement*  FindElement(DWORD tagID);
DLL_EXPORT bool         AddElement(TagElement* pElement);
DLL_EXPORT bool         AddElementEx(DWORD tagID, char *data, int dataSize);
DLL_EXPORT bool         SaveDCM( const wchar_t* newName);
DLL_EXPORT wchar_t*     GetLastErrMsg();

// -- dictionaries
DLL_EXPORT WORD         GetVR( DWORD tagID );
DLL_EXPORT wchar_t*     GetDicomMeaning( DWORD tagID );

// -- tag create tool
DLL_EXPORT bool         NewElement( DWORD tagID, TagElement** pElement );

// -- tag information tool.
DLL_EXPORT int          ReadInt( TagElement* pElem );
DLL_EXPORT char*        ReadAnsiString( TagElement* pElem );
DLL_EXPORT wchar_t*     ReadWideString( TagElement* pElem );
DLL_EXPORT bool         ReadPixelData( ImageInformation** pII );

DLL_EXPORT int          WriteInt( TagElement* pElem, const int iv );
DLL_EXPORT int          WriteAnsiString( TagElement* pElem, const char* as );
DLL_EXPORT int          WriteWideString( TagElement* pElem, const wchar_t* ws );

// -- related in images.
DLL_EXPORT bool         AddImage( ImageInformation* pII );

////////////////////////////////////////////////////////////////////////////////
// -- DLL MAIN
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
