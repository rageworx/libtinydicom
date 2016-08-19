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

#define LIB_EXPORT __attribute__ ((visibility ("default")))

LIB_EXPORT bool         NewDCM( const wchar_t* pFilePath );
LIB_EXPORT bool         OpenDCM( const wchar_t* pFilePath );
LIB_EXPORT bool         CloseDCM(void);
LIB_EXPORT bool         IsDCMOpened(void);
LIB_EXPORT int          GetElementCount();
LIB_EXPORT int          GetElement(int index, TagElement** pElement);
LIB_EXPORT TagElement*  FindElement(DWORD tagID);
LIB_EXPORT bool         AddElement(TagElement* pElement);
LIB_EXPORT bool         AddElementEx(DWORD tagID, char *data, int dataSize);
LIB_EXPORT bool         SaveDCM( const wchar_t* newName);
LIB_EXPORT wchar_t*     GetLastErrMsg();

// -- dictionaries
LIB_EXPORT WORD         GetVR( DWORD tagID );
LIB_EXPORT wchar_t*     GetDicomMeaning( DWORD tagID );

// -- tag create tool
LIB_EXPORT bool         NewElement( DWORD tagID, TagElement** pElement );

// -- tag information tool.
LIB_EXPORT int          ReadInt( TagElement* pElem );
LIB_EXPORT char*        ReadAnsiString( TagElement* pElem );
LIB_EXPORT wchar_t*     ReadWideString( TagElement* pElem );
LIB_EXPORT bool         ReadPixelData( ImageInformation* pII );

LIB_EXPORT int          WriteInt( TagElement* pElem, const int iv );
LIB_EXPORT int          WriteAnsiString( TagElement* pElem, const char* as );
LIB_EXPORT int          WriteWideString( TagElement* pElem, const wchar_t* ws );

// -- related in images.
LIB_EXPORT bool         AddImage( ImageInformation* pII );

#ifdef __cplusplus
}
#endif

#endif // __LIBDCM_H__
