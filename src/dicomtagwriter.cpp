#ifdef  _WIN32
	#include <windows.h>
	#include <tchar.h>
#endif // of _WIN32

#include <cstring>
#include <cstdint>

#include "dicomtagconfig.h"
#include "dicomdictionary.h"
#include "dicomdecoder.h"
#include "dicomtagwriter.h"
#include "stdunicode.h"
#include "swap.h"

using namespace DicomImageViewer;
using namespace std;

TagWriter::TagWriter( wstring &fileName )
{
    createInstance( fileName );
}

TagWriter::TagWriter( const wchar_t* fileName )
{
    wstring convStr = fileName;
    createInstance(convStr);
}

TagWriter::TagWriter( const char* fileName )
{
    wstring convStr = convertM2W(fileName);
    createInstance(convStr);
}

TagWriter::~TagWriter()
{
    clearTags();
    fileStream.close();
}

void TagWriter::createInstance( wstring &fileName )
{
    // configure me --
    bLittleEndian = DATA_ARRANGE_LITTLE_ENDIAN;

    // file open --

#ifdef  __GNUC__
    fileStream.open(convertW2M((wchar_t*)fileName.c_str()),
                    ios::binary | ios::out | ios::app);
#else
    fileStream.open(fileName.c_str(), ios::binary | ios::out | ios::app);
#endif

    if(!fileStream)
        bFileCreated = false;
    else
        bFileCreated = true;
}

bool TagWriter::writeData( const uint8_t* pData, size_t nLen )
{
    if(!bFileCreated)
        return false;

    fileStream.write( (char*)pData, nLen );
    fileLength = fileStream.tellg();

    return true;
}

bool TagWriter::writeString(string &str)
{
    if(!bFileCreated)
        return false;

    fileStream.write((char*)str.c_str(),str.size());
    fileLength = fileStream.tellg();

    return true;
}

bool TagWriter::writeBYTE(BYTE aByte)
{
    if(!bFileCreated)
        return false;

    fileStream.write((char*)&aByte,1);
    fileLength = fileStream.tellg();

    return false;
}

bool TagWriter::writeWORD( uint16_t aWord)
{
    if(!bFileCreated)
        return false;

    fileStream.write((char*)&aWord,2);
    fileLength = fileStream.tellg();

    return false;
}

bool TagWriter::writeDWORD( uint32_t aDWord )
{
    if(!bFileCreated)
        return false;

    fileStream.write((char*)&aDWord,4);
    fileLength = fileStream.tellg();

    return false;
}

void TagWriter::clearTags()
{
    size_t nTagCount = TagElements.size();

    if( nTagCount == 0)
        return;

    // do iterator ...
    list<TagElement*>::iterator  it;

    for(it=TagElements.begin(); it!=TagElements.end(); advance(it,1))
    {
        TagElement* pTE = *it;

        if( pTE != NULL )
        {
            if(pTE->alloced && (pTE->size > 0))
            {
                if(pTE->dynamicbuffer)
                    delete[] (char*)pTE->dynamicbuffer;

                pTE->alloced = FALSE;
            }
        }
    }

    while(!TagElements.empty())
    {
        delete TagElements.back();
        TagElements.pop_back();
    }
}

bool TagWriter::writeNextTag(TagElement *pTagElem)
{
    if(!pTagElem)
        return false;

    // check ID
    uint32_t nID = pTagElem->id;
    if(bLittleEndian)
    {
        nID = SwapDWORD(pTagElem->id);
    }
    writeDWORD(nID);

    // check VR
    uint16_t nVR = 0;
    memcpy(&nVR,pTagElem->VRtype,2);

    // check size
    uint32_t nSz = pTagElem->size;

    uint16_t nCVR = nVR;
    if(bLittleEndian)
    {
        nCVR = SwapWORD(nVR);
    }

    switch(nCVR)
    {
        case OB:
        case OW:
        case SQ:
        case UN:
        case UT:
            {
                // Write DWORD
                writeWORD(nVR);
                writeWORD(0);
                writeDWORD(nSz);
            }
            break;

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
        default:
            {
                // Write WORD;
                writeWORD(nVR);
                writeWORD((uint16_t)nSz);
            }
            break;
    }

    if( nSz > 0 )
	{
        if(pTagElem->alloced)
        {
            writeData((const uint8_t*)pTagElem->dynamicbuffer, nSz);
        }
		else
		{
            writeData((const uint8_t*)pTagElem->staticbuffer, nSz);
        }
	}

    return true;
}

void TagWriter::writeTags()
{
    // check file position.
    uint32_t nFpos = fileStream.tellg();
    if(nFpos != 0)
    {
        fileStream.seekg(0,ios::beg);
        fileStream.sync();
    }

    // first, wirte empty 128 bytes...

    uint8_t *pEmpty = new uint8_t[ID_OFFSET];
    memset(pEmpty,0,ID_OFFSET);
    writeData(pEmpty, ID_OFFSET);
    delete[] pEmpty;

    // write magic
    uint8_t pMagic[4];
    memcpy(pMagic,DICM,4);
    writeData(pMagic, 4);

    // write elements ...
    list<TagElement*>::iterator  it;

    for(it=TagElements.begin(); it!=TagElements.end(); advance(it,1))
    {
        TagElement* pTE = *it;
        writeNextTag(*it);
    }

}

bool TagWriter::IsCreated()
{
    return true;
}

bool TagWriter::Write()
{
    writeTags();

    return true;
}

TagElement* TagWriter::FindTagElement( uint32_t TagID)
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

TagElement* TagWriter::GetTagElement( uint32_t nIndex)
{
    if (nIndex > TagElements.size())
        return NULL;

    uint32_t nCnt = 0;

    list<TagElement*>::iterator  it;

    for(it=TagElements.begin(); it!=TagElements.end(); advance(it,1))
    {
        if(nCnt == nIndex)
        {
            return *it;
        }
        nCnt++;
    }

    return NULL;
}
