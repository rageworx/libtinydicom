#ifndef _DICOMTAGELEMENT_H_
#define _DICOMTAGELEMENT_H_

using namespace std;

namespace DicomImageViewer
{
    #define MAX_STATICBUFFER_LENGTH          64

    typedef struct _TagElement
    {
        uint32_t   id;
        uint8_t    VRtype[2];
        uint32_t   size;
        bool       alloced;
        uint8_t    staticbuffer[MAX_STATICBUFFER_LENGTH];
        uint8_t*   dynamicbuffer;
    }TagElement;
}

#endif // _DICOMTAGELEMENT_H_
