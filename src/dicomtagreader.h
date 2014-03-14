/*******************************************************************************
* STL , DicomImageViewer::DicomTagReader
*                                                    last revision : 2011-09-05
* =============================================================================
*
* * Programmed by Raphael Kim (rageworx@gmail.com)
* * Unicode Model.
*
*******************************************************************************/
#pragma once
#ifndef _DICOMTAGREADER_H_
#define _DICOMTAGREADER_H_

#include <fstream>
#include <list>

#include "dicomtagelement.h"
#include "dicomtagstore.h"

using namespace std;

namespace TinyDicom
{
    class TagReader: public TagStore
    {
        public:
            // public function methods ------------------------------------
            bool            IsLoaded();

            TagElement*     GetTagElementByID(unsigned long TagID);

            // constructor
            TagReader(wstring &fileName);
            TagReader(wchar_t *fileName);
            TagReader(char *fileName);

            // destructor
            ~TagReader();

        private:
            fstream             fileStream;
            unsigned            fileLength;     // DICOM limited to 3.2GB.
            bool                bFileLoaded;
            bool                bLittleEndian;

        private:
            // private function methods -----------------------------------
            void            createInstance(wstring &fileName);
            int             readString(char *pBuf, unsigned long nLength);
            BYTE            readBYTE();
            unsigned short  readWORD();
            unsigned long   readDWORD();

            unsigned long   getLength(unsigned short nVR,unsigned short nCarrier);
            bool            readNextTag(TagElement *pTagElem);
            void            readTags();

    };
};

#endif // _DICOMTAGREADER_H_
