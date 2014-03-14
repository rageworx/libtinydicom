#ifdef _WIN32
    #include <tchar.h>
#else
    #include "tchar.h"
#endif

#include "dicomtagconfig.h"
#include "dicomdictionary.h"
#include "dicomdecoder.h"
#include "dicomtagreader.h"
#include "stdunicode.h"

using namespace TinyDicom;

TagReader::TagReader(wstring &fileName)
{
    createInstance(fileName);
}

TagReader::TagReader(wchar_t *fileName)
{
    wstring fn = fileName;
    createInstance(fn);
}

TagReader::TagReader(char *fileName)
{
    wstring fn = convertM2W(fileName);
    createInstance(fn);
}

TagReader::~TagReader()
{
    ClearTags();
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
    char *pFn    = convertW2M(pWC);
    fileStream.open(pFn, ios::binary | ios::in | ios::app );
#else
    fileStream.open(fileName.c_str(), ios::binary | ios::in | ios::app );
#endif

    if(!fileStream)
    {
        bFileLoaded = false;
        fileStream.close();
        return;
    }

    // get file size .
    fileStream.seekg(0,ios::end);
    fileLength = fileStream.tellg();
    fileStream.seekg(0,ios::beg);

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

int     TagReader::readString(char *pBuf, unsigned long nLength)
{
    unsigned long   nCurPos = fileStream.tellg();

    if(nCurPos + nLength > fileLength)
        return 0;

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
    unsigned long   nCurPos = fileStream.tellg();

    if(nCurPos + 1 < fileLength)
        fileStream.read((char*)&aByte,1);

    return aByte;
}

unsigned short TagReader::readWORD()
{
    unsigned short  aWORD   = 0;
    unsigned long   nCurPos = fileStream.tellg();

    if(nCurPos + 2 < fileLength)
        fileStream.read((char*)&aWORD,2);

    return aWORD;
}

unsigned long TagReader::readDWORD()
{
    unsigned long   aDWORD  = 0;
    unsigned long   nCurPos = fileStream.tellg();

    if(nCurPos + 4 < fileLength)
        fileStream.read((char*)&aDWORD,4);

    return aDWORD;
}

unsigned long TagReader::getLength(unsigned short nVR,unsigned short nCarrier)
{
    unsigned short cVR = nVR;

    if(bLittleEndian)
    {
        // Swap it !!
        unsigned char *pA1 = (unsigned char*)&nVR;
        unsigned char *pA2 = pA1+1;
        cVR = (*pA1 << 8 ) + *pA2;
    }


    switch(cVR)
    {
        case OB:
        case OW:
        case SQ:
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
    unsigned long   aTag    = 0;
    unsigned long   nTemp   = readDWORD();

    if(bLittleEndian)
    {
        // Swap it !!
        unsigned char *pA1 = (unsigned char*)&nTemp;
        unsigned char *pA2 = pA1+1;
        unsigned char *pA3 = pA1+2;
        unsigned char *pA4 = pA1+3;
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

        unsigned short nVR = 0;
        unsigned short nCarrier = 0;
        memcpy(&nVR,aSubTag,2);
        memcpy(&nCarrier,&aSubTag[2],2);

        unsigned long nLen = getLength(nVR,nCarrier);

        if(nLen>0)
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
                    pTagElem->dynamicbuffer = (void*)pRead;
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

void    TagReader::readTags()
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

            unsigned long fPos = fileStream.tellg();
            if(fPos < fileLength)
            {
                bRepeat = true;
            }
        }else
            bRepeat = false;
    }
}

///////////////////////////////////////////////////////////////////

bool        TagReader::IsLoaded()
{
    return bFileLoaded;
}

TagElement* TagReader::GetTagElementByID(unsigned long TagID)
{
    if (TagID == 0)
        return NULL;

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
