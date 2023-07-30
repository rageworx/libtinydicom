/////////////////////////////////////////////////////////////////////////
//                                                                     //
// A DICOM tag list & exporting tool example                           //
// ==================================================================  //
// (C)Copyright 2023, Raphael Kim, libtinydcm author                   //
//                                                                     //
// * Disclaim *                                                        //
//  This example is not provides standard examination for medical      //
// result.                                                             //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

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

typedef struct _metainfo_t {
    float       spacing_w;
    float       spacing_h;
    size_t      width;
    size_t      height;
    char*       coltype;
    size_t      window_w;
    size_t      window_wd;
    size_t      window_c;
    size_t      window_cd;
    uint32_t    compression;
    char*       comp_method;
}metainfo_t;

static struct option long_opts[] = {
    { "help",           no_argument,        0, 'h' },
    { "exportraw",      required_argument,  0, 'e' },
    { NULL, 0, 0, 0 }
};

static char*        idcmfile = NULL;
static char*        orawprefix = NULL;
static metainfo_t   metainfo = {0};     

bool write_raw( const char* r, const uint8_t* d, size_t l )
{
    char outfn[512] = {0};
    snprintf( outfn, 512, "%s.raw", r );
            
    FILE* fp = fopen( outfn, "wb" );
    if ( fp != NULL )
    {
        fwrite( d, l, 1, fp );
        fclose( fp );
        return true;
    }

    return false;
}

bool write_meta( const char* r )
{
    char outfn[512] = {0};
    snprintf( outfn, 512, "%s.meta", r );

    FILE* fp = fopen( outfn, "w" );
    if ( fp != NULL )
    {
        fprintf( fp, "[METAINFO]\n" );
        fprintf( fp, "FILE=%s.raw\n", r );
        fprintf( fp, "[PIXEL_SPACING]\n" );
        fprintf( fp, "WIDTH=%f\n", metainfo.spacing_w );
        fprintf( fp, "HEIGHT=%f\n", metainfo.spacing_h );
        fprintf( fp, "[RESOLUTION]\n" );
        fprintf( fp, "WIDTH=%zu\n", metainfo.width );
        fprintf( fp, "HEIGHT=%zu\n", metainfo.height );
        fprintf( fp, "TYPE=" );
        if ( metainfo.coltype == NULL )
        {
            fprintf( fp, "UNKNOWN\n" );
        }
        else
        {
            fprintf( fp, "%s\n", metainfo.coltype );
        }
        // some DCM not have window width and center 
        if ( ( metainfo.window_w > 0 ) || ( metainfo.window_c ) )
        {
            fprintf( fp, "[WINDOW]\n" );
            fprintf( fp, "WIDTH=%zu\n", metainfo.window_w );
            fprintf( fp, "WIDTHDIV=%zu\n", metainfo.window_wd );
            fprintf( fp, "CENTER=%zu\n", metainfo.window_c );
            fprintf( fp, "CENTERDIV=%zu\n", metainfo.window_cd );
        }
        fprintf( fp, "[COMRESSION]\n" );
        fprintf( fp, "TYPE=%u\n", metainfo.compression );
        fprintf( fp, "STANDARD=" );
        if ( metainfo.comp_method == NULL )
        {
            fprintf( fp, "NONE\n" );
        }
        else
        {
            fprintf( fp, "%s\n", metainfo.comp_method );
        }

        fclose( fp );
        return true;
    }

    return false;
}

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
                        orawprefix = strdup( optarg );
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
        if ( GetElement( cnt, &pElem ) == cnt )
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

                // handle image related --
                if ( lID == 0x0028 )
                {
                    switch( hID )
                    {
                        case 0x0004: /// type of data -
                            if ( pElem->size > 0 )
                                metainfo.coltype = strdup( (const char*)pDt );
                            break;

                        case 0x0010: /// rows 
                            if ( pElem->size == 2 )
                                metainfo.height = *(uint16_t*)pDt;
                            else
                            if ( pElem->size == 4 )
                                metainfo.height = *(uint32_t*)pDt;
                            break;

                        case 0x0011: /// colums
                            if ( pElem->size == 2 )
                                metainfo.width = *(uint16_t*)pDt;
                            else
                            if ( pElem->size == 4 )
                                metainfo.width = *(uint32_t*)pDt;
                            break;

                        case 0x0030: /// pixel spacing 
                            if ( pDt != NULL )
                            {
                                // seperate by "\".
                                char* pSrc = strdup( (const char*)pDt );
                                char* pTok = strtok( pSrc, "\\" );
                                float cv = atof( pTok );
                                metainfo.spacing_h = cv;
                                pTok = strtok( NULL, "\\" );
                                cv = atof( pTok );
                                metainfo.spacing_w = cv;
                                delete[] pSrc;
                            }
                            break;

                        case 0x1050: /// window center
                            if ( pDt != NULL )
                            {
                                // seperate by "\".
                                char* pSrc = strdup( (const char*)pDt );
                                char* pTok = strtok( pSrc, "\\" );
                                if ( pTok != NULL )
                                {
                                    metainfo.window_c = atof( pTok );
                                    pTok = strtok( NULL, "\\" );
                                    metainfo.window_cd = atof( pTok );
                                }
                                else
                                {
                                    metainfo.window_c = atoi( pSrc );
                                }
                                delete[] pSrc;
                            }
                            break;

                        case 0x1051: // window width
                            if ( pDt != NULL )
                            {
                                // seperate by "\".
                                char* pSrc = strdup( (const char*)pDt );
                                char* pTok = strtok( pSrc, "\\" );
                                if ( pTok != NULL )
                                {
                                    metainfo.window_w = atof( pTok );
                                    pTok = strtok( NULL, "\\" );
                                    metainfo.window_wd = atof( pTok );
                                }
                                else
                                {
                                    metainfo.window_w = atoi( pSrc );
                                }
                                delete[] pSrc;
                            }
                            break;

                        case 0x2110:
                            if ( pDt != NULL )
                            {
                                int cv = atoi( (const char*)pDt );
                                metainfo.compression = cv;
                            }
                            break;

                        case 0x2114:
                            if ( pElem->size > 0 )
                                metainfo.comp_method = strdup( (const char*)pDt );
                        break;
                    }
                }

                // Handle data --
                if ( ( lID == 0x7FE0 ) & ( hID == 0x0010 ) &&
                     ( pElem->size > 0 ) )
                {
                    if ( orawprefix != NULL )
                    {
                        if ( write_raw( orawprefix, pDt, pElem->size ) == true )
                        {
                            printf( " ... file %s.raw written.\n",
                                    orawprefix );
                        }
                        else
                        {
                            printf( " ... file %s.raw write failure.\n",
                                    orawprefix );

                        }

                        if ( write_meta( orawprefix ) == true )
                        {
                            printf( " ... file %s.meta written.\n",
                                    orawprefix );
                        }
                        else
                        {
                            printf( " ... file %s.meta write failure.\n",
                                    orawprefix );
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

    // release memories
    if ( metainfo.coltype != NULL )
        delete metainfo.coltype;

    if ( metainfo.comp_method != NULL )
        delete metainfo.comp_method;

    return 0;
}
