#libtinydicom


## Introduce
 - A DICOM tag read/write library for using C++/STL
 - Related in DICOM tag file (.DCM) read and write library is too far to get free in light size. So I made this to read and write DCM files for free and tiny ! It is still prototype and need to be enhanced.

## Project target
 - Basically this project targeted to has non-platform-dependency.
 - Just build it with your gcc or MinGW in shell (or MSYS) via make. Maybe there's no platform dependency.

## License
 - MIT License.
 - You can use my source code whatever you want.
 - But no warranty, just remember this please.

## Known VR (WORD)
 - See source code of dicomdictinary.cpp.
 - I have gathering more VR information, but it is really narrow to know. Absolutely limited.
 - Still making it keep updated for latest version, but too many VR and dictionaries may need more time for only myself.

## Updated news

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
 - Working for latest dictionaty for 3.6 (2016c) now.
 - Tagged to 'dictionaryworking' until it complete written all dictionaries.
 - Added some tag state as like 'retired, trial, dicos, diconde'.

### 2014-11-06
 - updated dicom dictionary.
 - updated pixel information writing.
 - added dll project for exporting fuctions.
 - added dll testing project.