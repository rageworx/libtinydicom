# libtinydicom

## Version
 - 0.5.5.153

## Introduce
 - A DICOM tag read/write library for using standard C++.
 - Following DICOM standard part of "Media Interchange/DICOM Basic File Service" of 2023b.
 - Related in DICOM tag file (.DCM) read and write library is too far to get free in light size. So I made this to read and write DCM files for free and tiny ! It is still prototype and need to be enhanced.

## Project target
 - Basically this project targeted to has non-platform-dependency, designed for POSIX.
 - Just build it with your gcc or MinGW-W64 in shell (or MSYS2) via make. Maybe there's no platform dependency.

## License
 - MIT License.
 - You can use my source code whatever you want.
 - But no warranty, just remember this please.

## Known VR (WORD)
 - See source code of dicomdictinary.cpp.
 - I have gathering more VR information, but it is really narrow to know. Absolutely limited.
 - Still making it keep updated for latest version, but too many VR and dictionaries may need more time for only myself.

## Updated news

### 2023-07-30
 - Updated some library codes.
 - Fixed some bugs in src_test/dicomtaglist, also updated to export raw and meta info by `-e` argument.

### 2023-04-28
 - DICOM standard 2023b VR, Dictionary updated.
 - DicomDictionary::GetMediaMimeFromUID() included.

### 2023-04-27
 - Removed `BYTE`, `WORD`, `DWORD` types, and then replaced to cstdint types.
 - `BYTE` to `uint8_t`, `WORD` to `uint16_t` and `DWORD` to `uint32_t`.
 - Limit of reading size depends on platform `size_t` type size.
 - More buffer allocation check routines.

### 2023-04-26
 - Supporting -1 size of Pixel Data case, it guess actual size to left file size to actual pixel data length.
 - Added some more MBCS support APIs.

### 2021-04-28
 - libtinydicom now supports MacOS 11 (Big Sur) in universal libraray on Apple Silicon (M1).

### 2019-12-03
 - Moved all Makefiles into makes directory.
 - Moved all cbp project files into cbp directory.
 - VR type OB fixed to has 2 bytes padding.

### 2017-03-10
 - Fixed read and processing some unrecognized DICOM tag related in Pixel Data.

### 2017-03-04
 - Fixed windows dependency codes for other platforms like MAC OS and Linux.

### 2017-02-14
 - Fixed some bugs for reading compatibilities.

### 2017-02-13
 - Fixed a bug sometimes wrong pointer returns in FindElement();
 - Included FindElementIndex();

### 2017-02-09
 - Added some functions to supporting ANSI, UTF-8 base files.
 - Also inserted some macro to keep same method name.

### 2017-02-08
 - Enhanced to read bad VR and UUID tags.
 - Some DCM file contains bad VR, or UUID. Made it possible to read.

### 2016-12-15
 - Now supporting Makefile ( I didn't know this file not included... )
 - Make has two different options, `make` and `make debug`.
 - Removed some platform dependency issues.

### 2016-09-12
 - Working for latest dictionary for 3.6 (2016c) now.
 - Tagged to 'dictionaryworking' until it complete written all dictionaries.
 - Added some tag state as like 'retired, trial, dicos, diconde'.

### 2014-11-06
 - updated dicom dictionary.
 - updated pixel information writing.
 - added dll project for exporting fuctions.
 - added dll testing project.

