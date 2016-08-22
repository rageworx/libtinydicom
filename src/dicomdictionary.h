#ifndef _DICOMDICTIONARY_H_
#define _DICOMDICTIONARY_H_

namespace DicomImageViewer
{
    #define GET_GROUP_ID( _dwid_ )      (WORD)((_dwid_ & 0xFFFF0000) >> 16)
    #define GET_ELEMENT_ID( _dwid_ )    (WORD)(_dwid_ & 0x0000FFFF)
    #define VERSION_MAJORT( _wd_ )      (BYTE)((_wd_ & 0xFF00 ) >> 8)
    #define VERSION_MINOR( _wd_ )       (BYTE)( _wd & 0x00FF )

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
            static int FindKeyIndexFrom(const DWORD id, int idx);
            static const char* GetMean(const DWORD id);
            static WORD GetVR(const DWORD id, bool* sameavailed = NULL );
            static void GetVR(const DWORD id, void *pVR, bool* sameavailed = NULL );
            static int GetVRarray( const DWORD id, WORD**pVRarray );
    };
};


#endif // _DICOMDICTIONARY_H_
