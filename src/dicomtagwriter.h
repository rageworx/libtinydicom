/*******************************************************************************
* STL , DicomImageViewer::DicomTagWriter
*                                                    last revision : 2011-09-05
* =============================================================================
*
* * (C)Copyright 2011-2014 Raphael Kim (rage.kim)
* * Unicode Model.
*
*******************************************************************************/
#pragma once
#ifndef _DICOMTAGWRITER_H_
#define _DICOMTAGWRITER_H_

#include <fstream>
#include <list>

#include "dicomtagelement.h"
#include "dicomtagstore.h"

namespace DicomImageViewer
{
    class TagWriter: public TagStore
    {
        private:
            fstream     fileStream;
            size_t      fileLength;
            bool        bFileCreated;
            bool        bLittleEndian;

            // private function methods -----------------------------------
            void        createInstance( std::wstring &fileName );

            bool        writeData( const uint8_t* pData, size_t nLen );
            bool        writeString( std::string &str);
            bool        writeBYTE( uint8_t aByte );
            bool        writeWORD( uint16_t aWord );
            bool        writeDWORD( uint32_t aDWord );

            void        clearTags();
            bool        writeNextTag( TagElement *pTagElem );
            void        writeTags();

        public:

            // public function methods ------------------------------------
            bool        IsCreated();
            bool        Write();

            TagElement* FindTagElement( uint32_t TagID );
            TagElement* GetTagElement( uint32_t nIndex );

            // constructor
            TagWriter( std::wstring &fileName );
            TagWriter( const wchar_t *fileName );
            TagWriter( const char *fileName );

            // destructor
            ~TagWriter();
    };
}

#endif // _DICOMTAGWRITER_H_
