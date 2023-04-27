#ifndef _DICOMDICTIONARY_H_
#define _DICOMDICTIONARY_H_

namespace DicomImageViewer
{
    #define GET_GROUP_ID( _dwid_ )      (uint16_t)(( _dwid_ & 0xFFFF0000 ) >> 16 )
    #define GET_ELEMENT_ID( _dwid_ )    (uint16_t)( _dwid_ & 0x0000FFFF )
    #define MAKE_ID( _group_, _elem_ )  (uint32_t)(( _group_ << 16 ) | ( _elem_ & 0x0000FFFF ))
    #define VERSION_MAJOR( _wd_ )       (uint8_t)(( _wd_ & 0xFF00 ) >> 8 )
    #define VERSION_MINOR( _wd_ )       (uint8_t)( _wd & 0x00FF )

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
        uint32_t    id;
        const char* vr;
        const char* mean;
        int32_t     state;
    }TDicomDictionary;

    class DicomDictionary
    {
        public:
            static uint16_t     GetVersion();
            static const char*  GetLastUpdateFlag();
            static int32_t      FindKeyIndex(const uint32_t id);
            static int32_t      FindKeyIndexFrom(const uint32_t id, uint32_t idx);
            static const char*  GetMean(const uint32_t id);
            static uint16_t     GetVR(const uint32_t id, bool* sameavailed = NULL );
            static void         GetVR(const uint32_t id, void *pVR, bool* sameavailed = NULL );
            static int32_t      GetElemState( const uint32_t id );
            static int32_t      GetVRarray( const uint32_t id, uint16_t**pVRarray );
    };
}

#endif // _DICOMDICTIONARY_H_
