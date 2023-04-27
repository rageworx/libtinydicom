#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "dicomtagconfig.h"
#include "dicomdictionary.h"
#include "dicomdecoder.h"
#include "dicomtagreader.h"
#include "stdunicode.h"
#include "swap.h"

using namespace DicomImageViewer;

TagReader::TagReader( wstring &fileName )
{
    createInstance( fileName );
}

TagReader::TagReader( string &fileName )
{
    createInstance( fileName );
}

TagReader::TagReader( const wchar_t* fileName )
{
    wstring fn = fileName;
    createInstance( fn );
}

TagReader::TagReader( const char* fileName )
{
    string fn = fileName;
    createInstance( fn );
}

TagReader::~TagReader()
{
    clearTags();
}

///////////////////////////////////////////////////////////////////

void TagReader::createInstance( wstring &fileName )
{
    // configure me --

    wchar_t *pWC = (wchar_t*)fileName.c_str();
    string  aFn  = convertW2M(pWC);

    createInstance( aFn );
}

void TagReader::createInstance( string &fileName )
{
    bLittleEndian = (bool)DATA_ARRANGE_LITTLE_ENDIAN;
    fileLength = 0;

    fileStream.open( fileName.c_str(), ios::binary | ios::in );

    if( fileStream.is_open() == false )
    {
        bFileLoaded = false;
        fileStream.close();
        return;
    }

    // get file size .
    fileStream.seekg(0,fstream::end);
    fileLength = fileStream.tellg();
    fileStream.seekg(0,fstream::beg);

    // if file size smaller than 128 byte, it is error!
    if(fileLength < ID_OFFSET)
    {
        bFileLoaded = false;
        fileStream.close();
    }

    // skip by ID_OFFSET (128byte)
    fileStream.seekg(ID_OFFSET,ios::cur);

    // read magic and diff. with "DICM".
    char* pMagic = new char[4];
    readString(pMagic,4);
    if(strncmp(pMagic,DICM,4))
    {
        delete[] pMagic;

        bFileLoaded = false;
        fileStream.close();
    }else{
        delete[] pMagic;

        readTags();
        fileStream.close();

        bFileLoaded = true;
    }
}

size_t TagReader::readString( char *pBuf, size_t nLength )
{
    size_t nCurPos = fileStream.tellg();

    if(nCurPos + nLength > fileLength)
        return 0;

    if( pBuf != NULL )
    {
        memset(pBuf,0,4);

        fileStream.read(pBuf,nLength);

        return nLength;
    }

    return 0;
}

size_t TagReader::seekToNext()
{
    while( true )
    {
        uint8_t nRet = readBYTE();

        if ( nRet == 0 )
            return fileStream.tellg();

        size_t nCurPos = fileStream.tellg();

        if ( nCurPos == fileLength )
            break;
    }

    return 0;
}

uint8_t TagReader::readBYTE()
{
    uint8_t aByte   = 0;
    size_t  nCurPos = fileStream.tellg();

    if( nCurPos + 1 < fileLength )
        fileStream.read((char*)&aByte,1);

    return aByte;
}

uint16_t TagReader::readWORD()
{
    uint16_t auint16_t = 0;
    size_t   nCurPos = fileStream.tellg();

    if( nCurPos + 2 < fileLength )
        fileStream.read((char*)&auint16_t,2);

    return auint16_t;
}

uint32_t  TagReader::readDWORD()
{
    uint32_t auint32_t  = 0;
    size_t   nCurPos = fileStream.tellg();

    if( nCurPos + 4 < fileLength )
        fileStream.read((char*)&auint32_t,4);

    return auint32_t;
}

size_t TagReader::getLength( uint16_t* nVR, uint16_t nCarrier )
{
    if ( nVR == NULL )
        return 0;

    uint16_t cVR = *nVR;

    if( bLittleEndian == true )
    {
        cVR = SwapWORD( *nVR );
    }

    switch( cVR )
    {
        // Case if carrier 0, need to next size for Double uint16_t.
        // Related in strings ...
        case UC:
        case UN:
        case UT:
        // Related in floats ...
        case OB:
        // Related in integers ...
        case OF:
        case SQ:
        case OW:
            // Explicit VR with 32-bit length if other two bytes are zero
            if (nCarrier == 0)
                return readDWORD();

            // Implicit VR with 32-bit length
            return nCarrier;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // And others, normal cases ...
        // Related in strings ...
        case AE:
        case AS:
        case AT:
        case CS:
        case DA:
        case DS:
        case DT:
        case IS:
        case LO:
        case LT:
        case PN:
        case SH:
        case ST:
        case TM:
        case UI:
        // Related in floats ...
        case FD:
        case FL:
        // Related in integers ...
        case SL:
        case SS:
        case UL:
        case US:
        case OL:
        case QQ:
            return nCarrier;

        default:
            // Bad VR !
            *nVR = 0;
            break;
    }

    uint32_t tmpDW = 0;

    if ( bLittleEndian == true )
    {
        tmpDW = ( SwapWORD(cVR) << 16 ) | nCarrier;
        return SwapDWORD( tmpDW );
    }

    tmpDW = ( cVR << 16 ) | nCarrier;

    return tmpDW;
}

