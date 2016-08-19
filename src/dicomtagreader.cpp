#include <windows.h>
#include <tchar.h>

#include "dicomtagconfig.h"
#include "dicomdictionary.h"
#include "dicomdecoder.h"
#include "dicomtagreader.h"
#include "stools.h"

using namespace DicomImageViewer;

TagReader::TagReader( wstring &fileName )
{
    createInstance(fileName);
}

TagReader::TagReader( const wchar_t* fileName )
{
    wstring fn = fileName;
    createInstance(fn);
}

TagReader::TagReader( const char* fileName )
{
    wstring fn = ConvertFromMBCS(fileName);
    createInstance(fn);
}

TagReader::~TagReader()
{
    clearTags();
}

///////////////////////////////////////////////////////////////////

void    TagReader::createInstance(wstring &fileName)
{
    // configure me --
    bLittleEndian = DATA_ARRANGE_LITTLE_ENDIAN;
    fileLength = 0;

    // file open --
#ifdef  __GNUC__
    wchar_t *pWC = (wchar_t*)fileName.c_str();
    char *pFn    = ConvertFromUnicode(pWC);
    fileStream.open(pFn, ios::binary | ios::in );
#else
    fileStream.open(fileName.c_str(), ios::binary | ios::in );
#endif

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
        delete pMagic;

        bFileLoaded = false;
        fileStream.close();
    }else{
        delete pMagic;

        readTags();
        fileStream.close();

        bFileLoaded = true;
    }
}

int     TagReader::readString(char *pBuf, DWORD nLength)
{
    DWORD   nCurPos = fileStream.tellg();

    if(nCurPos + nLength > fileLength)
        return NULL;

    if(pBuf)
    {
        memset(pBuf,0,4);

        fileStream.read(pBuf,nLength);

        return nLength;
    }

    return 0;
}

BYTE    TagReader::readBYTE()
{
    BYTE    aByte   = 0;
    DWORD   nCurPos = fileStream.tellg();

    if(nCurPos + 1 < fileLength)
        fileStream.read((char*)&aByte,1);

    return aByte;
}

WORD    TagReader::readWORD()
{
    WORD    aWord   = 0;
    DWORD   nCurPos = fileStream.tellg();

    if(nCurPos + 2 < fileLength)
        fileStream.read((char*)&aWord,2);

    return aWord;
}

DWORD   TagReader::readDWORD()
{
    DWORD   aDWord  = 0;
    DWORD   nCurPos = fileStream.tellg();

    if(nCurPos + 4 < fileLength)
        fileStream.read((char*)&aDWord,4);

    return aDWord;
}

DWORD   TagReader::getLength(WORD nVR,WORD nCarrier)
{
    WORD cVR = nVR;

    if(bLittleEndian)
    {
        // Swap it !!
        BYTE *pA1 = (BYTE*)&nVR;
        BYTE *pA2 = pA1+1;
        cVR = (*pA1 << 8 ) + *pA2;
    }


    switch(cVR)
    {
        case OB:
        case OW:
        case UN:
        case UT:
            // Explicit VR with 32-bit length if other two bytes are zero
            if (nCarrier == 0)
                return readDWORD();

            // Implicit VR with 32-bit length
            return nCarrier;

        case AE:
        case AS:
        case AT:
        case CS:
        case DA:
        case DS:
        case DT:
        case FD:
        case FL:
        case IS:
        case LO:
        case LT:
        case PN:
        case SH:
        case SL:
        case SS:
        case SQ:
        case ST:
        case TM:
        case UI:
        case UL:
        case US:
        case QQ:
            return nCarrier;

        default:
            return (nVR << 16) + nCarrier;

    }
}

bool    TagReader::readNextTag(TagElement *pTagElem)
{
    DWORD   aTag    = 0;
    bool    bDone   = false;
    DWORD   nTemp   = readDWORD();

    if(bLittleEndian)
    {
        // Swap it !!
        BYTE *pA1 = (BYTE*)&nTemp;
        BYTE *pA2 = pA1+1;
        BYTE *pA3 = pA1+2;
        BYTE *pA4 = pA1+3;
        aTag = (*pA2 << 24 ) +
               (*pA1 << 16 ) +
               (*pA4 << 8  ) +
                *pA3;
    }else{
        aTag = nTemp;
    }

    if(aTag > 0)
    {
        char aSubTag[4] = {0};
        char *pRead = new char[4];
        readString(pRead,4);
        strncpy(aSubTag,pRead,4);
        delete pRead;

        WORD nVR = 0;
        WORD nCarrier = 0;
        memcpy(&nVR,aSubTag,2);
        memcpy(&nCarrier,&aSubTag[2],2);

        DWORD nLen = getLength(nVR,nCarrier);

        // added for some VR("SQ") using abnormal size.
        // Maybe SQ writes in Leadtools.
        // 0xFFFFFFFFFF means -1 in singed.
        if ( ( nLen > 0 ) && ( nLen != 0xFFFFFFFF ) )
        {
            pTagElem->id = aTag;

            memcpy(pTagElem->VRtype,&nVR,2);
            pTagElem->size = nLen;

            char *pRead = new char[nLen];
            memset(pRead,0,nLen);
            if(pRead)
            {
                readString(pRead,nLen);
                if(nLen > MAX_STATICBUFFER_LENGTH)
                {
                    pTagElem->dynamicbuffer = pRead;
                    pTagElem->alloced = TRUE;
                }else{
                    memset(pTagElem->staticbuffer,0,MAX_STATICBUFFER_LENGTH);
                    memcpy(pTagElem->staticbuffer,pRead,nLen);
                    pTagElem->alloced = FALSE;
                }
            }else
            {
                // it failed to read data !
                pTagElem->dynamicbuffer = NULL;
                pTagElem->alloced = FALSE;
                return false;
            }

            return true;
        }else{
            pTagElem->id = aTag;

            memcpy(pTagElem->VRtype,&nVR,2);
            pTagElem->size = nLen;
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

    while(bRepeat)
    {
        pTagElem = new TagElement;
        if(pTagElem)
        {
            memset(pTagElem,0,sizeof(TagElement));
            bRepeat = readNextTag(pTagElem);

            if(bRepeat)
            {
                TagElements.push_back(pTagElem);
            }

            DWORD fPos = fileStream.tellg();
            if(fPos < fileLength)
            {
                bRepeat = true;
            }
        }else
            bRepeat = false;
    }
}

bool TagReader::IsLoaded()
{
    return bFileLoaded;
}

TagElement* TagReader::GetTagElementByID(DWORD TagID)
{
    if (TagID == 0)
        return NULL;

    DWORD nCnt = 0;

    list<TagElement*>::iterator  it;

    for(it=TagElements.begin(); it!=TagElements.end(); advance(it,1))
    {
        TagElement *pE = *it;
        if(pE)
        {
            if(pE->id == TagID)
                return pE;
        }
    }

    return NULL;
}
