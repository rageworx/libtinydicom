#ifndef _DICOMDICTIONARY_H_
#define _DICOMDICTIONARY_H_

namespace DicomImageViewer
{
    #define GET_GROUP_ID( _dwid_ ) (WORD)((_dwid_ & 0xFFFF0000) >> 16)
    #define GET_ELEMENT_ID( _dwid_ ) (WORD)(_dwid & 0x0000FFFF)

    typedef struct _TDicomDictionary
    {
        DWORD   id;
        char    vr[3];
        char    mean[80];
    }TDicomDictionary;

    class DicomDictionary
    {
        public:
            static WORD GetVersion();
            static const char* GetLastUpdateFlag();
            static int FindKeyIndex(const DWORD id);
            static char* GetMean(const DWORD id);
            static WORD GetVR(const DWORD id);
            static void GetVR(const DWORD id, void *pVR);
    };
};


#endif // _DICOMDICTIONARY_H_
