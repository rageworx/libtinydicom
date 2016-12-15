#include "swap.h"

unsigned short DicomImageViewer::SwapWORD(unsigned short nWord)
{
    unsigned short nRet = 0;

    unsigned char *pA1 = (unsigned char*)&nWord;
    unsigned char *pA2 = pA1+1;
    nRet = (*pA1 << 8 ) + *pA2;

    return nRet;
}

unsigned long DicomImageViewer::SwapDWORD(unsigned long nDWord)
{
    unsigned long nRet = 0;

    unsigned char *pA1 = (unsigned char*)&nDWord;
    unsigned char *pA2 = pA1+1;
    unsigned char *pA3 = pA1+2;
    unsigned char *pA4 = pA1+3;
    nRet = (*pA2 << 24 ) +
           (*pA1 << 16 ) +
           (*pA4 << 8  ) +
            *pA3;

    return nRet;
}
