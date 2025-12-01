# UltralightUE

HTML/CSS/JavaScript UI integration for Unreal Engine using the Ultralight web renderer.

**Ultralight Inc. is in no way associated with this project in a professional matter. Support should be directed towards this Repo Author (@JailbreakPapa), and NOT WD Studios Corp.**

**This project includes Ultralight binaries & headers. BY USING THIS LIBRARY, YOU ACCEPT Ultralight's EULA.**

**Current Ultralight Version: 1.4.0 - stable**

---

## Table of Contents

- [Features](#features)
- [Supported Platforms](#supported-platforms)
- [Unreal Engine Versions](#unreal-engine-versions)
- [Installation](#installation)
- [Quick Start](#quick-start)
  - [C++ Example](#c-example)
  - [Blueprint Example](#blueprint-example)
- [API Reference](#api-reference)
  - [UUltralightSubsystem](#uultralightsubsystem)
  - [UUltralightView](#uultralightview)
  - [Input Handling](#input-handling)
- [Configuration](#configuration)
- [Directory Structure](#directory-structure)
- [Usage Examples](#usage-examples)
  - [Loading Local HTML Files](#loading-local-html-files)
  - [Loading Remote URLs](#loading-remote-urls)
  - [Dynamic HTML Content](#dynamic-html-content)
  - [Mouse and Keyboard Input](#mouse-and-keyboard-input)
  - [Resizing Views](#resizing-views)
- [Best Practices](#best-practices)
- [Troubleshooting](#troubleshooting)
- [Performance Considerations](#performance-considerations)
- [License](#license)

---

## Features

- **Modern Web Technologies**: Full HTML5, CSS3, and JavaScript (ES6+) support via Ultralight
- **Blueprint & C++ Support**: Complete Blueprint integration with clean C++ API
- **CPU Rendering**: No GPU driver required, works on all hardware configurations
- **Comprehensive Input**: Full mouse, keyboard, and scroll event forwarding
- **Transparent Backgrounds**: Native support for alpha-transparent UIs
- **UMG Integration**: Render targets work seamlessly with UMG widgets and materials
- **Hot Reload**: Supports Unreal's Live Coding for rapid iteration
- **Efficient Rendering**: Dirty rect optimization updates only changed regions
- **File System Bridge**: Automatic content loading from project and packaged builds
- **Cache Management**: Persistent cookies and localStorage support

---

## Supported Platforms

**Currently Supported:**
- Windows (64-bit)

**Planned Support:**
- macOS
- Linux
- Console platforms (NDA platforms can be added with minimal changes)

---

## Unreal Engine Versions

- **UE 5.3**
- **UE 5.5 and later**

Earlier versions may work but are untested.

---

## Installation

### 1. Add Plugin to Project

Copy the `UltralightUE` folder to your project's `Plugins/` directory:

```
YourProject/
└── Plugins/
    └── UltralightUE/
```

### 2. Enable Plugin

Open your project in Unreal Editor:
1. Go to **Edit → Plugins**
2. Search for "Ultralight"
3. Enable **UltralightUE**
4. Restart the editor when prompted

### 3. Module Dependencies (C++ Projects)

If using C++ code, add to your module's `.Build.cs`:

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "UltralightUE"  // Add this line
});
```

### 4. Verify Installation

The plugin will automatically:
- Load Ultralight DLLs on startup
- Create `Content/uicontent/` folder if missing
- Initialize logging to **Output Log → LogUltralightUE**

---

## Quick Start

### C++ Example

```cpp
#include "ULUESubsystem.h"
#include "Rendering/ULUEView.h"

void AMyActor::CreateWebUI()
{
    // Get subsystem
    UUltralightSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UUltralightSubsystem>();
    if (!Subsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("UltralightSubsystem not found"));
        return;
    }

    // Create view (1920x1080, transparent background, load example.com)
    UUltralightView* View = Subsystem->CreateView(
        1920,                           // Width
        1080,                           // Height
        true,                           // Transparent
        TEXT("https://example.com"),    // Initial URL
        nullptr                         // Auto-create render target
    );

    // Get render target for use in materials/UMG
    UTextureRenderTarget2D* RenderTarget = View->GetRenderTarget();

    // Use RenderTarget in material, widget, etc.
    MyMaterialInstance->SetTextureParameterValue(TEXT("WebUI"), RenderTarget);

    // Forward mouse input
    View->InjectMouseMove(FVector2D(100, 100));
    View->InjectMouseButton(FVector2D(100, 100), EULUEMouseButton::Left, true, false, false, false, false);

    // Forward keyboard input
    View->SetFocused(true);
    View->InjectKeyDown(EKeys::A, false, false, false, false, false);
    View->InjectChar(TEXT("a"), false, false, false, false);

    // Cleanup when done
    Subsystem->DestroyView(View);
}
```

### Blueprint Example

1. **Get Subsystem**:
   - `Get Game Instance` → `Get Subsystem (UltralightSubsystem)`

2. **Create View**:
   - Call `Create View` on subsystem
   - **Width**: 1920
   - **Height**: 1080
   - **Transparent**: true
   - **Initial URL**: `file:///index.html`
   - Save returned reference as variable

3. **Use Render Target**:
   - Call `Get Render Target` on view
   - Connect to **Image** widget's Brush → Texture
   - Or use in material parameter

4. **Forward Input** (in PlayerController):
   - On Mouse Move: `Inject Mouse Move`
   - On Mouse Button Down: `Inject Mouse Button` (bPressed = true)
   - On Mouse Button Up: `Inject Mouse Button` (bPressed = false)
   - On Key Down: `Set Focused` (true), then `Inject Key Down` and `Inject Char`

5. **Cleanup**:
   - Call `Destroy View` on subsystem when UI is no longer needed

---

## API Reference

### UUltralightSubsystem

Central manager for Ultralight renderer. Accessed via `GetGameInstance()->GetSubsystem<UUltralightSubsystem>()`.

#### Methods

**`CreateView`** (BlueprintCallable)
```cpp
UUltralightView* CreateView(
    int32 Width,
    int32 Height,
    bool bTransparent,
    FString InitialURL = TEXT(""),
    UTextureRenderTarget2D* ExistingTarget = nullptr
)
```
- **Returns**: New Ultralight view instance
- **Width/Height**: View dimensions in pixels
- **bTransparent**: Enable alpha transparency
- **InitialURL**: Optional starting URL (empty loads blank page)
- **ExistingTarget**: Optional pre-created render target (auto-created if null)

**`DestroyView`** (BlueprintCallable)
```cpp
void DestroyView(UUltralightView* View)
```
- **View**: View to destroy and release native resources
- Call this explicitly or let GC handle cleanup

**`GetResourceRoot`** (BlueprintPure)
```cpp
FString GetResourceRoot() const
```
- **Returns**: Path to plugin's internal resources directory

---

### UUltralightView

Wrapper around Ultralight view with rendering and input capabilities.

#### Content Loading

**`LoadURL`** (BlueprintCallable)
```cpp
void LoadURL(const FString& URL)
```
- **URL**: Load web content from URL
- Supports: `http://`, `https://`, `file:///`
- Example: `LoadURL(TEXT("https://google.com"))`

**`LoadHTML`** (BlueprintCallable)
```cpp
void LoadHTML(const FString& HTML, const FString& VirtualURL = TEXT("about:blank"))
```
- **HTML**: Raw HTML string to render
- **VirtualURL**: Base URL for relative resources
- Example: `LoadHTML(TEXT("<h1>Hello World</h1>"), TEXT("about:blank"))`

#### View Management

**`Resize`** (BlueprintCallable)
```cpp
void Resize(int32 Width, int32 Height)
```
- Dynamically resize view and render target
- **Warning**: Recreates surface, avoid frequent calls

**`SetFocused`** (BlueprintCallable)
```cpp
void SetFocused(bool bFocused)
```
- Set keyboard focus state
- **Required** before keyboard input works

**`GetRenderTarget`** (BlueprintPure)
```cpp
UTextureRenderTarget2D* GetRenderTarget() const
```
- **Returns**: Render target for use in materials/UMG
- Updates automatically each tick when view is dirty

---

### Input Handling

All input methods are **BlueprintCallable** in category **Ultralight | Input**.

#### Mouse Input

**`InjectMouseMove`**
```cpp
void InjectMouseMove(FVector2D Position)
```
- **Position**: Mouse coordinates in view space (0,0 = top-left)
- Call on every mouse move event

**`InjectMouseButton`**
```cpp
void InjectMouseButton(
    FVector2D Position,
    EULUEMouseButton Button,
    bool bPressed,
    bool bShift = false,
    bool bCtrl = false,
    bool bAlt = false,
    bool bMeta = false
)
```
- **Position**: Click coordinates
- **Button**: `None`, `Left`, `Middle`, `Right`
- **bPressed**: true = mouse down, false = mouse up
- **Modifiers**: Shift, Ctrl, Alt, Meta (Windows key)

#### Scroll Input

**`InjectScroll`**
```cpp
void InjectScroll(
    FVector2D ScrollDelta,
    bool bByPage = false,
    bool bShift = false,
    bool bCtrl = false,
    bool bAlt = false,
    bool bMeta = false
)
```
- **ScrollDelta**: Scroll amount (X = horizontal, Y = vertical)
- **bByPage**: true = page scroll, false = pixel scroll

#### Keyboard Input

**`InjectKeyDown`**
```cpp
void InjectKeyDown(
    FKey Key,
    bool bIsRepeat = false,
    bool bShift = false,
    bool bCtrl = false,
    bool bAlt = false,
    bool bMeta = false
)
```
- **Key**: Unreal FKey (automatically converts to virtual key code)
- **bIsRepeat**: Auto-repeat flag

**`InjectKeyUp`**
```cpp
void InjectKeyUp(
    FKey Key,
    bool bShift = false,
    bool bCtrl = false,
    bool bAlt = false,
    bool bMeta = false
)
```

**`InjectChar`**
```cpp
void InjectChar(
    const FString& Characters,
    bool bShift = false,
    bool bCtrl = false,
    bool bAlt = false,
    bool bMeta = false
)
```
- **Characters**: Typed text for input fields
- **Required** for proper text entry (call after `InjectKeyDown`)

---

## Configuration

### File System Paths

**Content Root**: `<Project>/Content/uicontent/`
- Place all HTML/CSS/JS/image files here
- Auto-created by subsystem if missing

**Resource Root**: `<Plugin>/Source/ThirdParty/UltralightUELibrary/resources/`
- Ultralight internal resources (cacert.pem, ICU data)
- **Do not modify**

**Cache Directory**: `<Project>/Saved/UltralightCache/`
- Stores cookies, localStorage, session data
- Add to `.gitignore`

### Required DLLs (Windows)

Located in `<Plugin>/Binaries/Win64/`:
- `Ultralight.dll`
- `UltralightCore.dll`
- `WebCore.dll`

Loaded automatically on plugin startup.

---

## Directory Structure

```
YourProject/
├── Content/
│   └── uicontent/              # Your HTML/CSS/JS files
│       ├── index.html
│       ├── styles.css
│       └── app.js
├── Saved/
│   └── UltralightCache/        # Runtime cache (auto-generated)
└── Plugins/
    └── UltralightUE/
        ├── Binaries/Win64/     # Ultralight DLLs
        ├── Source/
        │   ├── UltralightUE/   # Plugin source
        │   └── ThirdParty/
        │       └── UltralightUELibrary/
        │           └── resources/  # Ultralight internal resources
        └── UltralightUE.uplugin
```

---

## Usage Examples

### Loading Local HTML Files

**File**: `Content/uicontent/menu.html`
```html
<!DOCTYPE html>
<html>
<head>
    <style>
        body { background: transparent; color: white; font-family: Arial; }
        .button { padding: 20px; background: #007acc; cursor: pointer; }
    </style>
</head>
<body>
    <div class="button" onclick="alert('Clicked!')">Click Me</div>
</body>
</html>
```

**C++ Load**:
```cpp
View->LoadURL(TEXT("file:///menu.html"));
```

**Blueprint Load**:
- Node: `Load URL`
- URL: `file:///menu.html`

---

### Loading Remote URLs

```cpp
// HTTPS
View->LoadURL(TEXT("https://google.com"));

// HTTP
View->LoadURL(TEXT("http://example.com"));
```

**Note**: Requires internet connection. Use local files for offline support.

---

### Dynamic HTML Content

```cpp
FString DynamicHTML = FString::Printf(TEXT(R"(
<!DOCTYPE html>
<html>
<head>
    <style>
        body { background: rgba(0,0,0,0.8); color: white; }
        h1 { font-size: 48px; }
    </style>
</head>
<body>
    <h1>Player: %s</h1>
    <p>Score: %d</p>
</body>
</html>
)"), *PlayerName, PlayerScore);

View->LoadHTML(DynamicHTML, TEXT("about:blank"));
```

---

### Mouse and Keyboard Input

**PlayerController C++**:
```cpp
void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("LeftClick", IE_Pressed, this, &AMyPlayerController::OnLeftClickPressed);
    InputComponent->BindAction("LeftClick", IE_Released, this, &AMyPlayerController::OnLeftClickReleased);
    InputComponent->BindAxis("MouseX", this, &AMyPlayerController::OnMouseX);
    InputComponent->BindAxis("MouseY", this, &AMyPlayerController::OnMouseY);
}

void AMyPlayerController::OnMouseX(float Value)
{
    MousePosition.X += Value;
    if (UltralightView)
        UltralightView->InjectMouseMove(MousePosition);
}

void AMyPlayerController::OnLeftClickPressed()
{
    if (UltralightView)
        UltralightView->InjectMouseButton(MousePosition, EULUEMouseButton::Left, true);
}

void AMyPlayerController::OnLeftClickReleased()
{
    if (UltralightView)
        UltralightView->InjectMouseButton(MousePosition, EULUEMouseButton::Left, false);
}
```

**Blueprint**:
1. `Event Tick` → Calculate mouse position in view space
2. Call `Inject Mouse Move` with position
3. On click: Call `Inject Mouse Button` with `bPressed = true/false`

---

### Resizing Views

```cpp
// Create small view
UUltralightView* View = Subsystem->CreateView(800, 600, true);

// Resize to fullscreen later
View->Resize(1920, 1080);
```

**Warning**: Resizing recreates the surface. Minimize resize frequency.

---

## Best Practices

### 1. Content Organization
- ✅ Keep all UI files in `Content/uicontent/`
- ✅ Use relative paths in HTML/CSS (`./images/logo.png`)
- ✅ Organize by feature (`uicontent/menus/`, `uicontent/hud/`)

### 2. Performance
- ✅ Use reasonable resolutions (≤ 1920x1080 for 60fps)
- ✅ Enable transparency only when needed
- ✅ Destroy views when not visible
- ❌ Don't create/destroy views every frame
- ❌ Don't resize frequently

### 3. Input Handling
- ✅ Call `SetFocused(true)` before keyboard input
- ✅ Use `InjectChar` for text input (not just `InjectKeyDown`)
- ✅ Convert mouse positions to view-space coordinates
- ❌ Don't inject input to unfocused views

### 4. Focus Management
```cpp
// When UI becomes active
View->SetFocused(true);
View->InjectMouseMove(FVector2D(960, 540)); // Center cursor

// When UI closes
View->SetFocused(false);
```

### 5. Lifecycle Management
```cpp
// Store view as UPROPERTY for GC protection
UPROPERTY()
UUltralightView* MenuView;

// Cleanup in destructor or explicit close
void AMyGameMode::CloseMenu()
{
    if (MenuView)
    {
        UUltralightSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UUltralightSubsystem>();
        Subsystem->DestroyView(MenuView);
        MenuView = nullptr;
    }
}
```

### 6. Transparency Usage
```cpp
// Opaque background (better performance)
UUltralightView* HUD = Subsystem->CreateView(1920, 1080, false);

// Transparent (required for alpha blending)
UUltralightView* Overlay = Subsystem->CreateView(1920, 1080, true);
```

---

## Troubleshooting

### View Not Displaying

**Check**:
1. Is render target assigned to material/widget?
2. Is view loading content? (check Output Log → LogUltralightUE)
3. Are files in `Content/uicontent/`?
4. Is URL correct? Use `file:///` for local files

**Solution**:
```cpp
// Verify render target
UTextureRenderTarget2D* RT = View->GetRenderTarget();
UE_LOG(LogTemp, Warning, TEXT("Render Target Valid: %s"), RT ? TEXT("Yes") : TEXT("No"));

// Check content loading
View->LoadURL(TEXT("file:///index.html"));
// Look for errors in Output Log
```

### Input Not Working

**Check**:
1. Is `SetFocused(true)` called?
2. Are mouse coordinates in view space (0,0 to Width,Height)?
3. Is view receiving events? (add logging)

**Solution**:
```cpp
// Enable focus
View->SetFocused(true);

// Verify coordinates
UE_LOG(LogTemp, Warning, TEXT("Mouse: %s"), *MousePos.ToString());

// Test with simple input
View->InjectMouseMove(FVector2D(100, 100));
View->InjectMouseButton(FVector2D(100, 100), EULUEMouseButton::Left, true);
```

### Content Not Loading

**Check**:
1. File exists in `Content/uicontent/`?
2. Correct file path format?
3. MIME type supported?

**Solution**:
```cpp
// Use absolute path for debugging
FString FullPath = FPaths::ProjectContentDir() + TEXT("uicontent/index.html");
bool bExists = FPaths::FileExists(FullPath);
UE_LOG(LogTemp, Warning, TEXT("File exists: %s"), bExists ? TEXT("Yes") : TEXT("No"));

// Try loading HTML directly
View->LoadHTML(TEXT("<h1>Test</h1>"), TEXT("about:blank"));
```

### DLL Load Errors

**Check**:
1. Are DLLs in `Plugins/UltralightUE/Binaries/Win64/`?
2. Correct Unreal version?
3. Check Output Log for specific DLL name

**Solution**:
- Verify plugin is enabled in Project Settings → Plugins
- Regenerate project files (right-click `.uproject` → Generate Visual Studio project files)
- Verify Windows 64-bit build

### Performance Issues

**Check**:
1. View resolution too high?
2. Multiple views active?
3. Complex CSS animations?

**Solution**:
```cpp
// Reduce resolution
View->Resize(1280, 720);

// Destroy unused views
Subsystem->DestroyView(UnusedView);

// Simplify CSS (avoid heavy animations, filters)
```

---

## Performance Considerations

### CPU Rendering Path
- UltralightUE uses CPU rendering (no GPU driver)
- Performance scales with resolution and complexity
- Typical targets:
  - **1920x1080**: 60fps with moderate CSS
  - **1280x720**: 60fps with complex CSS/animations
  - **3840x2160**: 30fps (not recommended)

### Optimization Tips

1. **Resolution**:
   - Start with 1280x720 and scale up as needed
   - Use lower resolution for background UIs

2. **View Count**:
   - Minimize active views (≤ 3 concurrent)
   - Destroy views when hidden

3. **CSS Performance**:
   - Avoid heavy box-shadow, blur, filters
   - Use transform animations (GPU-accelerated in browsers)
   - Minimize repaints with `will-change` CSS property

4. **Content Loading**:
   - Preload frequently used views
   - Use local files over remote URLs
   - Cache dynamic HTML generation

5. **Dirty Rect Optimization**:
   - Plugin automatically uses dirty rects
   - Only changed regions upload to GPU
   - Static content renders once

---

## License

### UltralightUE Plugin

**MIT License**

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

---

### Ultralight Library

Ultralight (C), "Ultralight UI" are trademarks of **Ultralight Inc.** All Rights Reserved.

**Ultralight is NOT licensed under MIT**. It has separate licensing terms.

**Pricing Information**: https://ultralig.ht/#pricing

By using UltralightUE, you agree to comply with Ultralight's EULA and licensing requirements. Please review their pricing and licensing terms before commercial use.

---

## Documentation

Detailed documentation is being developed in the repository's Wiki.

**Current Topics**:
- Advanced input handling
- JavaScript ↔ C++ communication (planned)
- Custom file system providers
- Performance profiling
- Packaging for distribution

---

## Support

For issues, questions, or contributions:
- **GitHub Issues**: [Report bugs or request features](https://github.com/JailbreakPapa/UltralightUE/issues)
- **Author**: @JailbreakPapa
- **Ultralight**: Do NOT contact Ultralight Inc. for plugin support

---

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request with clear description

Follow Unreal Engine coding standards and include documentation for new features.

---

## Credits

- **Plugin Author**: Mikael Aboagye (@JailbreakPapa)
- **Ultralight Engine**: Ultralight Inc.
- **Community Contributors**: See GitHub contributors

---

**Happy coding!** Build amazing web-powered UIs in Unreal Engine with UltralightUE.