bool TagReader::readNextTag( TagElement *pTagElem )
{
    uint32_t aTag    = 0;
    bool     bDone   = false;
    uint32_t nTemp   = readDWORD();

    if ( bLittleEndian == true )
    {
        aTag = SwapDWORD( nTemp );
    }
    else
    {
        aTag = nTemp;
    }

    if( aTag > 0 )
    {
        uint32_t nCurReadPos = fileStream.tellg();
        char aSubTag[4] = {0};
        readString(aSubTag,4);

        uint16_t nVR = 0;
        uint16_t nCarrier = 0;
        memcpy(&nVR,aSubTag,2);
        memcpy(&nCarrier,&aSubTag[2],2);

        bool  bVRLenTested = true;
        uint32_t nLen = getLength(&nVR,nCarrier);

        bool bImageTag = false;

        if ( ( aTag & 0xFF000000 ) == 0x7F000000 )
        {
            // Test Some Wrong/Bad DICOM contains Pixel datas in abnormal.
            if ( aTag > 0x7FE00000 )
            {
                bImageTag = true;
            }
        }

        /***
        **  Testing VR and Length ...
        ** added for some VR("SQ") using abnormal size.
        ** Maybe SQ writes in Leadtools.
        ** 0xFFFFFFFFFF means -1 in singed integer.
        ***/

        if ( nLen == 0 )
        {
            bVRLenTested = false;
        }
        else
        if ( nLen == 0xFFFFFFFF ) /// == -1
        {
            bVRLenTested = false;
        }

        if ( ( nLen + nCurReadPos ) > fileLength )
        {
            return false;
        }

        if ( ( bVRLenTested == true ) || ( bImageTag == true ) )
        {
            pTagElem->id = aTag;

            // Handle BAD VR ...
            if ( nVR == 0  )
            {
                if ( bImageTag == true )
                {
                    // Test Some Wrong/Bad DICOM contains Pixel datas in abnormal.
                    if ( aTag == 0x7FE00000 )
                    {
                        nLen = 4;
                        nVR  = 0;
                    }
                    else
                    {
                        // Check nVR.
                        if ( ( nVR != OB ) && ( nVR != OW ) && ( nVR != OL ) )
                        {
                            // Assume it is uint16_t pixel data.
                            nVR = OW;
                        }
                    }

                }
                else
                {
                    // Find right VR.
                    nVR = DicomDictionary::GetVR( aTag );
                }
            }

            memcpy( pTagElem->VRtype, &nVR, 2 );
            pTagElem->size = nLen;

            if ( ( nLen > 0 ) && ( nLen < 0xFFFFFFFF ) )
            {
#ifdef DEBUG
				printf(" ... alloc size : %u\n", nLen );
#endif
                char* pRead = new char[nLen];
                if ( pRead != NULL ) // prevent allocation failure.
                {
                    memset( pRead, 0, nLen );
                    readString(pRead,nLen);
                    if( nLen > MAX_STATICBUFFER_LENGTH )
                    {
                        pTagElem->dynamicbuffer = (uint8_t*)pRead;
                        pTagElem->alloced = true;
                    }
                    else
                    {
                        memset( pTagElem->staticbuffer, 0, MAX_STATICBUFFER_LENGTH );
                        memcpy( pTagElem->staticbuffer, pRead,nLen );
                        pTagElem->alloced = false;
                    }
                }
                else
                {
                    // it failed to read data !
                    pTagElem->dynamicbuffer = NULL;
                    pTagElem->alloced = false;
                    return false;
                }
            }
            else
            if ( nLen == 0xFFFFFFFF )
            {
                // automatic size calculation ...
                // maybe left file size maening of actual pixel data size ?
                nLen =  fileLength - nCurReadPos;
                if ( nLen > 0 )
                {
                    char* pRead = new char[nLen];
                    if ( pRead != NULL )
                    {
                        memset(pRead,0,nLen);
                        readString(pRead,nLen);
                        pTagElem->dynamicbuffer = (uint8_t*)pRead;
                        pTagElem->alloced = true;
                        pTagElem->size = nLen;
                        fileStream.seekg(0,fstream::end);
                    }
                }
                else
                {
                    memset( pTagElem->staticbuffer, 0, MAX_STATICBUFFER_LENGTH );
                    pTagElem->alloced = false;
                    pTagElem->size = 0;
                    return false;
                }
            }

            return true;
        }
        else
        {
            pTagElem->id = aTag;

            memcpy(pTagElem->VRtype,&nVR,2);
            pTagElem->size = 0;
            pTagElem->dynamicbuffer = NULL;

            return true;
        }
    }

    return false;
}

void TagReader::readTags()
{
    TagElement *pTagElem = NULL;
    bool        bRepeat = true;

    while( bRepeat )
    {
        pTagElem = new TagElement;
        if( pTagElem != NULL )
        {
            memset( pTagElem, 0, sizeof(TagElement) );
            bool bRead = readNextTag(pTagElem);

            if( bRead == true )
            {
#ifdef DEBUG
                printf( "#DICOM TAG %08X : %s, (%d bytes)\n",
                        pTagElem->id,
                        pTagElem->VRtype,
                        pTagElem->size );
#endif // DEBUG
                TagElements.push_back(pTagElem);
            }
            else
            {
                delete pTagElem;

                size_t nCurReadPos = fileStream.tellg();

                if ( nCurReadPos >= fileLength )
                {
                    bRepeat = false;
                }
            }
        }
        else
        {
            bRepeat = false;
        }
    }
}

bool TagReader::IsLoaded()
{
    return bFileLoaded;
}

TagElement* TagReader::GetTagElementByID( uint32_t TagID )
{
    if (TagID == 0)
        return NULL;

    size_t nCnt = 0;

    list<TagElement*>::iterator  it;

    for( it=TagElements.begin(); it!=TagElements.end(); advance(it,1) )
    {
        TagElement* pE = *it;
        if( pE != NULL )
        {
            if( pE->id == TagID )
                return pE;
        }
    }

    return NULL;
}
