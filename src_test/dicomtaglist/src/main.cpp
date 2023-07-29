#include <unistd.h>
#include <wchar.h>
#include <getopt.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include <libdcm.h>

#define DEF_PRINT_HEX_LIMIT     ( 12 )

using namespace tinydicom;

static struct option long_opts[] = {
    { "help",           no_argument,        0, 'h' },
    { "exportraw",      required_argument,  0, 'e' },
    { NULL, 0, 0, 0 }
};

char* idcmfile = NULL;
char* orawfile = NULL;

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

void showHelp()
{
    printf( "_options_\n" );
    printf( " h/help                    = show this help.\n" );
    printf( " e/exportraw {file prefix} = exports raw image and information as named file prefix.\n" );
    printf( "\n" );
}

int main( int argc, char** argv )
{
    int libdcmv[4] = {0};
    GetTinyDicomLibVersion( libdcmv );

    printf( "DICOM tag lsit viewer with libtinydcm v%d.%d.%d.%d,\n"
            "(C)Copyrighted 2023 Raphael Kim, rageworx@gmail.com\n",
            libdcmv[0], libdcmv[1], libdcmv[2], libdcmv[3] );

    for(;;)
    {
        int optidx = 0;
        int opt = getopt_long( argc, argv,
                               " :e:h",
                               long_opts, &optidx );
        if ( opt >= 0 )
        {
            switch( (char)opt )
            {
                default:
                case 'h':
                case '?':
                    showHelp();
                    return 0;

                case 'e':
                    if ( optarg != NULL )
                    {
                        orawfile = strdup( optarg );
                    }
                    break;
            }
        }
        else
            break;
    }

    // get non-optioned file name ...
    for( ; optind<argc; optind++ )
    {
        const char* tmp = argv[optind];
        
        if ( ( tmp != NULL) && ( idcmfile == NULL ) )
        {
            idcmfile = strdup( tmp );
            break;
        }
    }

    if ( idcmfile == NULL )
    {
        printf( "\n * no parameter to open a file.\n" );
        return 0;
    }

    const char* fname = idcmfile;

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

                // Handle image ...
                if ( ( lID == 0x7FE0 ) & ( hID == 0x0010 ) &&
                     ( pElem->size > 0 ) )
                {
                    if ( orawfile != NULL )
                    {
                        char outfn[512] = {0};
                        snprintf( outfn, 512,
                                  "%s.raw",
                                  orawfile );
                        FILE* fp = fopen( outfn, "wb" );
                        if ( fp != NULL )
                        {
                            fwrite( pDt, pElem->size, 1, fp );
                            fclose( fp );
                            printf( " ... file %s written.\n",
                                    outfn );
                        }
                        else
                        {
                            printf( " ... file %s write failure.\n",
                                    outfn );

                        }
                    }
                }
            }
            else
            {
                printf( "      -- no additional data\n" );
            }
        }
        else
        {
            const wchar_t* errstr = GetLastErrMsg();
            if ( wcslen( errstr ) > 0 )
                printf( "[%4d] error by %S.\n", cnt+1, GetLastErrMsg() );
            else
                printf( "[%4d] (no data)\n", cnt+1 );
        }
    }

    CloseDCM();

    return 0;
}
