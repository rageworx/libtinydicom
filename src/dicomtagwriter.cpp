#ifdef  _WIN32
	#include <windows.h>
	#include <tchar.h>
#endif // of _WIN32

#include "dicomtagconfig.h"
#include "dicomdictionary.h"
#include "dicomdecoder.h"
#include "dicomtagwriter.h"
#include "stdunicode.h"
#include "swap.h"

using namespace DicomImageViewer;

TagWriter::TagWriter( wstring &fileName )
{
    createInstance( fileName );
}

TagWriter::TagWriter( const wchar_t* fileName )
{
    wstring convStr = fileName;
    createInstance(convStr);
}

TagWriter::TagWriter( const char* fileName)
{
    wstring convStr = convertM2W(fileName);
    createInstance(convStr);
}

TagWriter::~TagWriter()
{
    clearTags();
    fileStream.close();
}

void TagWriter::createInstance(wstring &fileName)
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

bool TagWriter::writeData( const char* pData, DWORD nLen )
{
    if(!bFileCreated)
        return false;

    fileStream.write(pData,nLen);
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

bool TagWriter::writeWORD(WORD aWord)
{
    if(!bFileCreated)
        return false;

    fileStream.write((char*)&aWord,2);
    fileLength = fileStream.tellg();

    return false;
}

bool TagWriter::writeDWORD(DWORD aDWord)
{
    if(!bFileCreated)
        return false;

    fileStream.write((char*)&aDWord,4);
    fileLength = fileStream.tellg();

    return false;
}

void TagWriter::clearTags()
{
    int nTagCount = TagElements.size();
    if( nTagCount == 0)
        return;

    // do iterator ...
    list<TagElement*>::iterator  it;

    for(it=TagElements.begin(); it!=TagElements.end(); advance(it,1))
    {
        TagElement* pTE = *it;

        if(pTE)
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
    DWORD nID = pTagElem->id;
    if(bLittleEndian)
    {
        nID = SwapDWORD(pTagElem->id);
    }
    writeDWORD(nID);

    // check VR
    WORD  nVR = 0;
    memcpy(&nVR,pTagElem->VRtype,2);

    // check size
    DWORD nSz = pTagElem->size;

    WORD nCVR = nVR;
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
                writeWORD((WORD)nSz);
            }
            break;
    }

    if( nSz > 0 )
	{
        if(pTagElem->alloced)
        {
            writeData((char*)pTagElem->dynamicbuffer, nSz);
        }
		else 
		{
            writeData((char*)pTagElem->staticbuffer, nSz);
        }
	}
	
    return true;
}

void TagWriter::writeTags()
{
    // check file position.
    DWORD nFpos = fileStream.tellg();
    if(nFpos != 0)
    {
        fileStream.seekg(0,ios::beg);
        fileStream.sync();
    }

    // first, wirte empty 128 bytes...

    char *pEmpty = new char[ID_OFFSET];
    memset(pEmpty,0,ID_OFFSET);
    writeData(pEmpty, ID_OFFSET);
    delete[] pEmpty;

    // write magic
    char pMagic[4];
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

TagElement* TagWriter::FindTagElement(DWORD TagID)
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

TagElement* TagWriter::GetTagElement(DWORD nIndex)
{
    if (nIndex > TagElements.size())
        return NULL;

    DWORD nCnt = 0;

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
