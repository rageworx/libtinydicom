/*******************************************************************************
* STL , DicomImageViewer::DicomTagReader
*                                                    last revision : 2017-02-08
* =============================================================================
*
* * (C)Copyright 2011-2014 Raphael Kim (rage.kim)
* *
* * Unicode Model.
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
            DWORD       fileLength;     // DICOM limited to 3.2GB.
            bool        bFileLoaded;
            bool        bLittleEndian;

            // private function methods -----------------------------------
            void        createInstance( wstring &fileName );
            void        createInstance( string &fileName );

            int         readString(char *pBuf, DWORD nLength);
            DWORD       seekToNext();
            BYTE        readBYTE();
            WORD        readWORD();
            DWORD       readDWORD();

            DWORD       getLength(WORD* nVR,WORD nCarrier);
            bool        readNextTag(TagElement *pTagElem);
            void        readTags();

        public:

            // public function methods ------------------------------------
            bool        IsLoaded();

            TagElement* GetTagElementByID(DWORD TagID);

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
