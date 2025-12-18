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
#include "UltralightUE.h"
#include "ULUELogInterface.h"
#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "ThirdParty/UltralightUELibrary/ULUELibrary.h"



#define LOCTEXT_NAMESPACE "FUltralightUEModule"

void FUltralightUEModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("UltralightUE")->GetBaseDir();

	// Add on the relative location of the ultralight dll(s) and load them.
#if PLATFORM_WINDOWS
	// First try plugin Binaries directory (for editor)
	WebCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/WebCore.dll"));
	UltralightCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/UltralightCore.dll"));
	UltralightLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/Ultralight.dll"));
	
	// If not found, try executable directory (for packaged builds)
	if (!FPaths::FileExists(WebCoreLibraryPath))
	{
		FString ExeDir = FPlatformProcess::GetModulesDirectory();
		WebCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("WebCore.dll"));
		UltralightCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("UltralightCore.dll"));
		UltralightLibraryPath = FPaths::Combine(*ExeDir, TEXT("Ultralight.dll"));
	}
	
	// Also try the directory where the executable is located
	if (!FPaths::FileExists(WebCoreLibraryPath))
	{
		FString ExeDir = FPaths::GetPath(FPlatformProcess::ExecutablePath());
		WebCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("WebCore.dll"));
		UltralightCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("UltralightCore.dll"));
		UltralightLibraryPath = FPaths::Combine(*ExeDir, TEXT("Ultralight.dll"));
	}
#elif PLATFORM_MAC
	// First try plugin Source directory (for editor)
	WebCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libWebCore.dylib"));
	UltralightCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libUltralightCore.dylib"));
	UltralightLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libUltralight.dylib"));
	AppCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libAppCore.dylib"));
	
	// If not found, try executable directory (for packaged builds)
	if (!FPaths::FileExists(WebCoreLibraryPath))
	{
		FString ExeDir = FPlatformProcess::GetModulesDirectory();
		WebCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libWebCore.dylib"));
		UltralightCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libUltralightCore.dylib"));
		UltralightLibraryPath = FPaths::Combine(*ExeDir, TEXT("libUltralight.dylib"));
		AppCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libAppCore.dylib"));
	}
	
	// Also try the directory where the executable is located
	if (!FPaths::FileExists(WebCoreLibraryPath))
	{
		FString ExeDir = FPaths::GetPath(FPlatformProcess::ExecutablePath());
		WebCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libWebCore.dylib"));
		UltralightCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libUltralightCore.dylib"));
		UltralightLibraryPath = FPaths::Combine(*ExeDir, TEXT("libUltralight.dylib"));
		AppCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libAppCore.dylib"));
	}
#elif PLATFORM_LINUX
	// First try plugin Source directory (for editor)
	WebCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libWebCore.so"));
	UltralightCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libUltralightCore.so"));
	UltralightLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libUltralight.so"));
	AppCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libAppCore.so"));
	
	// If not found, try executable directory (for packaged builds)
	if (!FPaths::FileExists(WebCoreLibraryPath))
	{
		FString ExeDir = FPlatformProcess::GetModulesDirectory();
		WebCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libWebCore.so"));
		UltralightCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libUltralightCore.so"));
		UltralightLibraryPath = FPaths::Combine(*ExeDir, TEXT("libUltralight.so"));
		AppCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libAppCore.so"));
	}
	
	// Also try the directory where the executable is located
	if (!FPaths::FileExists(WebCoreLibraryPath))
	{
		FString ExeDir = FPaths::GetPath(FPlatformProcess::ExecutablePath());
		WebCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libWebCore.so"));
		UltralightCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libUltralightCore.so"));
		UltralightLibraryPath = FPaths::Combine(*ExeDir, TEXT("libUltralight.so"));
		AppCoreLibraryPath = FPaths::Combine(*ExeDir, TEXT("libAppCore.so"));
	}
#endif // PLATFORM_WINDOWS
	/// Assign handles to Library(s) path(s).
	WebCoreHandle = !WebCoreLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*WebCoreLibraryPath) : nullptr;
	UltralightCoreHandle = !UltralightCoreLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*UltralightCoreLibraryPath) : nullptr;
	UltralightHandle = !UltralightLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*UltralightLibraryPath) : nullptr;
	
#if PLATFORM_MAC || PLATFORM_LINUX
	AppCoreHandle = !AppCoreLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*AppCoreLibraryPath) : nullptr;
	// On Mac/Linux, AppCore is optional but recommended
	if (WebCoreHandle && UltralightCoreHandle && UltralightHandle)
	{
		// Startup Ultralight engine.
		ultralightue::NotifyUltralightUEStartup();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("UltralightUE: Error", "Failed to load UltralightUE! Please check the log for any messages. if you cant fix the issue, create a issue on github! (https://github.com/JailbreakPapa/UltralightUE)"));
	}
#else
	if (WebCoreHandle && UltralightCoreHandle && UltralightHandle)
	{
		// Startup Ultralight engine.
		ultralightue::NotifyUltralightUEStartup();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("UltralightUE: Error", "Failed to load UltralightUE! Please check the log for any messages. if you cant fix the issue, create a issue on github! (https://github.com/JailbreakPapa/UltralightUE)"));
	}
#endif
}

void FUltralightUEModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handles to ultralight dll(s).
	DestroyUltralightHandles();
}

bool FUltralightUEModule::CheckUltralightResources(FPakFile& p_resourcepak)
{
	if (p_resourcepak.GetIsMounted() && p_resourcepak.FindPrunedDirectory(TEXT("uicontent")))
	{
		/// Looks like we were able to get all needed resources.
		return true;
	}
	return false;
}

bool FUltralightUEModule::CheckUltralightResources(FString& path)
{
	/// First get the content directory, and check if uiresources are listed there.
	if (path.IsEmpty())
	{
		path = FPaths::ProjectContentDir() / TEXT("uicontent");
	}
	if (FPaths::DirectoryExists(path))
	{
		/// Looks like we were able to get all needed resources.
		return true;
	}
	
	if (GetLogInterface())
	{
		GetLogInterface()->LogWarning(TEXT("UltralightUE: Failed to find UIContent! "));
	}
	return false;
}

void FUltralightUEModule::SetLoggingInterface(ultralightue::ULUELogInterface& in_logginginterface)
{
	m_loginterface = &in_logginginterface;
}

ultralightue::ULUELogInterface* FUltralightUEModule::GetLogInterface() const
{
    return static_cast<ultralightue::ULUELogInterface*>(m_loginterface);
}

void FUltralightUEModule::DestroyUltralightHandles()
{
	FPlatformProcess::FreeDllHandle(UltralightHandle);
	FPlatformProcess::FreeDllHandle(UltralightCoreHandle);
	FPlatformProcess::FreeDllHandle(WebCoreHandle);
#if PLATFORM_MAC || PLATFORM_LINUX
	FPlatformProcess::FreeDllHandle(AppCoreHandle);
	AppCoreHandle = nullptr;
#endif
	UltralightHandle = nullptr;
	UltralightCoreHandle = nullptr;
	WebCoreHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUltralightUEModule, UltralightUE)