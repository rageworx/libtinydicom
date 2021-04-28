#ifndef __TESTMAIN_H__
#define __TESTMAIN_H__

#ifdef _WIN32
#include <windows.h>
#include <wchar.h>
#endif /// of _WIN32

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef __APPLE__
#include <io.h>
#endif /// of __APPLE__
#include "libdcm.h"

#endif // of __TESTMAIN_H__
