#libtinydicom


## Introduce
 - A DICOM tag library for using C++/STL
 - Related in DICOM tag file (.DCM) read and write library is too far to get free. So I made this to read and write DCM files for free and tiny ! It is still prototype and need to be enhanced.

## Project target
 - Basically this project targeted to has non-platform-dependency.
 - But current is using to Code::Blocks project file (CBP) and targeted for Windows32.

## Known VR (WORD)
 - See source code of dicomdictinary.cpp.
 - I have gathering more VR information, but it is really narrow to know. Absolutely limited.

## Updated news
### 2014-11-06
 - updated dicom dictionary.
 - updated pixel information writing.
 - added dll project for exporting fuctions.
 - added dll testing project.