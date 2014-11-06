#include <windows.h>
#include <tchar.h>

#include "stools.h"

#include "dicomtagreader.h"
#include "dicomtagwriter.h"
#include "dicomdictionary.h"

#include "libdcm.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static DicomImageViewer::TagReader*   pReader         = NULL;
static DicomImageViewer::TagWriter*   pWriter         = NULL;

#if defined(UNICODE) || defined(_UNICODE)
    #define TSTRING wstring
    #ifndef _T
        #define _T(x) L##x
    #endif
#else
    #define TSTRING string
#endif

static TSTRING lastErrMsg;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void tool_replace_element(TagElement* pDst, TagElement* pSrc)
{
    if(!pSrc)
        return;

    if(!pDst)
        return;

    pDst->id = pSrc->id;
    memcpy(pDst->VRtype, pSrc->VRtype, 2);
    pDst->size = pSrc->size;

    if(pDst->alloced)
    {
        free(pDst->dynamicbuffer);
        pDst->dynamicbuffer = NULL;
        pDst->alloced = false;
    }

    if(pSrc->alloced)
    {
        pDst->dynamicbuffer = malloc(pSrc->size);
        memcpy(pDst->dynamicbuffer, pSrc->dynamicbuffer, pSrc->size);
    }else{
        memset(pDst->staticbuffer, 0, MAX_STATICBUFFER_LENGTH);
        memcpy(pDst->staticbuffer, pSrc->staticbuffer, MAX_STATICBUFFER_LENGTH);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// New DCM is simple.
// Just make an empty DCM and read it again !
DLL_EXPORT bool NewDCM( const wchar_t* pFilePath )
{
    lastErrMsg.clear();

    if( pReader != NULL )
    {
        delete pReader;
        pReader = NULL;
    }

    if( pWriter != NULL )
    {
        delete pWriter;
        pWriter = NULL;
    }

    /*
    pWriter = new DicomImageViewer::TagWriter( pFilePath );
    if ( pWriter != NULL )
    {
        pWriter->Write();

        delete pWriter;
        pWriter = NULL;
    }

    CloseDCM();

    return OpenDCM( pFilePath );
    */

    pReader = new DicomImageViewer::TagReader( pFilePath );
    if ( pReader != NULL )
    {
        return true;
    }

    return false;
}

DLL_EXPORT bool OpenDCM( const wchar_t* pFilePath )
{
    lastErrMsg.clear();

    if(pReader)
    {
        lastErrMsg = _T("File already open.");
        return false;
    }

    if(pWriter)
    {
        delete pWriter;
        pWriter = NULL;
    }

    pReader = new DicomImageViewer::TagReader(pFilePath);
    if ( pReader != NULL )
    {
        if ( pReader->IsLoaded() )
        {
            return true;
        }
    }

    lastErrMsg = _T("Failed to open DCM file.");
    return false;
}

DLL_EXPORT bool CloseDCM(void)
{
    lastErrMsg.clear();

    if(pWriter)
    {
        delete pWriter;
        pWriter = NULL;
    }

    if(pReader)
    {
        delete pReader;
        pReader = NULL;
    }

    return true;
}

DLL_EXPORT bool IsDCMOpened(void)
{
    if(pReader)
    {
        return pReader->IsLoaded();
    }

    return false;
}

DLL_EXPORT int GetElementCount()
{
    lastErrMsg.clear();

    if(pReader)
    {
        return pReader->GetTagCount();
    }

    lastErrMsg = _T("DICOM not open.");

    return -1;
}

DLL_EXPORT int GetElement(int index, TagElement** pElement)
{
    lastErrMsg.clear();

    if(pReader)
    {
        if(index < pReader->GetTagCount())
        {
            *pElement = (TagElement*)pReader->GetTagElement(index);
            if(!pElement)
            {
                lastErrMsg = _T("DICOM tag not found");
                return -3;
            }
            return index;
        }
        else
        {
            lastErrMsg = _T("Out of index");
            return -2;
        }
    }

    return -1;
}

DLL_EXPORT TagElement* FindElement(DWORD tagID)
{
    lastErrMsg.clear();

    if(pReader)
    {
        TagElement *pRet = (TagElement*)pReader->FindTagElement(tagID);
        return pRet;
    }

    lastErrMsg = _T("DICOM not open");

    return NULL;
}

DLL_EXPORT bool AddElement(TagElement* pElement)
{
    lastErrMsg.clear();

    if(!pElement)
    {
        lastErrMsg = _T("DICOM tag element is NULL");
        return false;
    }

    TagElement* pFoundElem      = NULL;

    if(pReader)
    {
        pFoundElem = (TagElement*)pReader->FindTagElement(pElement->id);
        if(pFoundElem)
        {
            // if found same tag, replace it.
            tool_replace_element(pFoundElem, pElement);
            return true;
        }
        else
        {
            pReader->AddTagElement((DicomImageViewer::TagElement*)pElement);
            return true;
        }

    }

    lastErrMsg = _T("DICOM not open");

    return false;
}

DLL_EXPORT bool AddElementEx(DWORD tagID, char *data, int dataSize)
{
    lastErrMsg.clear();

    TagElement* pNewElem = new TagElement();

    memset(pNewElem, 0, sizeof(TagElement));

    pNewElem->id = tagID;

    WORD newVR = DicomImageViewer::DicomDictionary::GetVR(tagID);

    memcpy(pNewElem->VRtype, &newVR, 2);
    if(dataSize >= MAX_STATICBUFFER_LENGTH)
    {
        pNewElem->alloced = TRUE;
        pNewElem->dynamicbuffer = malloc(dataSize);
        memcpy(pNewElem->dynamicbuffer, data, dataSize);
    }else{
        memcpy(pNewElem->staticbuffer, data, dataSize);
    }

    if(AddElement(pNewElem) == false)
    {
        if(pNewElem->alloced)
        {
            free(pNewElem->dynamicbuffer);
        }
        delete pNewElem;

        lastErrMsg = _T("Failed to adding new DICOM tag");

        return false;
    }

    return true;
}

DLL_EXPORT bool SaveDCM( const wchar_t* newName )
{
    lastErrMsg.clear();

    if( newName == NULL )
    {
        lastErrMsg = _T("save file name not decided");
        return false;
    }

    if(pReader)
    {
        if (pWriter)
        {
            delete pWriter;
            pWriter = NULL;
        }

        if(!pWriter)
        {
            pWriter = new DicomImageViewer::TagWriter(newName);

            if(!pWriter)
            {
                lastErrMsg = _T("");
                return false;
            }

            int tagCount = pReader->GetTagCount();

            for(int cnt=0; cnt<tagCount; cnt++)
            {
                TagElement* pSrcElem = (TagElement*)pReader->GetTagElement(cnt);
                if(pSrcElem)
                {
                    TagElement *pNewElem = new TagElement();

                    if(pNewElem)
                    {
                        memset(pNewElem,0,sizeof(TagElement));

                        // copy it ..
                        pNewElem->id = pSrcElem->id;
                        memcpy(pNewElem->VRtype,pSrcElem->VRtype,2);
                        pNewElem->alloced = pSrcElem->alloced;
                        pNewElem->size = pSrcElem->size;

                        if(pSrcElem->alloced)
                        {
                            if(pSrcElem->size > 0)
                            {
                                pNewElem->dynamicbuffer = malloc(pSrcElem->size);
                                if(pNewElem->dynamicbuffer)
                                    memcpy(pNewElem->dynamicbuffer, pSrcElem->dynamicbuffer, pSrcElem->size);
                            }else{
                                pNewElem->alloced = FALSE;
                            }

                        }else{
                            memcpy(pNewElem->staticbuffer, pSrcElem->staticbuffer, MAX_STATICBUFFER_LENGTH);
                        }

                        pWriter->AddTagElement((DicomImageViewer::TagElement*)pNewElem);
                    }
                }
            }

            // New DICOM tag library added new function: Sort !
            pWriter->Sort();

            // Then write DICOM tags to disk.
            pWriter->Write();

            delete pWriter;
            pWriter = NULL;

            return TRUE;
        }
    }

    lastErrMsg = _T("");

    return FALSE;
}

DLL_EXPORT WORD  GetVR(DWORD tagID)
{
    return DicomImageViewer::DicomDictionary::GetVR(tagID);
}

DLL_EXPORT wchar_t* GetDicomMeaning(DWORD tagID)
{
#if defined(UNICODE) || defined(_UNICODE)
    const char* refstr = DicomImageViewer::DicomDictionary::GetMean(tagID);
    return ConvertFromMBCS( refstr );
#else
    return DicomImageViewer::DicomDictionary::GetMean(tagID);
#endif
}

DLL_EXPORT bool NewElement( DWORD tagID, TagElement** pElement )
{
    lastErrMsg.clear();

    if( pReader == NULL )
    {
        lastErrMsg = _T("Tag reader not loaded.");
        return false;
    }

    if ( FindElement( tagID ) != NULL )
        return false;       /// Already Exist.

    TagElement* newElem = new TagElement();
    if( newElem != NULL )
    {
        memset( newElem, 0, sizeof( TagElement ) );
        WORD wVR = GetVR( tagID );
        newElem->id = tagID;
        memcpy( newElem->VRtype, &wVR, 2 );

        if ( AddElement( newElem ) == true )
        {
            *pElement = newElem;
            return true;
        }
    }

    lastErrMsg = _T("");

    return false;
}

DLL_EXPORT wchar_t* GetLastErrMsg()
{
    return (wchar_t*)lastErrMsg.c_str();
}

DLL_EXPORT int ReadInt( TagElement* pElem )
{
    int retInt = 0;

    if ( pElem != NULL )
    {
        if ( strncmp( pElem->VRtype, "US", 2 ) == 0 )
        {
            WORD tmpUS = 0;
            memcpy( &tmpUS, pElem->staticbuffer, 2 );
            retInt = tmpUS;
        }
        else
        if ( strncmp( pElem->VRtype, "SS", 2 ) == 0 )
        {
            short tmpSS = 0;
            memcpy( &tmpSS, pElem->staticbuffer, 2 );
            retInt = tmpSS;
        }
        else
        if ( strncmp( pElem->VRtype, "UL", 2 ) == 0 )
        {
            DWORD tmpDW = 0;
            memcpy( &tmpDW, pElem->staticbuffer, 4 );
            retInt = (int)tmpDW;
        }
        else
        if ( strncmp( pElem->VRtype, "SL", 2 ) == 0 )
        {
            long tmpLG = 0;
            memcpy( &tmpLG, pElem->staticbuffer, 4 );
            retInt = tmpLG;
        }
    }

    return retInt;
}

DLL_EXPORT char* ReadAnsiString( TagElement* pElem )
{
    if ( pElem != NULL )
    {
        if  ( strncmp( pElem->VRtype, "DS", 2 ) == 0 )
        {
            if ( pElem->alloced == true )
            {
                return strdup( (char*)pElem->dynamicbuffer );
            }
            else
            {
                return strdup( (char*)pElem->staticbuffer );
            }
        }
    }

    return NULL;
}

DLL_EXPORT wchar_t* ReadWideString( TagElement* pElem )
{
    if ( pElem != NULL )
    {
        if  ( strncmp( pElem->VRtype, "DS", 2 ) == 0 )
        {
            if ( pElem->alloced == true )
            {
                return wcsdup( (wchar_t*)pElem->dynamicbuffer );
            }
            else
            {
                return wcsdup( (wchar_t*)pElem->staticbuffer );
            }
        }
    }

    return NULL;
}

DLL_EXPORT bool ReadPixelData( ImageInformation* pII )
{
    if ( pII == NULL )
        return false;

    TagElement* pTagRow = FindElement( 0x00280010 );    /// Rows
    TagElement* pTagCol = FindElement( 0x00280011 );    /// Cols
    TagElement* pTagBit = FindElement( 0x00280101 );    /// Using "bits stored"
    TagElement* pTagPSp = FindElement( 0x00280030 );    /// Pixel spacing
    TagElement* pTagPxs = FindElement( 0x7FE00010 );

    if ( pTagPSp != NULL )
    {
        if ( strncmp( pTagPSp->VRtype, "DS", 2 ) == 0 )
        {
            char* refstr = (char*)pTagPSp->staticbuffer;
            if ( pTagPSp->alloced == true )
            {
                refstr = (char*)pTagPSp->dynamicbuffer;
            }

            if ( refstr != NULL )
            {
                // find seperator "\" or "/"
                char  cSep[2] = "\\";
                char* pSep    = strstr( refstr, cSep );
                if ( pSep == NULL )
                {
                    cSep[0] = '/';
                    pSep    = strstr( refstr, cSep );
                }

                if ( pSep != NULL )
                {
                    char* tok1 = strtok( refstr, cSep );
                    char* tok2 = strtok( NULL, cSep );

                    if ( tok1 != NULL )
                    {
                        pII->spacing_w = atoi( tok1 );
                    }

                    if ( tok2 != NULL )
                    {
                        pII->spacing_h = atoi( tok2 );
                    }
                }
            }
        }
    }

    if ( ( pTagRow != NULL ) || ( pTagCol != NULL ) || ( pTagBit != NULL ) )
    {
        pII->bpp    = ReadInt( pTagBit );
        pII->width  = ReadInt( pTagRow );
        pII->height = ReadInt( pTagCol );

        if ( pTagPxs != NULL )
        {
            if ( pTagPxs->alloced == true )
            {
                pII->pixels = pTagPxs->dynamicbuffer;
            }
            else
            {
                pII->pixels = pTagPxs->staticbuffer;
            }
        }

        return true;
    }

    return false;
}

DLL_EXPORT int  WriteInt(  TagElement* pElem, const int iv )
{
    int writeInt = 0;

    if ( pElem != NULL )
    {
        if ( strncmp( pElem->VRtype, "US", 2 ) == 0 )
        {
            WORD tmpUS = (WORD)iv;
            memcpy( pElem->staticbuffer, &tmpUS, 2 );
            pElem->size = 2;
            writeInt = tmpUS;
        }
        else
        if ( strncmp( pElem->VRtype, "SS", 2 ) == 0 )
        {
            short tmpSS = (short)iv;
            memcpy( pElem->staticbuffer, &tmpSS, 2 );
            pElem->size = 2;
            writeInt = tmpSS;
        }
        else
        if ( strncmp( pElem->VRtype, "UL", 2 ) == 0 )
        {
            DWORD tmpDW = (DWORD)iv;
            memcpy( pElem->staticbuffer, &tmpDW, 4 );
            pElem->size = 4;
            writeInt = (int)tmpDW;
        }
        else
        if ( strncmp( pElem->VRtype, "SL", 2 ) == 0 )
        {
            long tmpLG = (long)iv;
            memcpy( pElem->staticbuffer, &tmpLG, 4 );
            pElem->size = 4;
            writeInt = tmpLG;
        }
    }

    return writeInt;
}

DLL_EXPORT int  WriteAnsiString( TagElement* pElem, const char* as )
{
    int writeInt = 0;

    if ( pElem != NULL )
    {
        int wLen = strlen( as );
        if ( wLen >= 64 )
        {
            if ( pElem->dynamicbuffer != NULL )
            {
                delete[] pElem->dynamicbuffer;
                pElem->dynamicbuffer = NULL;
                pElem->alloced = false;
            }

            pElem->dynamicbuffer = new char[ wLen + 1 ];
            if ( pElem->dynamicbuffer != NULL )
            {
                memset( pElem->dynamicbuffer, 0, wLen + 1 );
                memcpy( pElem->dynamicbuffer, as, wLen );
                pElem->size = wLen;
                pElem->alloced = true;

                writeInt = wLen;
            }
        }
        else
        {
            if ( pElem->dynamicbuffer != NULL )
            {
                delete[] pElem->dynamicbuffer;
                pElem->dynamicbuffer = NULL;
                pElem->alloced = false;
            }

            memset( pElem->staticbuffer, 0, 64 );
            memcpy( pElem->staticbuffer, as, wLen );
            pElem->size = wLen;

            writeInt = wLen;
        }
    }

    return writeInt;
}

DLL_EXPORT int  WriteWideString( TagElement* pElem, const wchar_t* ws )
{
    int writeInt = 0;

    if ( pElem != NULL )
    {
        int wLen = wcslen( ws );
        if ( wLen >= 32 )
        {
            if ( pElem->dynamicbuffer != NULL )
            {
                delete[] pElem->dynamicbuffer;
                pElem->dynamicbuffer = NULL;
                pElem->alloced = false;
            }

            pElem->dynamicbuffer = new wchar_t[ wLen + 1 ];
            if ( pElem->dynamicbuffer != NULL )
            {
                memset( pElem->dynamicbuffer, 0, ( wLen + 1 ) * 2 );
                memcpy( pElem->dynamicbuffer, ws, wLen * 2 );
                pElem->size = wLen * 2;
                pElem->alloced = true;

                writeInt = wLen * 2;
            }
        }
        else
        {
            if ( pElem->dynamicbuffer != NULL )
            {
                delete[] pElem->dynamicbuffer;
                pElem->dynamicbuffer = NULL;
                pElem->alloced = false;
            }

            memset( pElem->staticbuffer, 0, 64 );
            memcpy( pElem->staticbuffer, ws, wLen * 2 );
            pElem->size = wLen * 2;

            writeInt = wLen;
        }
    }

    return writeInt;
}

DLL_EXPORT bool AddImage( ImageInformation* pII )
{
    if ( pII == NULL )
        return false;

    if (!pReader)
        return false;

    // Write widht, height in Column and Rows.
    if ( ( pII->width != 0 ) && ( pII->height != 0 ) )
    {
        TagElement* tagCol = FindElement( 0x00280011 );
        if ( tagCol == NULL )
        {
            if ( NewElement( 0x00280011, &tagCol ) == false )
            {
                return false;
            }
        }

        WriteInt( tagCol, pII->width );

        TagElement* tagRow = FindElement( 0x00280010 );
        if ( tagRow == NULL )
        {
            if ( NewElement( 0x00280010, &tagRow ) == false )
            {
                return false;
            }
        }

        WriteInt( tagRow, pII->height );
    }

    int bitsalloced = 0;

    if ( pII->bpp != 0 )
    {
        TagElement* tagBS = FindElement( 0x00280101 );
        if ( tagBS == NULL )
        {
            if ( NewElement( 0x00280101, &tagBS ) == false )
            {
                return false;
            }
        }

        if ( pII->bpp <= 8 )
        {
            bitsalloced = 8;
        }
        else
        if ( pII->bpp <= 16 )
        {
            bitsalloced = 16;
        }
        else
        if ( pII->bpp <= 32 )
        {
            bitsalloced = 32;
        }

        TagElement* tagBA = FindElement( 0x00280100 );
        if ( tagBA == NULL )
        {
            if ( NewElement( 0x00280100, &tagBA ) == false )
            {
                return false;
            }
        }

        WriteInt( tagBS, pII->bpp );
        WriteInt( tagBA, bitsalloced );
    }

    // Write pixel spacing information.
    if ( ( pII->spacing_w != 0.0f ) && ( pII->spacing_h != 0.0f ) )
    {
        TagElement* tagPS = FindElement( 0x00280030 );
        if ( tagPS == NULL )
        {
            if ( NewElement( 0x00280030, &tagPS ) == false )
            {
                return false;
            }
        }

        char mappedstr[64] = {0};

        sprintf( mappedstr, "%.6f\\%0.6f ", pII->spacing_w, pII->spacing_h );

        WriteAnsiString( tagPS, mappedstr );
    }

    // Write Pixels.
    if ( pII->pixels != NULL )
    {
        TagElement* tagPxs = FindElement( 0x7FE00010 );
        if ( tagPxs == NULL )
        {
            if ( NewElement( 0x7FE00010, &tagPxs ) == false )
            {
                return false;
            }
        }

        tagPxs->size = pII->width * pII->height * ( bitsalloced / 8 );

        if( tagPxs->size > 64 )
        {
            tagPxs->dynamicbuffer = new char[ tagPxs->size ];
            if ( tagPxs->dynamicbuffer != NULL )
            {
                tagPxs->alloced = true;
                memset( tagPxs->dynamicbuffer, 0, tagPxs->size );
                memcpy( tagPxs->dynamicbuffer, pII->pixels, tagPxs->size );
            }
            else
            {
                memset( tagPxs->staticbuffer, 0, 64 );
                memcpy( tagPxs->staticbuffer, pII->pixels, tagPxs->size );
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_THREAD_ATTACH:
            return false;

        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE; // succesful
}
