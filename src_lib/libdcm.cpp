#ifdef _WIN32
    #include <windows.h>
    #include <tchar.h>
#endif /// of _WIN32
	
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "dicomtagconfig.h"
#include "dicomtagreader.h"
#include "dicomtagwriter.h"
#include "dicomdictionary.h"

#include "libdcm.h"
#include "stdunicode.h"
#include "version.h"

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
    #endif /// of _T
#else
    #define TSTRING string
    #ifndef _T    
        #define _T
    #endif /// of _T
#endif /// of defined, UNICODE or _UNICODE

static TSTRING lastErrMsg;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void tool_replace_element(DCMTagElement* pDst, DCMTagElement* pSrc)
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

static DCMTagElement* FindPixelDataElement()
{
    lastErrMsg.clear();

    if ( pReader != NULL )
    {
        unsigned tagsz = pReader->GetTagCount();
        if ( tagsz > 0 )
        {
            DCMTagElement* pAssume = NULL;

            for( unsigned cnt=0; cnt<tagsz; cnt++ )
            {
                DCMTagElement* pRet = (DCMTagElement*)pReader->GetTagElement( cnt );

                if ( ( pRet->id & 0xFF00FFFF ) > 0x7F000000 )
                {
                    // Check VR
                    if ( ( strncmp( pRet->VRtype, "OW", 2 ) == 0 ) ||
                         ( strncmp( pRet->VRtype, "OB", 2 ) == 0 ) )
                    {
                        return pRet;
                    }
                    else
                    if ( pRet->alloced == true ) /// Assume to Pixel Data
                    {
                        pAssume = pRet;
                    }
                }
            }

            // Some DCM tags not using VR, damn. So I assume it.
            if ( pAssume != NULL )
            {
                return pAssume;
            }
        }
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

LIB_EXPORT void GetTinyDicomLibVersion( int* versions )
{
    if ( versions != NULL )
    {
        int verarrs[] = {LIB_VERSION};
        memcpy( versions, verarrs, sizeof(int)*4 );
    }
}

// New DCM is simple.
LIB_EXPORT bool NewDCMW( const wchar_t* pFilePath )
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

    pReader = new DicomImageViewer::TagReader( pFilePath );
    if ( pReader != NULL )
    {
        return true;
    }

    lastErrMsg = _T("Cannot make a mew reader instance.");

    return false;
}

LIB_EXPORT bool NewDCMA( const char* pFilePath )
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

    pReader = new DicomImageViewer::TagReader( pFilePath );
    if ( pReader != NULL )
    {
        return true;
    }

    lastErrMsg = _T("Cannot make a new reader instance.");

    return false;
}

LIB_EXPORT bool OpenDCMW( const wchar_t* pFilePath )
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

LIB_EXPORT bool OpenDCMA( const char* pFilePath )
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

LIB_EXPORT bool CloseDCM(void)
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

LIB_EXPORT bool IsDCMOpened(void)
{
    if(pReader)
    {
        return pReader->IsLoaded();
    }

    return false;
}

LIB_EXPORT int GetElementCount()
{
    lastErrMsg.clear();

    if(pReader)
    {
        return pReader->GetTagCount();
    }

    lastErrMsg = _T("DICOM not open.");

    return -1;
}

