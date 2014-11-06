#include "testmain.h"

WORD GetElem2WORD( TagElement* pElem )
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
    wchar_t reffn[] = L"TEST.DCM";

    wprintf( L"Open %s ... ", reffn );

    if ( OpenDCM( reffn ) == true )
    {
        wprintf( L"OK.\n" );
        wprintf( L"\n" );

        TagElement* pTagModal = FindElement( 0x00080060 );
        TagElement* pTagKVP = FindElement( 0x00180060 );
        TagElement* pTagIPP = FindElement( 0x00200032 );
        TagElement* pTagIOP = FindElement( 0x00200037 );
        TagElement* pTagFORUID = FindElement( 0x00200052 );

        TagElement* pTagSPP = FindElement( 0x00280002 );
        TagElement* pTagPI  = FindElement( 0x00280004 );
        TagElement* pTagRow = FindElement( 0x00280010 );
        TagElement* pTagCol = FindElement( 0x00280011 );
        TagElement* pTagPxS = FindElement( 0x00280030 );

        printf( "\n" );
        printf( "Modality                   : %s\n", pTagModal->staticbuffer );
        printf( "KVP                        : %s\n", pTagKVP->staticbuffer );
        printf( "Image Position Patient     : %s\n", pTagIPP->staticbuffer );
        printf( "Image Orientation Patient  : %s\n", pTagIOP->staticbuffer );
        printf( "Frame of Reference UID     : %s\n", pTagFORUID->staticbuffer );

        printf( "\n" );

        printf( "Samples per pixel          : %d\n", GetElem2WORD( pTagSPP ) );
        printf( "Photometric Interpretation : %s\n", pTagPI->staticbuffer );
        printf( "Rows                       : %d\n", GetElem2WORD( pTagRow ) );
        printf( "Colums                     : %d\n", GetElem2WORD( pTagCol ) );
        printf( "Pixel Spacing              : %s\n", pTagPxS->staticbuffer );

        CloseDCM();
    }
    else
    {
        wprintf( L"Failure !\n" );
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
        TagElement* newTagModal = NULL;
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

            TagElement* newTagPI = NULL;
            if ( NewElement( 0x00280004, &newTagPI ) == true )
            {
                WriteAnsiString( newTagPI, "MONOCHROME2" );
            }

            TagElement* newTagIT = NULL;
            if ( NewElement( 0x00080008, &newTagIT ) == true )
            {
                WriteAnsiString( newTagIT, "ORIGINAL\\PRIMARY\\AXIAL" );
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
