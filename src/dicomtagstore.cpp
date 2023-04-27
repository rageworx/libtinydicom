#ifdef _WIN32
	#include <windows.h>
	#include <tchar.h>
#endif /// of _WIN32

#include <cstring>
#include <cstdint>

#include "dicomtagconfig.h"
#include "dicomdictionary.h"
#include "dicomdecoder.h"
#include "dicomtagstore.h"
#include "stdunicode.h"

using namespace DicomImageViewer;

////////////////////////////////////////////////////////////////////////////////

bool tagSortProc( const TagElement* first, const TagElement* second );

////////////////////////////////////////////////////////////////////////////////

TagStore::TagStore()
: bLittleEndian( (bool)DATA_ARRANGE_LITTLE_ENDIAN )
{
    // configure me --
}

TagStore::~TagStore()
{
    clearTags();
}

void TagStore::clearTags()
{
    size_t nTagCount = TagElements.size();

    if( nTagCount == 0 )
        return;

    // do iterator ...
    list<TagElement*>::iterator  it;

    for( it=TagElements.begin(); it!=TagElements.end(); advance(it,1) )
    {
        TagElement* pTE = *it;

        if( pTE != NULL )
        {
            if( pTE->size && pTE->dynamicbuffer && pTE->alloced )
            {
                delete[] pTE->dynamicbuffer;
                pTE->alloced = false;
            }
        }
    }

    while( !TagElements.empty() )
    {
        delete TagElements.back();
        TagElements.pop_back();
    }
}

bool TagStore::IsCreated()
{
    return true;
}

size_t TagStore::GetTagCount()
{
    return TagElements.size();
}

TagElement* TagStore::FindTagElement( uint32_t TagID )
{
    if (TagID == 0)
        return NULL;

    list<TagElement*>::iterator  it;

    for(it=TagElements.begin(); it!=TagElements.end(); advance(it,1))
    {
        TagElement *pE = *it;

        if( pE != NULL )
        {
            if( pE->id == TagID )
                return pE;
        }
    }

    return NULL;
}

size_t TagStore::AddTagElement( uint32_t TagID, uint16_t wVR, const uint8_t* data, size_t size )
{
    TagElement *pNewTag = new TagElement;

    if ( pNewTag == NULL )
        return 0;

    // erase it as NULL, first.
    memset( pNewTag, 0, sizeof(TagElement) );

    // http://dicom.nema.org/dicom/2013/output/chtml/part05/sect_6.2.html
    // 2 bytes align required at case of OB with NULL.
    pNewTag->id    = TagID;
    memcpy( pNewTag->VRtype, &wVR, 2 );
    uint32_t rsize = (uint32_t)size;
    pNewTag->size  = rsize;

    // VR-"OB" need size padding.
    if ( *(uint16_t*)pNewTag->VRtype == OB )
    {
        if ( rsize%2 > 0 )
        {
            rsize++;
        }
    }

    if( rsize > 0 )
    {
        if( size > MAX_STATICBUFFER_LENGTH )
        {
            pNewTag->dynamicbuffer = new uint8_t[size];
            if ( pNewTag->dynamicbuffer != NULL )
            {
                pNewTag->alloced = true;
                memset( pNewTag->dynamicbuffer,0,size);
                memcpy( pNewTag->dynamicbuffer,data,rsize);
            }
            else
            {
                // memory allocation failure meaning failure.
                pNewTag->alloced = false;
                pNewTag->dynamicbuffer = NULL;
                delete pNewTag;
                return 0;
            }
        }else{
            memset(pNewTag->staticbuffer,0,MAX_STATICBUFFER_LENGTH);
            memcpy(pNewTag->staticbuffer,data,rsize);
			pNewTag->alloced = false;
        }
    }

    TagElements.push_back(pNewTag);

    return TagElements.size();
}

size_t TagStore::AddTagElement( TagElement *pTag )
{
    if(!pTag)
        return 0;

    // check same elements by ID.
    if( FindTagElement( pTag->id ) )
        return 0;

    TagElements.push_back(pTag);

    return TagElements.size();
}

TagElement* TagStore::GetTagElement( size_t nIndex )
{
    if ( nIndex >= TagElements.size() )
        return NULL;

    list<TagElement*>::iterator  it = TagElements.begin();

    advance( it, nIndex );

    return *it;
}

void TagStore::Sort()
{
    TagElements.sort( tagSortProc );
}

////////////////////////////////////////////////////////////////////////////////

bool tagSortProc( const TagElement* first, const TagElement* second )
{
    if ( second->id > first->id )
        return true;

    return false;
}