LIB_EXPORT int GetElement(int index, DCMTagElement** pElement)
{
    lastErrMsg.clear();

    if( pReader != NULL )
    {
        if( (unsigned)index < pReader->GetTagCount() )
        {
            DicomImageViewer::TagElement* pElem = pReader->GetTagElement((unsigned int)index);
            *pElement = (DCMTagElement*)pElem;
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

LIB_EXPORT int FindElementIndex(DWORD tagID)
{
    lastErrMsg.clear();

    if( pReader != NULL )
    {
        unsigned tagsz = pReader->GetTagCount();
        if ( tagsz > 0 )
        {
            for( unsigned cnt=0; cnt<tagsz; cnt++ )
            {
                DCMTagElement* pRet = (DCMTagElement*)pReader->GetTagElement( cnt );

                if ( pRet != NULL )
                {
                    if ( pRet->id == tagID )
                        return cnt;
                }
            }
        }
    }

    lastErrMsg = _T("DICOM may not open, or could not find tag ID.");

    return -1;
}

LIB_EXPORT DCMTagElement* FindElement(DWORD tagID)
{
    lastErrMsg.clear();

    if( pReader != NULL )
    {
        DCMTagElement* pRet = (DCMTagElement*)pReader->FindTagElement( tagID );
        if ( pRet != NULL )
        {
            return pRet;
        }
    }

    lastErrMsg = _T("DICOM may not open, or could not find tag ID.");

    return NULL;
}

LIB_EXPORT bool AddElement(DCMTagElement* pElement)
{
    lastErrMsg.clear();

    if(!pElement)
    {
        lastErrMsg = _T("DICOM tag element is NULL");
        return false;
    }

    DicomImageViewer::TagElement* pFoundElem      = NULL;

    if(pReader)
    {
        pFoundElem = pReader->FindTagElement( pElement->id );
        if(pFoundElem)
        {
            // if found same tag, replace it.
            tool_replace_element( (DCMTagElement*)pFoundElem, (DCMTagElement*)pElement);
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

LIB_EXPORT bool AddElementEx(DWORD tagID, char *data, int dataSize)
{
    lastErrMsg.clear();

    DicomImageViewer::TagElement* pNewElem = new DicomImageViewer::TagElement();

    memset(pNewElem, 0, sizeof(DicomImageViewer::TagElement));

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

    if(AddElement( (DCMTagElement*)pNewElem ) == false)
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

LIB_EXPORT bool SaveDCM( const wchar_t* newName )
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
                DicomImageViewer::TagElement* pSrcElem = pReader->GetTagElement( cnt );
                if(pSrcElem)
                {
                    DicomImageViewer::TagElement *pNewElem = new DicomImageViewer::TagElement();

                    if(pNewElem)
                    {
                        memset(pNewElem,0,sizeof(DicomImageViewer::TagElement));

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

LIB_EXPORT WORD  GetVR(DWORD tagID)
{
    return DicomImageViewer::DicomDictionary::GetVR(tagID);
}

LIB_EXPORT wchar_t* GetDicomMeaning(DWORD tagID)
{
    const char* refstr = DicomImageViewer::DicomDictionary::GetMean(tagID);
    return convertM2W( refstr );
}

LIB_EXPORT bool NewElement( DWORD tagID, DCMTagElement** pElement )
{
    lastErrMsg.clear();

    if( pReader == NULL )
    {
        lastErrMsg = _T("Tag reader not loaded.");
        return false;
    }

    if ( FindElement( tagID ) != NULL )
        return false;       /// Already Exist.

    DCMTagElement* newElem = (DCMTagElement*)new DicomImageViewer::TagElement();
    if( newElem != NULL )
    {
        memset( newElem, 0, sizeof( DCMTagElement ) );
        WORD wVR = GetVR( tagID );
        newElem->id = tagID;
        memcpy( newElem->VRtype, &wVR, 2 );

        if ( AddElement( newElem ) == true )
        {
            *pElement = newElem;
            return true;
        }
    }

    lastErrMsg.clear();

    return false;
}

LIB_EXPORT wchar_t* GetLastErrMsg()
{
    return (wchar_t*)lastErrMsg.c_str();
}

LIB_EXPORT int ReadInt( DCMTagElement* pElem )
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

LIB_EXPORT char* ReadAnsiString( DCMTagElement* pElem )
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

LIB_EXPORT wchar_t* ReadWideString( DCMTagElement* pElem )
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

LIB_EXPORT bool ReadPixelData( ImageInformation* pII )
{
    if ( pII == NULL )
        return false;

    DCMTagElement* pTagRow = FindElement( 0x00280010 );    /// Rows
    DCMTagElement* pTagCol = FindElement( 0x00280011 );    /// Cols
    DCMTagElement* pTagPln = FindElement( 0x00280012 );    /// Planes
    DCMTagElement* pTagBit = FindElement( 0x00280101 );    /// Using "bits stored"
    DCMTagElement* pTagPSp = FindElement( 0x00280030 );    /// Pixel spacing
    DCMTagElement* pTagPxs = FindPixelDataElement();       /// Find Pixel container.

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
                
        if ( pTagPln != NULL )
        {
            pII->planes = ReadInt( pTagPln );
        }
        else
        {
            pII->planes = 0; /// not availed.
        }

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

            return true;
        }

    }

    return false;
}

LIB_EXPORT int  WriteInt(  DCMTagElement* pElem, const int iv )
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

LIB_EXPORT int  WriteAnsiString( DCMTagElement* pElem, const char* as )
{
    int writeInt = 0;

    if ( pElem != NULL )
    {
        int wLen = strlen( as );
        int bLen = wLen + (wLen % 2); // need 2 bytes padding.
        if ( wLen >= 64 )
        {
            if ( pElem->dynamicbuffer != NULL )
            {
                delete[] (char*)pElem->dynamicbuffer;
                pElem->dynamicbuffer = NULL;
                pElem->alloced = false;
            }

            pElem->dynamicbuffer = new char[ bLen + 1 ];
            if ( pElem->dynamicbuffer != NULL )
            {
                memset( pElem->dynamicbuffer, 0, bLen + 1 );
                memcpy( pElem->dynamicbuffer, as, wLen );
                if ( wLen != bLen )
                {
                    char* ptrx = (char*)pElem->dynamicbuffer;
                    ptrx += wLen;
                    *ptrx = 0x20;
                }
                pElem->size = bLen;
                pElem->alloced = true;

                writeInt = bLen;
            }
        }
        else
        {
            if ( pElem->dynamicbuffer != NULL )
            {
                delete[] (char*)pElem->dynamicbuffer;
                pElem->dynamicbuffer = NULL;
                pElem->alloced = false;
            }

            memset( pElem->staticbuffer, 0, 64 );
            memcpy( pElem->staticbuffer, as, wLen );
            if ( wLen != bLen )
            {
                strcat( pElem->staticbuffer, " " );
            }

            pElem->size = bLen;

            writeInt = bLen;
        }
    }

    return writeInt;
}

LIB_EXPORT int  WriteWideString( DCMTagElement* pElem, const wchar_t* ws )
{
    int writeInt = 0;

    if ( pElem != NULL )
    {
        int wLen = wcslen( ws );
        if ( wLen >= 32 )
        {
            if ( pElem->dynamicbuffer != NULL )
            {
                delete[] (char*)pElem->dynamicbuffer;
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
                delete[] (char*)pElem->dynamicbuffer;
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

LIB_EXPORT bool AddImage( ImageInformation* pII )
{
    if ( pII == NULL )
        return false;

    if (!pReader)
        return false;

    // Write widht, height in Column and Rows.
    if ( ( pII->width > 0 ) && ( pII->height > 0 ) )
    {
        DCMTagElement* tagCol = FindElement( 0x00280011 );
        if ( tagCol == NULL )
        {
            if ( NewElement( 0x00280011, &tagCol ) == false )
            {
                return false;
            }
        }

        WriteInt( tagCol, pII->width );

        DCMTagElement* tagRow = FindElement( 0x00280010 );
        if ( tagRow == NULL )
        {
            if ( NewElement( 0x00280010, &tagRow ) == false )
            {
                return false;
            }
        }

        WriteInt( tagRow, pII->height );
        
        DCMTagElement* tagPlanes = NULL;
        
        if ( pII->planes > 0 )
        {
            tagPlanes = FindElement( 0x00280012 );
            if ( tagPlanes == NULL )
            {
                if ( NewElement( 0x00280012, &tagPlanes ) == false )
                {
                    return false;
                }
            }
            
            WriteInt( tagPlanes, pII->planes );
        }
    }

    int bitsalloced = 0;

    if ( pII->bpp != 0 )
    {
        DCMTagElement* tagBS = FindElement( 0x00280101 );
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

        DCMTagElement* tagBA = FindElement( 0x00280100 );
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
        DCMTagElement* tagPS = FindElement( 0x00280030 );
        if ( tagPS == NULL )
        {
            if ( NewElement( 0x00280030, &tagPS ) == false )
            {
                return false;
            }
        }

        char mappedstr[64] = {0};

        snprintf( mappedstr, 64, "%.6f\\%0.6f ", pII->spacing_w, pII->spacing_h );

        WriteAnsiString( tagPS, mappedstr );
    }

    // Write Pixels.
    if ( pII->pixels != NULL )
    {
        //DCMTagElement* tagPxs = FindElement( 0x7FE00010 );
        DCMTagElement* tagPxs = FindPixelDataElement();
        if ( tagPxs == NULL )
        {
            if ( NewElement( 0x7FE00010, &tagPxs ) == false )
            {
                return false;
            }
        }

        // Check VR is OW or OB.
        // OB may not working for many DICOM viewers.
        if ( strncmp( tagPxs->VRtype, "OW", 2 ) != 0 )
        {
            tagPxs->VRtype[0] = 'O';
            tagPxs->VRtype[1] = 'W';
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
