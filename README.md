# UltralightUE
Ultralight UI Support for Unreal Engine 4.27.2 +.

**Ultralight Inc. is in no way assosiated with this project in a professional matter. support should be directed towards this Repo Author (@JailbreakPapa) , and NOT WD Studios Corp.**

**This Project includes Ultralight Binaries & Includes. BY USING THIS LIBRARY, YOU ACCEPT Ultralight's EULA.**

**Current Supported Ultralight Version: 1.4.0 - stable**

# Unreal Versions

Supported Unreal Engine versions:
- 5.3
- 5.5 and Up.

# Documentation

Documentation is currently being worked on, but will be contained within the repo's Wiki.

# Supported Platforms

Currently Supported Platforms with the plugin are: Windows. 

*Please Note*: Any platform, NDA, or not, can be added with the plugin with little, to no changes. 

# License

Ultralight ***IS NOT*** Licensed under MIT, it has its own licensing terms that users will have to abide by when using the library and/or plugin.

For more information, please read pricing information: (https://ultralig.ht/#pricing)

UltralightUE is Licensed under the very-permissive MIT License.

```
MIT License

Copyright (c) 2023 Mikael Aboagye & Ultralight Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Ultralight (C), "Ultralight UI" are trademarks of Ultralight Inc. All Rights Reserved.
```

# UE5 Runtime Integration (added)

- Ultralight DLLs are loaded automatically by the plugin module.
- `UUltralightSubsystem` owns the renderer, ticks it via FTSTicker, and exposes `CreateView`.
- `UUltralightView` wraps an Ultralight view and renders into a `UTextureRenderTarget2D`.
- Input helpers are provided on `UUltralightView` for mouse/keyboard/scroll forwarding.
- `ULUEFileSystem` now understands a UI content root and a dedicated resource root for Ultralight data.

## Quick start

1) Place your UI files under `Content/uicontent` (the subsystem creates the folder if missing).  
2) Make sure the plugin's packaged resources stay untouched (`Plugins/UltralightUE/Source/ThirdParty/UltralightUELibrary/resources`).  
3) From code or Blueprint, get `UltralightSubsystem` (`GetGameInstanceSubsystem`).  
4) Call `CreateView(Width, Height, bTransparent, InitialURL)`; keep the returned `UUltralightView`.  
5) Use `GetRenderTarget()` from the view in a material/UMG brush.  
6) Forward input: `InjectMouseMove`, `InjectMouseButton`, `InjectScroll`, `InjectKeyDown/Up`, `InjectChar`.  
7) Call `DestroyView` on the subsystem or let the UObject be GC'd when done.

## Rendering details

- CPU renderer path is used (no GPU driver required) and surfaces are copied into the render target each tick.
- Dirty rects are respected via Ultralight surfaces; uploads only happen when the view reports changes.
- Cache directory: `<Project>/Saved/UltralightCache`.
