#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include <libdcm.h>

#define DEF_PRINT_HEX_LIMIT     12

using namespace tinydicom;

bool isStringType( const uint8_t* sVR, const uint8_t* dt )
{
    if ( ( sVR != NULL ) && ( dt != NULL ) )
    {
        // kind of UI ...
        if ( sVR[0] == 'U' )
        {
            if ( ( sVR[1] == 'I' ) )
                return true;
        }

        // kind of DS, DA ..
        if ( sVR[0] == 'D' )
        {
            if ( ( sVR[1] == 'S' ) || ( sVR[1] == 'A' ) )
                return true;
        }

        // kind of TM
        if ( sVR[0] == 'T' )
        {
            if ( ( sVR[1] == 'M' ) )
                return true;
        }

        // kind of LO
        if ( sVR[0] =='L' )
        {
            if ( ( sVR[1] == 'O' ) )
                return true;
        }

        // kind of CS
        if ( sVR[0] == 'C' )
        {
            if ( ( sVR[1] =='S' ) )
                return true;
        }

        // kind of SH, ST
        if ( sVR[0] == 'S' )
        {
            if ( ( sVR[1] =='H' ) || ( sVR[1] == 'T' ) )
                return true;
        }

        // kind of PN,
        if ( sVR[0] == 'P' )
        {
            if ( ( sVR[1] =='N' ) )
                return true;
        }

    }

    return false;
}

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
            if ( ( pElem->VRtype[0] > 0 ) && ( pElem->VRtype[1] > 0 ) )
            {
                printf( "      -- VR = %02X%02X (%s)\n",
                        pElem->VRtype[0],
                        pElem->VRtype[1],
                        pElem->VRtype );
            }
            else
            {
                printf( "      -- VR = (none)\n" );
            }

            if ( ( pElem != NULL ) && ( pElem->size > 0 ) )
            {
                printf( "      -- size = %u\n", pElem->size );
                printf( "      -- data = " );

                uint8_t* pDt = NULL;
                if ( pElem->alloced == false )
                    pDt = (uint8_t*)pElem->staticbuffer;
                else
                if ( pElem->dynamicbuffer != NULL )
                    pDt = (uint8_t*)pElem->dynamicbuffer;

                if ( isStringType( pElem->VRtype, pDt ) == true )
                {
                    char pStr[80] = {0};
                    memset( pStr, 0, 80 );
                    snprintf( pStr, 80, "%s", (const char*)pDt );
                    printf( "\"%s\"", pStr );
                }
                else
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
            else
            {
                printf( "(nodata)\n" );
            }
        }
        else
        {
            printf( "[%4d] error by %S.\n", cnt+1, GetLastErrMsg() );
        }
    }

    CloseDCM();

    return 0;
}
