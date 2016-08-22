/*******************************************************************************
* STL , DicomImageViewer::DicomDecoder
*
* * (C)Copyright 2011-2014 Raphael Kim (rage.kim)
* * Unicode Model.
*
*******************************************************************************/
#ifndef _DICOMDECODER_H_
#define _DICOMDECODER_H_

#include <fstream>
#include <list>

// fstream requires "std" namespace.
using namespace std;

namespace DicomImageViewer
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

    // String related VRs ------------------------------------------------------
    // AE: (String, ~16bytes) Application Entitiy
    #define AE                          0x4145
    // AS: (String, 4bytes, fixed) Age String
    #define AS                          0x4153
    // AT: (String, 4bytes, fixed) Attribute Tag
    #define AT                          0x4154
    // CS: (String, ~16bytes) Code String
    #define CS                          0x4353
    // DA: (String, 8bytes, fixed in DCM tags) Date, need refer to NEMA DICOM PS3.5 pg38.
    #define DA                          0x4441
    // DS: (String, ~16bytes) Decimal String
    #define DS                          0x4453
    // DT: (String, ~26bytes in DCM tags) Date Time
    #define DT                          0x4454
    // IS: (String, ~12bytes) Integer String
    #define IS                          0x4953
    // LO: (String, ~64bytes) Long String
    #define LO                          0x4C4F
    // LT: (String, ~10240bytes) Long Text
    #define LT                          0x4C54
    // PN: (String, ~64bytes) Person Name
    #define PN                          0x504E
    // SH: (String, ~16btytes) Short String
    #define SH                          0x5348
    // ST: (String, ~1024 bytes) Short Text
    #define ST                          0x5354
    // TM: (String, ~14bytes) Time
    #define TM                          0x544D
    // UC: (String, ~2^32-2 bytes) Unlimited Characters
    #define UC                          0x5543
    // UI: (String, ~64bytes) Unique Identifier (UID)
    #define UI                          0x5549
    // UN: (String, ~any) Unknown
    #define UN                          0x554E
    // UT: (String, ~2^32-2 bytes) Unlimited Texxt
    #define UT                          0x5554


    // Floating point VRs ------------------------------------------------------
    // FD: (Floating point, 4bytes) Floating point single
    #define FD                          0x4644
    // FL: (Floating point, 8bytes) Floating point double
    #define FL                          0x464C
    // OB: (BYTE, 1byte) Other Byte
    #define OB                          0x4F42
    // OD: (64bit IEEE 754 stream, 2^32-8 bytes) Other Double
    #define OD                          0x4FD0


    // Integer VRs ------------------------------------------------------
    // SL: (Signed long, 4bytes) Signed Long
    #define SL                          0x534C
    // SS: (Singed Short, 2bytes) Sing Short
    #define SS                          0x5353
    // SQ: (? Described in Section 7.5) Sequence of Items
    #define SQ                          0x5351
    // UL: (Unsigned Long, 4bytes) Unsigned Long
    #define UL                          0x554C
    // US: (Unsigned Short, 2bytes) Unsigned Short
    #define US                          0x5553
    // OF: (32bit IEEE 754 stream, 2^32-4 bytes) Other Float
    #define OF                          0x4F46
    // OL: (32bit words, may 4bytes) Other Long
    #define OL                          0x4F4C
    // OW: (16bit word, may 2bytes) Other Word
    #define OW                          0x4F57

    // Unknown VRs ------------------------------------------------------
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
