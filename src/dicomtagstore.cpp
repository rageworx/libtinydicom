#include <windows.h>
#include <tchar.h>

#include "dicomtagconfig.h"
#include "dicomdictionary.h"
#include "dicomdecoder.h"
#include "dicomtagstore.h"
#include "stdunicode.h"
#include "swap.h"

#include <algorithm>

using namespace TinyDicom;

////////////////////////////////////////////////////////////////////////////////

bool tagSortProc( const TagElement* first, const TagElement* second );

////////////////////////////////////////////////////////////////////////////////

TagStore::TagStore()
 : bLittleEndian(DATA_ARRANGE_LITTLE_ENDIAN)
{
    // configure me --
}

TagStore::~TagStore()
{
    ClearTags();
}

void TagStore::ClearTags()
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
            if(pTE->size && pTE->dynamicbuffer && pTE->alloced)
            {
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

bool TagStore::isCreated()
{
    return true;
}

unsigned long TagStore::GetTagCount()
{
    return TagElements.size();
}

TagElement* TagStore::FindTagElement(unsigned long TagID)
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

unsigned long TagStore::AddTagElement(unsigned long TagID, unsigned short wVR, const char* data, unsigned long size)
{
    TagElement *pNewTag = new TagElement;

    // erase it as NULL, first.
    memset(pNewTag,0,sizeof(TagElement));

    pNewTag->id = TagID;
    memcpy(pNewTag->VRtype, &wVR, 2);

    int i;
    unsigned short ush;
    unsigned int ulo;

    unsigned short nCVR = wVR;
    if ( bLittleEndian )
    {
        nCVR = SwapWORD(wVR);
    }

    char* dataBuffer = NULL;

    switch ( nCVR )
    {
        case UI:
        case IS:
        case DS:
        case CS:
            dataBuffer = new char[size+1];
            memset(dataBuffer, 0, size+1);
            memcpy(dataBuffer, data, size);
            if (( size % 2 ) != 0 )
            {
                dataBuffer[size] = '\0';
                size++;
            }
            break;

        case UL:
            dataBuffer = new char[sizeof(unsigned int)];

            sscanf(data, "%d", &i);
            size = sizeof(unsigned int);
            ulo = (unsigned int)i;
            memcpy(dataBuffer, &ulo, sizeof(unsigned int));
            break;

        case US:
            dataBuffer = new char[sizeof(unsigned short)];
            sscanf(data, "%d", &i);
            size = sizeof(unsigned short);
            ush = (unsigned short)i;
            memcpy(dataBuffer, &ush, sizeof(unsigned short));
            break;

        default:
            dataBuffer = new char[size];
            memcpy(dataBuffer, data, size);
            break;
    }

    pNewTag->size = size;
    if ( pNewTag->size )
    {
        if( pNewTag->size > MAX_STATICBUFFER_LENGTH )
        {
            pNewTag->dynamicbuffer = new char[pNewTag->size];
            pNewTag->alloced = true;
            memcpy(pNewTag->dynamicbuffer, dataBuffer, pNewTag->size);
        }
        else
        {
            memset(pNewTag->staticbuffer, 0, MAX_STATICBUFFER_LENGTH);
            memcpy(pNewTag->staticbuffer, dataBuffer, pNewTag->size);
			pNewTag->alloced = false;
        }
    }

    if ( dataBuffer == NULL )
    {
        delete[] dataBuffer;
    }

    TagElements.push_back(pNewTag);

    return TagElements.size();
}

int  TagStore::AddTagElement(TagElement *pTag)
{
    if(!pTag)
        return -1;

    // check same elements by ID.
    if(FindTagElement(pTag->id))
        return -2;

    TagElements.push_back(pTag);

    return TagElements.size();
}

void TagStore::Sort()
{
    TagElements.sort( tagSortProc );
}

TagElement* TagStore::GetTagElement(unsigned long nIndex)
{
    if (nIndex > TagElements.size())
        return NULL;

    unsigned long nCnt = 0;

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

////////////////////////////////////////////////////////////////////////////////

bool tagSortProc( const TagElement* first, const TagElement* second )
{
    if ( second->id > first->id )
        return true;

    return false;
}
