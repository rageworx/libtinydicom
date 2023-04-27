#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include <libdcm.h>

#define DEF_PRINT_HEX_LIMIT     12

using namespace tinydicom;

int main( int argc, char** argv )
{
    int libdcmv[4] = {0};
    GetTinyDicomLibVersion( libdcmv );

    printf( "DICOM tag lsit viewer with libtinydcm v%d.%d.%d.%d,\n"
            "(C)Copyrighted 2023 Raphael Kim, rageworx@gmail.com\n",
            libdcmv[0], libdcmv[1], libdcmv[2], libdcmv[3] );

    if ( argc <= 1 )
    {
        printf( "\n * no parameter to open a file.\n" );
        return 0;
    }

    const char* fname = argv[1];

    if ( OpenDCMA( fname ) == false )
    {
        printf( "\n * Error, cannot open file = %s\n", fname );
        return 0;
    }

    int elems = GetElementCount();

    printf( " * Elements = %d\n", elems );

    const char no_dict[] = "(No definition availed)";

    for( int cnt=0; cnt<elems; cnt++ )
    {
        DCMTagElement* pElem = NULL;
        if ( GetElement( cnt, &pElem ) > 0 )
        {
            uint16_t lID = ( pElem->id >> 16 );
            uint16_t hID = ( pElem->id & 0x0000FFFF);

            const char* pMean = GetDicomMeaningA( pElem->id );
            if ( pMean == NULL )
                pMean = no_dict;

            printf( "[%4d] %04X,%04X = %s\n",
                    cnt+1,lID, hID,
                    pMean );
            printf( "      -- VR = %02X%02X (%s)\n",
                    pElem->VRtype[0],
                    pElem->VRtype[1],
                    pElem->VRtype );
            printf( "      -- size = %u\n", pElem->size );
            printf( "      -- data = " );

            uint8_t* pDt = NULL;
            if ( pElem->alloced == false )
                pDt = (uint8_t*)pElem->staticbuffer;
            else
            if ( pElem->dynamicbuffer != NULL )
                pDt = (uint8_t*)pElem->dynamicbuffer;

            if ( pDt != NULL )
            {
                for( int dQ=0; dQ<pElem->size; dQ++ )
                {
                    if ( dQ < DEF_PRINT_HEX_LIMIT )
                    {
                        printf( "%02X ", pDt[dQ] );
                    }
                    else
                    {
                        printf( "..." );
                        break;
                    }
                }
                printf( "\n" );
            }
        }
        else
        {
            printf( " * element [%03d] %S.\n", cnt+1, GetLastErrMsg() );
        }
    }

    CloseDCM();

    return 0;
}
