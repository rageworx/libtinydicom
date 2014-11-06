#ifdef _WIN32
    #include <windows.h>
#else
    #include <cstdio>
    #include <cstdlib>
    #include <cstring>
    #include <locale>
#endif

#include "stools.h"

char *ConvertFromUnicode( const wchar_t *src)
{
    static char* dest = NULL;

    if ( ( dest != NULL ) || ( src == NULL ) )
    {
        delete[] dest;
        dest = NULL;
    }

    if ( src == NULL )
    {
        return NULL;
    }

#ifdef _WIN32
    int len = WideCharToMultiByte( CP_ACP, 0, src, -1, NULL, 0, NULL, NULL );
    dest = (char*)calloc(len+1,1);

    WideCharToMultiByte(CP_ACP,0,src,-1,dest,len,NULL,NULL);
#else
    int alen = wcstombs( NULL, src, 0 );
    if ( alen > 0 )
    {
        dest = new char[ alen + 1 ];
        if ( dest != NULL )
        {
            memset( dest, 0, alen + 1 );
            wcstombs( dest, src, alen + 1 );
        }
    }
#endif
    return dest;
}

wchar_t *ConvertFromMBCS( const char *src)
{
    static wchar_t* dest = NULL;

    if ( ( dest != NULL ) || ( src == NULL ) )
    {
        delete[] dest;
        dest = NULL;
    }

    if ( src == NULL )
    {
        return NULL;
    }

#ifdef _WIN32
    int len = strlen(src) + 1;
    dest = (wchar_t*)calloc(len * 2,1);
    MultiByteToWideChar(CP_ACP,0,src,-1,dest,len);
#else
    int alen = mbstowcs( NULL, src, 0 );
    if ( alen > 0 )
    {
        dest = new wchar_t[ alen + 1 ];
        if ( dest != NULL )
        {
            int szbuff = (alen + 1)*sizeof(wchar_t);
            memset( dest, 0,  szbuff );
            mbstowcs( dest, src, alen );
        }
    }
#endif
    return dest;
}

WORD SwapWORD(WORD nWord)
{
    WORD nRet = 0;

    BYTE *pA1 = (BYTE*)&nWord;
    BYTE *pA2 = pA1+1;
    nRet = (*pA1 << 8 ) + *pA2;

    return nRet;
}

DWORD SwapDWORD(DWORD nDWord)
{
    DWORD nRet = 0;

    BYTE *pA1 = (BYTE*)&nDWord;
    BYTE *pA2 = pA1+1;
    BYTE *pA3 = pA1+2;
    BYTE *pA4 = pA1+3;
    nRet = (*pA2 << 24 ) +
           (*pA1 << 16 ) +
           (*pA4 << 8  ) +
            *pA3;

    return nRet;
}

