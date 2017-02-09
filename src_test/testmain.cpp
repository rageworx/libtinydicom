#include "testmain.h"

WORD GetElem2WORD( DCMTagElement* pElem )
{
    WORD tmpUS = 0;

    if ( pElem != NULL )
    {
        if ( strncmp( pElem->VRtype, "US", 2 ) == 0 )
        {
            memcpy( &tmpUS, pElem->staticbuffer, 2 );
        }
    }

    return tmpUS;
}

int main(int argc, char** argv)
{
    wchar_t reffn[] = L"dcm/TEST.DCM";

    wprintf( L"Open %S ... ", reffn );

    if ( OpenDCM( reffn ) == true )
    {
        wprintf( L"OK.\n" );
        wprintf( L"\n" );

        DCMTagElement* pTagModal = FindElement( 0x00080060 );
        DCMTagElement* pTagKVP = FindElement( 0x00180060 );
        DCMTagElement* pTagIPP = FindElement( 0x00200032 );
        DCMTagElement* pTagIOP = FindElement( 0x00200037 );
        DCMTagElement* pTagFORUID = FindElement( 0x00200052 );

        DCMTagElement* pTagSPP = FindElement( 0x00280002 );
        DCMTagElement* pTagPI  = FindElement( 0x00280004 );
        DCMTagElement* pTagRow = FindElement( 0x00280010 );
        DCMTagElement* pTagCol = FindElement( 0x00280011 );
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

        if ( pTagPxS != NULL )
        {
            printf( "Pixel Spacing              : %s\n", pTagPxS->staticbuffer );
        }

        CloseDCM();
    }
    else
    {
        wprintf( L"Failure !\n" );
        system("PAUSE");

        return 0;
    }

    wchar_t newDCMn[] = L"NEW.DCM";

    wprintf( L"\n\n" );
    wprintf( L"Creating %s ... ", newDCMn );

    if ( _waccess( newDCMn, F_OK) == 0 )
    {
        if ( _wunlink( newDCMn ) != 0 )
        {
            wprintf( L"Failure\nFile existed and not be removed !\n" );
            return 0;
        }
    }

    if ( NewDCM( newDCMn ) == true )
    {
        wprintf( L"Ok !\n" );
        wprintf( L"Adding some tags ... \n" );

        // Modality = CT
        DCMTagElement* newTagModal = NULL;
        if ( NewElement( 0x00080060, &newTagModal ) == true )
        {
            WriteAnsiString( newTagModal, "CT" );
        }
        else
        {
            wprintf( L"Failed to create Modality tag.\n" );
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
        imginfo.pixels = new char[ 500*500*2 ];

        if ( imginfo.pixels != NULL )
        {
            memset( imginfo.pixels, 0, 500*500*2 );
            if ( AddImage( &imginfo ) == false )
            {
                wprintf( L"Error: Failed to add an image.\n" );
                delete[] imginfo.pixels;

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

        wprintf( L"Writing ..." );

        if( SaveDCM( newDCMn ) == true )
        {
            wprintf( L"Ok.\n");
        }
        else
        {
            wprintf( L"Failed.\n");
        }

        CloseDCM();
    }
    else
    {
        printf("Failure !\n");
    }

    system("PAUSE");

    return 0;
}
