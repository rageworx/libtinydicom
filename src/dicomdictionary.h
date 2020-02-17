#ifndef _DICOMDICTIONARY_H_
#define _DICOMDICTIONARY_H_

namespace DicomImageViewer
{
    #define GET_GROUP_ID( _dwid_ )      (WORD)(( _dwid_ & 0xFFFF0000 ) >> 16 )
    #define GET_ELEMENT_ID( _dwid_ )    (WORD)( _dwid_ & 0x0000FFFF )
    #define MAKE_ID( _group_, _elem_ )  (DWORD)(( _group_ << 16 ) | ( _elem_ & 0x0000FFFF ))
    #define VERSION_MAJOR( _wd_ )       (BYTE)(( _wd_ & 0xFF00 ) >> 8 )
    #define VERSION_MINOR( _wd_ )       (BYTE)( _wd & 0x00FF )

    /*
    "ELEM_STATE_RETIRED" is used to indicate that the corresponding Data Element,
    SOP Class, or Transfer Syntax has been retired.
    Retired items are shown italicized.
    When the name of a retired Data Element has been reused,
    the retired element has commented, or "ELEM_STATE_TRIAL " in
    the cases in which the Data Element was used in a Draft For Trial Implementation
    but not standardized.
    */
    enum EDicomDictionaryState
    {
        ELEM_STATE_UNKNWON  = 0x00000000,
        ELEM_STATE_NORMAL   = 0x00000001,
        ELEM_STATE_TRIAL    = 0x00000002,
        ELEM_STATE_DICOS    = 0x00000004,
        ELEM_STATE_DICONDE  = 0x00000008,
        ELEM_STATE_RETIRED  = 0x00000010,
    };

    typedef struct _TDicomDictionary
    {
        DWORD       id;
        const char* vr;
        const char* mean;
        int         state;
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
            static int GetElemState( const DWORD id );
            static int GetVRarray( const DWORD id, WORD**pVRarray );
    };
}

#endif // _DICOMDICTIONARY_H_
