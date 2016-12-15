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

using namespace std;

namespace DicomImageViewer
{
    class TagWriter: public TagStore
    {
        private:
            fstream     fileStream;
            DWORD       fileLength;     // up to 3.2GB (32bit)
            bool        bFileCreated;
            bool        bLittleEndian;

            // private function methods -----------------------------------
            void        createInstance( wstring &fileName );

            bool        writeData( const char* pData, DWORD nLen );
            bool        writeString(string &str);
            bool        writeBYTE(BYTE aByte);
            bool        writeWORD(WORD aWord);
            bool        writeDWORD(DWORD aDWord);

            void        clearTags();
            bool        writeNextTag(TagElement *pTagElem);
            void        writeTags();

        public:

            // public function methods ------------------------------------
            bool        IsCreated();
            bool        Write();

            TagElement* FindTagElement(DWORD TagID);
            TagElement* GetTagElement(DWORD nIndex);

            // constructor
            TagWriter( wstring &fileName );
            TagWriter( const wchar_t *fileName );
            TagWriter( const char *fileName );

            // destructor
            ~TagWriter();
    };
}

#endif // _DICOMTAGWRITER_H_
