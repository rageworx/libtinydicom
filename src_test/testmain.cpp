#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "testmain.h"

#if defined(UNICODE) || defined(_UNICODE)
    #define _T##x   L##x
    #define PRT     wprintf
    #define CHAR_T  wchar_t
    #define _S_     "%S"
    #define ACCESS  _waccess
    #define UNLINK  _wunlink
#else
    #define _T
    #define PRT     printf
    #define CHAR_T  char
    #define _S_     "%s"
    #define ACCESS  access
    #define UNLINK  unlink
#endif

#ifndef WORD
    #define WORD    uint16_t
#endif /// of WORD

using namespace tinydicom;
using namespace std;

WORD GetElem2WORD( DCMTagElement* pElem )
{
    WORD tmpUS = 0;

    if ( pElem != NULL )
    {
        if ( memcmp( &pElem->VRtype, "US", 2 ) == 0 )
        {
            memcpy( &tmpUS, pElem->staticbuffer, 2 );
        }
    }

    return tmpUS;
}

int main(int argc, char** argv)
{
    int libvers[4] = {0};
    GetTinyDicomLibVersion( libvers );
    
    PRT( _T"libtinydicom version %d.%d.%d.%d testing \n",
             libvers[0], libvers[1], libvers[2], libvers[3] );
    PRT( _T"(C)2023 Raphael Kim, rageworx@gmail.com\n\n" );
    
    CHAR_T reffn[] = _T"../dcm/TEST.DCM";
    //CHAR_T reffn[] = _T"../dcm/SIMFIT.DCM";

    PRT( _T"Loading DCM ["_S_"] ... ", reffn );

    if ( OpenDCM( reffn ) == true )
    {
        PRT( _T"OK.\n" );
        PRT( _T"\n" );

        DCMTagElement* pTagModal = FindElement( 0x00080060 );
        DCMTagElement* pTagKVP = FindElement( 0x00180060 );
        DCMTagElement* pTagIPP = FindElement( 0x00200032 );
        DCMTagElement* pTagIOP = FindElement( 0x00200037 );
        DCMTagElement* pTagFORUID = FindElement( 0x00200052 );

        DCMTagElement* pTagSPP = FindElement( 0x00280002 );
        DCMTagElement* pTagPI  = FindElement( 0x00280004 );
        DCMTagElement* pTagRow = FindElement( 0x00280010 );
        DCMTagElement* pTagCol = FindElement( 0x00280011 );
        DCMTagElement* pTagPln = FindElement( 0x00280012 );
        DCMTagElement* pTagPxS = FindElement( 0x00280030 );

        printf( "\n" );
        if ( pTagModal != NULL )
        {
            printf( "Modality                   : %s\n", pTagModal->staticbuffer );
        }

        if ( pTagKVP != NULL )
        {
            printf( "KVP                        : %s\n", pTagKVP->staticbuffer );
        }

        if ( pTagIPP != NULL )
        {
            printf( "Image Position Patient     : %s\n", pTagIPP->staticbuffer );
        }

        if ( pTagIOP != NULL )
        {
            printf( "Image Orientation Patient  : %s\n", pTagIOP->staticbuffer );
        }

        if ( pTagFORUID != NULL )
        {
            printf( "Frame of Reference UID     : %s\n", pTagFORUID->staticbuffer );
        }

        printf( "\n" );

        if ( pTagSPP != NULL )
        {
            printf( "Samples per pixel          : %d\n", GetElem2WORD( pTagSPP ) );
        }

        if ( pTagPI != NULL )
        {
            printf( "Photometric Interpretation : %s\n", pTagPI->staticbuffer );
        }

        if ( pTagRow != NULL )
        {
            printf( "Rows                       : %d\n", GetElem2WORD( pTagRow ) );
        }

        if ( pTagCol != NULL )
        {
            printf( "Colums                     : %d\n", GetElem2WORD( pTagCol ) );
        }
        
        if ( pTagPln != NULL )
        {
            printf( "Planes                     : %d\n", GetElem2WORD( pTagPln ) );
        }

        if ( pTagPxS != NULL )
        {
            printf( "Pixel Spacing              : %s\n", pTagPxS->staticbuffer );
        }

        CloseDCM();
    }
    else
    {
        PRT( _T"Failure !\n" );
#ifdef DEBUG_W_PAUSE
        system("PAUSE");
#endif /// of DEBUG_W_PAUSE
        return 0;
    }
    
    fflush( stdout );

    CHAR_T newDCMn[] = _T"NEW.DCM";

    PRT( _T"\n\n" );
    PRT( _T"Creating a new DCM ["_S_"] ... ", newDCMn );

    if ( ACCESS( newDCMn, 0 ) == 0 )
    {
        if ( UNLINK( newDCMn ) != 0 )
        {
            PRT( _T"Failure\nFile existed and not be removed !\n" );
            return 0;
        }
    }

    if ( NewDCM( newDCMn ) == true )
    {
        PRT( _T"Ok !\n" );
        PRT( _T"Adding some tags ... \n" );

        // Modality = CT
        DCMTagElement* newTagModal = NULL;
        if ( NewElement( 0x00080060, &newTagModal ) == true )
        {
            WriteAnsiString( newTagModal, "CT" );
        }
        else
        {
            PRT( _T"Failed to create Modality tag.\n" );
            CloseDCM();
            return 0;
        }

        // An empty image for test.
        ImageInformation imginfo = {0};
        imginfo.width  = 500;
        imginfo.height = 500;
        imginfo.spacing_w = 0.25f;
        imginfo.spacing_h = 0.25f;
        imginfo.bpp    = 16;
        imginfo.pixels = new uint8_t[ 500*500*2 ];

        if ( imginfo.pixels != NULL )
        {
            memset( imginfo.pixels, 0, 500*500*2 );
            if ( AddImage( &imginfo ) == false )
            {
                PRT( _T"Error: Failed to add an image.\n" );
                delete[] (char*)imginfo.pixels;

                return 0;
            }

            DCMTagElement* newTagPI = NULL;
            if ( NewElement( 0x00280004, &newTagPI ) == true )
            {
                WriteAnsiString( newTagPI, "MONOCHROME2" );
            }

            DCMTagElement* newTagIT = NULL;
            if ( NewElement( 0x00080008, &newTagIT ) == true )
            {
                WriteAnsiString( newTagIT, "ORIGINAL\\PRIMARY\\AXIAL " );
            }
        }

        PRT( _T"Writing ..." );

        if( SaveDCM( newDCMn ) == true )
        {
            PRT( _T"Ok.\n");
        }
        else
        {
            PRT( _T"Failed.\n");
        }

        CloseDCM();
    }
    else
    {
        printf("Failure !\n");
    }

#ifdef DEBUG_W_PAUSE
    system("PAUSE");
#endif /// of DEBUG_W_PAUSE

    return 0;
}
