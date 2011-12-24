/*
 * This file wraps header file in google-gflags, so you can include this file
 * instead of "third_party\google-gflags\src\windows\gflags\gflags.h". Here is why.
 *
 * The origin project file in google-gflags is for dll version. If we use it to generate
 * static library(.lib), we must declare three macros, not only when we build the lib file, 
 * but also we USE the lib file. In the purpose of usability, we add this file.
 */


#ifndef BASE_GFLAGS_H__
#define BASE_GFLAGS_H__

//Add three macros, see README_windows.txt(in third_party\google-gflags) for detail.
#define GFLAGS_DLL_DECL
#define GFLAGS_DLL_DECLARE_FLAG
#define GFLAGS_DLL_DEFINE_FLAG
#include "gflags/gflags.h"

#endif // #ifndef BASE_GFLAGS_H__
