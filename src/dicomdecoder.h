/*******************************************************************************
* STL , DicomImageViewer::DicomDecoder
*
* * Programmed by Raphael Kim (rageworx@gmail.com)
* * Unicode Model.
*
*******************************************************************************/
#ifndef _DICOMDECODER_H_
#define _DICOMDECODER_H_

#include <fstream>
#include <list>

// fstream requires "std" namespace.
using namespace std;

namespace TinyDicom
{

    #define PIXEL_REPRESENTATION        0x00280103
    #define TRANSFER_SYNTAX_UID         0x00020010
    #define MODALITY                    0x00080060
    #define SLICE_THICKNESS             0x00180050
    #define SLICE_SPACING               0x00180088
    #define SAMPLES_PER_PIXEL           0x00280002
    #define PHOTOMETRIC_INTERPRETATION  0x00280004
    #define PLANAR_CONFIGURATION        0x00280006
    #define NUMBER_OF_FRAMES            0x00280008
    #define ROWS                        0x00280010
    #define COLUMNS                     0x00280011
    #define PIXEL_SPACING               0x00280030
    #define BITS_ALLOCATED              0x00280100
    #define WINDOW_CENTER               0x00281050
    #define WINDOW_WIDTH                0x00281051
    #define RESCALE_INTERCEPT           0x00281052
    #define RESCALE_SLOPE               0x00281053
    #define RED_PALETTE                 0x00281201
    #define GREEN_PALETTE               0x00281202
    #define BLUE_PALETTE                0x00281203
    #define ICON_IMAGE_SEQUENCE         0x00880200
    #define PIXEL_DATA                  0x7FE00010

    // additional TAG ID for VATECH.
    #define SOPCLASS_UID                0x00020002  // added by rage.kim
    #define SOPINSTANCE_UID             0x00180008  // added by rage.kim

    #define ITEM                        0xFFFEE000
    #define ITEM_DELIMITATION           0xFFFEE00D
    #define SEQUENCE_DELIMITATION       0xFFFEE0DD

    #define AE                          0x4145
    #define AS                          0x4153
    #define AT                          0x4154
    #define CS                          0x4353
    #define DA                          0x4441
    #define DS                          0x4453
    #define DT                          0x4454
    #define FD                          0x4644
    #define FL                          0x464C
    #define IS                          0x4953
    #define LO                          0x4C4F
    #define LT                          0x4C54
    #define PN                          0x504E
    #define SH                          0x5348
    #define SL                          0x534C
    #define SS                          0x5353
    #define ST                          0x5354
    #define TM                          0x544D
    #define UI                          0x5549
    #define UL                          0x554C
    #define US                          0x5553
    #define UT                          0x5554
    #define OB                          0x4F42
    #define OW                          0x4F57
    #define SQ                          0x5351
    #define UN                          0x554E
    #define QQ                          0x3F3F

    #define ID_OFFSET                   0x80        // = 128
    #define DICM                        "DICM"
    #define IMPLICIT_VR                 0x2D2D      // "--"

    #define CHARMIN                     -128
    #define SHORTMIN                    -32768

    #define MIN8                        CHARMIN
    #define MIN16                       SHORTMIN

    #define UNIT_MM                     "mm"
    #define UNIT_INCH                   "inch"

};
#endif // of _DICOMDECODER_H_
