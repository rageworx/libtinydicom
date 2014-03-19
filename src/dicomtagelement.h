#pragma once
#ifndef _DICOMTAGELEMENT_H_
#define _DICOMTAGELEMENT_H_

#include <windows.h>
using namespace std;

namespace TinyDicom
{
    #define MAX_STATICBUFFER_LENGTH          64

    typedef struct _TagElement
    {
        unsigned long   id;
        char            VRtype[2];
        unsigned long   size;
        BOOL            alloced;
        char            staticbuffer[MAX_STATICBUFFER_LENGTH];
        void*           dynamicbuffer;
    }TagElement;
};

#endif // _DICOMTAGELEMENT_H_
