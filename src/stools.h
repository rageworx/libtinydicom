#ifndef _STOOLS_H_
#define _STOOLS_H_

#ifdef _cplusplus
extern "C" {
#endif

#ifndef BYTE
    #define BYTE    unsigned char
#endif /// of BYTE

#ifndef WORD
    #define WORD    unsigned short
#endif /// of WORD

#ifndef DWORD
    #define DWORD   unsigned long
#endif /// of DWORD

char*       ConvertFromUnicode( const wchar_t *src );
wchar_t*    ConvertFromMBCS( const char *src );

WORD        SwapWORD(WORD nWord);
DWORD       SwapDWORD(DWORD nDWord);

#ifdef _cplusplus
};
#endif

#endif // of _STOOLS_H_
