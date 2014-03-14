/*******************************************************************************
* STL , DicomImageViewer::DicomTagWriter
*                                                    last revision : 2011-09-05
* =============================================================================
*
* * Programmed by Raphael Kim (rageworx@gmail.com)
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

namespace TinyDicom
{
    class TagWriter: public TagStore
    {
        private:
            fstream     fileStream;
            unsigned    fileLength;     // up to 3.2GB (32bit)
            bool        bFileCreated;
            bool        bLittleEndian;

            // private function methods -----------------------------------
            void        createInstance(wstring &fileName);

            bool        writeData(char* pData, unsigned long nLen);
            bool        writeString(string &str);
            bool        writeBYTE(unsigned char aByte);
            bool        writeWORD(unsigned short aWord);
            bool        writeDWORD(unsigned long aDWrod);

            void        clearTags();
            bool        writeNextTag(TagElement *pTagElem);
            void        writeTags();

        public:

            // public function methods ------------------------------------
            bool        IsCreated();
            bool        Write();

            TagElement* FindTagElement(unsigned long TagID);
            TagElement* GetTagElement(unsigned long nIndex);

            // constructor
            TagWriter(wstring &fileName);
            TagWriter(wchar_t *fileName);
            TagWriter(char *fileName);

            // destructor
            ~TagWriter();
    };
};

#endif // _DICOMTAGWRITER_H_
