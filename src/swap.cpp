#include <cstdint>
#include "swap.h"

uint16_t DicomImageViewer::SwapWORD(uint16_t nWord)
{
    uint16_t nRet = 0;

    uint8_t* pA1 = (unsigned char*)&nWord;
    uint8_t* pA2 = pA1+1;
    nRet = (*pA1 << 8 ) + *pA2;

    return nRet;
}

uint32_t DicomImageViewer::SwapDWORD(uint32_t nDWord)
{
    uint32_t nRet = 0;

    uint8_t* pA1 = (unsigned char*)&nDWord;
    uint8_t* pA2 = pA1+1;
    uint8_t* pA3 = pA1+2;
    uint8_t* pA4 = pA1+3;
    nRet = (*pA2 << 24 ) +
           (*pA1 << 16 ) +
           (*pA4 << 8  ) +
            *pA3;

    return nRet;
}
