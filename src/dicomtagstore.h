/*******************************************************************************
* STL , DicomImageViewer::DicomTagStore (on Memory)
* - base class for DICOM tag read/write
*                                                    last revision : 2011-09-05
* =============================================================================
*
* * (C)Copyright 2011-2014 Raphael Kim (rage.kim)
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

namespace DicomImageViewer
{
    class TagStore
    {
        protected:
            bool                bLittleEndian;
            list<TagElement*>   TagElements;

            // private function methods -----------------------------------
            bool        IsCreated();

        public:

            // public function methods ------------------------------------
            void        clearTags();

            DWORD       GetTagCount();
            TagElement* FindTagElement(DWORD TagID);
            TagElement* GetTagElement(DWORD nIndex);

            DWORD       AddTagElement(DWORD TagID,WORD wVR, char* data, DWORD size);
            int         AddTagElement(TagElement *pTag);

            void        Sort();

            // constructor
            TagStore();

            // destructor
            ~TagStore();
    };
};

#endif // _DICOMTAGSTORE_H_
