/*******************************************************************************
* STL , DicomImageViewer::DicomTagReader
*                                                    last revision : 2023-04-27
* =============================================================================
*
* * (C)Copyright 2011-2014 Raphael Kim (rage.kim)
* *
* * Unicode Model.
*
* Update in 2023-04-27
*  : Using standard integer types.
*
* Update in 2017-02-08
*  : Enhanced to handle bad VR, UUID DICOM file.
*
*
*******************************************************************************/
#pragma once
#ifndef _DICOMTAGREADER_H_
#define _DICOMTAGREADER_H_

#include <fstream>
#include <list>

#include "dicomtagconfig.h"
#include "dicomtagelement.h"
#include "dicomtagstore.h"

using namespace std;

namespace DicomImageViewer
{
    class TagReader: public TagStore
    {
        private:
            fstream     fileStream;
            size_t      fileLength;
            bool        bFileLoaded;
            bool        bLittleEndian;

            // private function methods -----------------------------------
            void        createInstance( wstring &fileName );
            void        createInstance( string &fileName );

            size_t      readString(char *pBuf, size_t nLength);
            size_t      seekToNext();
            uint8_t     readBYTE();
            uint16_t    readWORD();
            uint32_t    readDWORD();

            size_t      getLength(uint16_t* nVR, uint16_t nCarrier);
            bool        readNextTag(TagElement *pTagElem);
            void        readTags();

        public:

            // public function methods ------------------------------------
            bool        IsLoaded();

            TagElement* GetTagElementByID(uint32_t TagID);

            // constructor
            TagReader( wstring &fileName );
            TagReader( string &fileName );
            TagReader( const wchar_t* fileName );
            TagReader( const char* fileName );

            // destructor
            ~TagReader();
    };
}

#endif // _DICOMTAGREADER_H_
