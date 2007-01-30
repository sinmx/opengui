// OpenGUI (http://opengui.sourceforge.net)
// This source code is released under the BSD License
// See LICENSE.TXT for details

#ifndef C9F536DD_7BC8_4f35_BE1C_8CC4548BFCDC
#define C9F536DD_7BC8_4f35_BE1C_8CC4548BFCDC

/*
    The purpose of this include file is to identify the system type and
    compiler that is being used. This is required for certain system
    specific function calls and includes.

    If your platform and/or compiler are not listed here, then you will
    have to port OpenGUI to your environment. If you're porting OpenGUI
    to a new target, please put your platform/compiler detection updates
    in this file.
*/


//Initial definitions of known platforms and compilers
#define OGL_PLATFORM_WIN32 1
#define OGL_PLATFORM_LINUX 2

#define OGL_COMPILER_MSVC 1
#define OGL_COMPILER_GCC  2




/* Detect compiler */
//check for MSVC
#if defined ( _MSC_VER )
#define OGL_COMPILER OGL_COMPILER_MSVC
//	#pragma message( "Detected Compiler: MSVC" )

//check for GCC
#elif defined ( __GNUC__ )
#define OGL_COMPILER OGL_COMPILER_GCC

#else
#pragma error "Unknown compiler! Refer to OGL_Platform.h"

#endif


/* Detect Platform */
//checking for win32 is easy
#if defined( __WIN32__ ) || defined( _WIN32 )
#define OGL_PLATFORM OGL_PLATFORM_WIN32
//	#pragma message( "Detected Platform: WIN32" )

//assume Linux if nothing else fits
#else
#define OGL_PLATFORM OGL_PLATFORM_LINUX

#endif

#if defined ( _DEBUG )
#define OGL_DEBUG
#endif

#if OGL_COMPILER == OGL_COMPILER_MSVC
#pragma warning (disable : 4251) // This warning can be disregarded
//#pragma warning(disable : 4996) // for now, ignore the deprecation warnings on the STL
#define strcmpi _strcmpi // VC8 doesn't like POSIX style strcmpi
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE // VC8 also doesn't like a lot of standard C library functions
#endif
#endif // OGL_COMPILER == OGL_COMPILER_MSVC

#endif // C9F536DD_7BC8_4f35_BE1C_8CC4548BFCDC
