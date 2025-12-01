/*
 *   UltralightUE - Ultralight header wrapper
 *   
 *   This file wraps Ultralight includes to prevent macro conflicts with UE.
 *   Include this file in .cpp files AFTER all UE includes.
 */

#pragma once

// Push UE macros that might conflict
#pragma push_macro("Type")
#pragma push_macro("TEXT")
#pragma push_macro("PI")

#undef Type
#undef TEXT
#undef PI

// Include Ultralight headers
#include <Ultralight/Ultralight.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/FileSystem.h>
#include <Ultralight/platform/Logger.h>
#include <Ultralight/Bitmap.h>
#include <Ultralight/View.h>
#include <Ultralight/Renderer.h>
#include <Ultralight/Buffer.h>
#include <Ultralight/Listener.h>
#include <Ultralight/MouseEvent.h>
#include <Ultralight/KeyEvent.h>
#include <Ultralight/ScrollEvent.h>

// Pop UE macros back
#pragma pop_macro("PI")
#pragma pop_macro("TEXT")
#pragma pop_macro("Type")

