/*******************************************************************************
* STL , DicomImageViewer::DicomTagStore (on Memory)
* - base class for DICOM tag read/write
*                                                    last revision : 2023-04-27
* =============================================================================
*
* * (C)Copyright 2011-2023 Raphael Kim (rageworx@gmail.com)
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

            size_t      GetTagCount();
            TagElement* FindTagElement(uint32_t TagID);
            TagElement* GetTagElement(size_t nIndex);

            size_t      AddTagElement(uint32_t TagID, uint16_t wVR, const uint8_t* data, size_t size);
            size_t      AddTagElement(TagElement *pTag);

            void        Sort();

            // constructor
            TagStore();

            // destructor
            ~TagStore();
    };
}

#endif // _DICOMTAGSTORE_H_
