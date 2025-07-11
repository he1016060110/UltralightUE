/*
 *   Copyright (c) 2023 Mikael Aboagye & Ultralight Inc.
 *   All rights reserved.

 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

#if defined _WIN32 || defined _WIN64
#define ULTRALIGHTUE_IMPORT __declspec(dllimport)
#elif defined __linux__
#define ULTRALIGHTUELIBRARY_IMPORT __attribute__((visibility("default")))
#else
#define ULTRALIGHTUELIBRARY_IMPORT
#endif

#if defined _WIN32 || defined _WIN64
#include <Windows.h> // This is generally acceptable in a ThirdParty context if needed by the library itself
                     // but usually, UE code tries to avoid direct Windows.h includes in public headers.
                     // If only for dllexport/dllimport, it might be avoidable.

#define ULTRALIGHTUELIBRARY_EXPORT __declspec(dllexport)
#else
// #include <stdio.h> // This include seems unlikely to be needed for export macros. Removed.
#endif

#ifndef ULTRALIGHTUELIBRARY_EXPORT
#define ULTRALIGHTUELIBRARY_EXPORT
#endif

// It's crucial that ULTRALIGHTUE_API (for the UltralightUE module) is NOT defined here.
// UBT will generate it based on the module's .Build.cs and .uplugin settings.
// Any definition here would conflict.