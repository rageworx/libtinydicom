/*******************************************************************************
* STL , DicomImageViewer::DicomTagStore (on Memory)
* - base class for DICOM tag read/write
*                                                    last revision : 2011-09-05
* =============================================================================
*
* * Programmed by Raphael Kim (rageworx@gmail.com)
* * Unicode Model.
*
*******************************************************************************/
#pragma once
#ifndef _DICOMTAGSTORE_H_
#define _DICOMTAGSTORE_H_

#include <fstream>
#include <list>

#include "dicomtagelement.h"

// fstream requires "std" namespace.
using namespace std;

namespace TinyDicom
{
    class TagStore
    {
        public:
            TagStore();
            virtual ~TagStore();

        public:
            void            ClearTags();

            unsigned long   GetTagCount();
            TagElement*     FindTagElement(unsigned long TagID);
            TagElement*     GetTagElement(unsigned long nIndex);

            unsigned long   AddTagElement(unsigned long TagID,unsigned short wVR, char* data, unsigned long size);
            int             AddTagElement(TagElement *pTag);

        private:
            // private function methods -----------------------------------
            bool            IsCreated();

        protected:
            bool                bLittleEndian;
            list<TagElement*>   TagElements;

    };
};

#endif // _DICOMTAGSTORE_H_
