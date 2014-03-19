#pragma once
#ifndef _DICOMDICTIONARY_H_
#define _DICOMDICTIONARY_H_

namespace TinyDicom
{
    #define GET_GROUP_ID( _dwid_ ) (unsigned short)((_dwid_ & 0xFFFF0000) >> 16)
    #define GET_ELEMENT_ID( _dwid_ ) (unsigned short)(_dwid & 0x0000FFFF)

    typedef struct _TDicomDictionary
    {
        unsigned long   id;
        char    mean[80];
    }TDicomDictionary;

    class DicomDictionary
    {
        public:
            static unsigned short GetVersion();
            static char* GetLastUpdateFlag();
            static int FindKeyIndex(const unsigned long id);
            static char* GetMean(const unsigned long id);
            static unsigned short GetVR(const unsigned long id);
            static void GetVR(const unsigned long id, void *pVR);
    };
};


#endif // _DICOMDICTIONARY_H_
