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

using System.IO;
using UnrealBuildTool;

public class UltralightUELibrary : ModuleRules
{
    /// NOTE: If you do have access, feel free to add console/NDA based platforms here as well. you can also contact: contact@wdstudios.tech for access as well.
    // Credit for these two functions, that were added by a very nice UEForms user: https://forums.unrealengine.com/t/how-to-modify-build-file-to-copy-dlls-to-binaries/353587

    public string GetUProjectPath()
    {
        // ModuleDirectory = Plugins\UltralightUE\Source\ThirdParty\UltralightUELibrary
        // Need to go up 5 levels to reach project root
        return Directory.GetParent(ModuleDirectory).Parent.Parent.Parent.Parent.ToString();
    }

    private void CopyToBinaries(string Filepath, ReadOnlyTargetRules Target)
    {
        string binariesDir = "";
        string filename = "";

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            binariesDir = Path.Combine(GetUProjectPath(), "Binaries", "Win64");
            filename = Path.GetFileName(Filepath);
        }

        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            binariesDir = Path.Combine(GetUProjectPath(), "Mac", "Release");
            filename = Path.GetFileName(Filepath);
        }

        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            binariesDir = Path.Combine(GetUProjectPath(), "Linux", "x86_64-unknown-linux-gnu");
            filename = Path.GetFileName(Filepath);
        }

        if (!Directory.Exists(binariesDir))
            Directory.CreateDirectory(binariesDir);

        // Always copy to ensure DLLs are up to date
        File.Copy(Filepath, Path.Combine(binariesDir, filename), true);
    }
    public UltralightUELibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string PlatformDir = "Win64";
            string SrcDir = Path.Combine(ModuleDirectory, PlatformDir);

            PublicAdditionalLibraries.AddRange(new string[] {
                Path.Combine(SrcDir, "AppCore.lib"),
                Path.Combine(SrcDir, "WebCore.lib"),
                Path.Combine(SrcDir, "UltralightCore.lib"),
                Path.Combine(SrcDir, "Ultralight.lib")
            });

            // DelayLoadDLLs - just filenames, DLLs will be in same directory as executable
            PublicDelayLoadDLLs.AddRange(new string[] {
                "AppCore.dll",
                "WebCore.dll",
                "Ultralight.dll",
                "UltralightCore.dll"
            });

            // Copy DLLs to multiple locations for different use cases
            string PluginBinDir = Path.Combine(GetUProjectPath(), "Plugins", "UltralightUE", "Binaries", PlatformDir);
            string ProjectBinDir = Path.Combine(GetUProjectPath(), "Binaries", PlatformDir);
            
            if (!Directory.Exists(PluginBinDir))
                Directory.CreateDirectory(PluginBinDir);
            if (!Directory.Exists(ProjectBinDir))
                Directory.CreateDirectory(ProjectBinDir);

            // DLLs and resources from Win64 directory
            string[] Win64Files = { "AppCore.dll", "WebCore.dll", "UltralightCore.dll", "Ultralight.dll", "inspector_resources.pak", "cacert.pem" };
            // Resources from resources directory
            string ResourceDir = Path.Combine(ModuleDirectory, "resources");
            string[] ResourceOnlyFiles = { "icudt67l.dat" };

            // Copy and register Win64 files
            foreach (string file in Win64Files)
            {
                string srcPath = Path.Combine(SrcDir, file);
                if (File.Exists(srcPath))
                {
                    try
                    {
                        File.Copy(srcPath, Path.Combine(PluginBinDir, file), true);
                        File.Copy(srcPath, Path.Combine(ProjectBinDir, file), true);
                    }
                    catch { /* Ignore copy errors during build */ }
                }
                RuntimeDependencies.Add(Path.Combine("$(TargetOutputDir)", file), srcPath, StagedFileType.NonUFS);
            }

            // Copy and register resource files (like ICU data)
            foreach (string file in ResourceOnlyFiles)
            {
                string srcPath = Path.Combine(ResourceDir, file);
                if (File.Exists(srcPath))
                {
                    try
                    {
                        File.Copy(srcPath, Path.Combine(PluginBinDir, file), true);
                        File.Copy(srcPath, Path.Combine(ProjectBinDir, file), true);
                    }
                    catch { /* Ignore copy errors during build */ }
                }
                RuntimeDependencies.Add(Path.Combine("$(TargetOutputDir)", file), srcPath, StagedFileType.NonUFS);
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            string PlatformDir = "Mac";
            // For Mac, dylibs are often placed in a "Frameworks" or "Resources" directory within the app bundle,
            // or directly alongside the executable. The CopyToBinaries logic might need adjustment based on how Mac bundles are structured by UE.
            // Assuming dylibs are copied to a location where they can be found by rpath or DYLD_LIBRARY_PATH.
            // The original script copied them to "Mac/Release" relative to UProject path, which might be outside the bundle.
            // For simplicity, we'll keep a similar structure for RuntimeDependencies pointing to plugin's own binary staging area.

            string DylibDir = Path.Combine(ModuleDirectory, PlatformDir, "Release"); // As per original script for source location

            PublicAdditionalLibraries.AddRange(new string[] {
                Path.Combine(DylibDir, "libAppCore.dylib"),
                Path.Combine(DylibDir, "libWebCore.dylib"),
                Path.Combine(DylibDir, "libUltralightCore.dylib"),
                Path.Combine(DylibDir, "libUltralight.dylib")
            });

            // PublicDelayLoadDLLs might not be the best way for dylibs on Mac, but kept for consistency if it works.
            // Often, dylibs are linked directly and @rpath is used.
            PublicDelayLoadDLLs.AddRange(new string[] {
                Path.Combine("$(PluginDir)", "Source", "ThirdParty", "UltralightUELibrary", PlatformDir, "Release", "libAppCore.dylib"),
                Path.Combine("$(PluginDir)", "Source", "ThirdParty", "UltralightUELibrary", PlatformDir, "Release", "libWebCore.dylib"),
                Path.Combine("$(PluginDir)", "Source", "ThirdParty", "UltralightUELibrary", PlatformDir, "Release", "libUltralightCore.dylib"),
                Path.Combine("$(PluginDir)", "Source", "ThirdParty", "UltralightUELibrary", PlatformDir, "Release", "libUltralight.dylib")
            });

            // RuntimeDependencies.AddRange(new RuntimeDependency[] {
            //     new RuntimeDependency(Path.Combine(DylibDir, "libWebCore.dylib")),
            //     new RuntimeDependency(Path.Combine(DylibDir, "libUltralightCore.dylib")),
            //     new RuntimeDependency(Path.Combine(DylibDir, "libUltralight.dylib")),
            //     new RuntimeDependency(Path.Combine(ModuleDirectory, "resources", "inspector_resources.pak")), // Assuming resources are platform-agnostic or also in Mac/Release
            //     new RuntimeDependency(Path.Combine(ModuleDirectory, "resources", "cacert.pem"))
            // });
            RuntimeDependencies.Add(Path.Combine(DylibDir, "libAppCore.dylib"));
            RuntimeDependencies.Add(Path.Combine(DylibDir, "libWebCore.dylib"));
            RuntimeDependencies.Add(Path.Combine(DylibDir, "libUltralightCore.dylib"));
            RuntimeDependencies.Add(Path.Combine(DylibDir, "libUltralight.dylib"));
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "resources", "cacert.pem"));

            // The CopyToBinaries function for Mac might need to target the correct location within the .app bundle
            // For now, it copies to <UProjectPath>/Mac/Release as per original.
            CopyToBinaries(Path.Combine(DylibDir, "libAppCore.dylib"), Target);
            CopyToBinaries(Path.Combine(DylibDir, "libWebCore.dylib"), Target);
            CopyToBinaries(Path.Combine(DylibDir, "libUltralightCore.dylib"), Target);
            CopyToBinaries(Path.Combine(DylibDir, "libUltralight.dylib"), Target);
            // Pak and pem files for Mac - assuming they are in a common resources folder or also in Mac/Release
            // The original script didn't copy these for Mac, so we'll add them assuming they are needed.
            // Their location might be different, this is an assumption based on Win64.
            CopyToBinaries(Path.Combine(ModuleDirectory, "resources", "cacert.pem"), Target);

            // Inspector assets are optional (depends on SDK distribution).
            string InspectorPakPath = Path.Combine(ModuleDirectory, "resources", "inspector_resources.pak");
            if (File.Exists(InspectorPakPath))
            {
                RuntimeDependencies.Add(InspectorPakPath);
                CopyToBinaries(InspectorPakPath, Target);
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            string PlatformDir = "Linux";
            string ArchDir = "x86_64-unknown-linux-gnu"; // As per original
            string SoDir = Path.Combine(ModuleDirectory, PlatformDir, ArchDir);
            string PluginBinariesSoDir = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "UltralightUELibrary", PlatformDir, ArchDir);


            PublicAdditionalLibraries.AddRange(new string[] {
                Path.Combine(SoDir, "libAppCore.so"),
                Path.Combine(SoDir, "libWebCore.so"),
                Path.Combine(SoDir, "libUltralightCore.so"),
                Path.Combine(SoDir, "libUltralight.so")
            });

            // PublicDelayLoadDLLs is primarily a Windows concept. For Linux, linking is typically direct.
            // However, UBT might handle this. Keeping for consistency if it has an effect.
            PublicDelayLoadDLLs.AddRange(new string[] {
                Path.Combine(PluginBinariesSoDir, "libAppCore.so"),
                Path.Combine(PluginBinariesSoDir, "libWebCore.so"),
                Path.Combine(PluginBinariesSoDir, "libUltralightCore.so"),
                Path.Combine(PluginBinariesSoDir, "libUltralight.so")
            });

            // RuntimeDependencies.AddRange(new RuntimeDependency[] {
            //     new RuntimeDependency(Path.Combine(SoDir, "libWebCore.so")),
            //     new RuntimeDependency(Path.Combine(SoDir, "libUltralightCore.so")),
            //     new RuntimeDependency(Path.Combine(SoDir, "libUltralight.so")),
            //     new RuntimeDependency(Path.Combine(ModuleDirectory, "resources", "inspector_resources.pak")),
            //     new RuntimeDependency(Path.Combine(ModuleDirectory, "resources", "cacert.pem"))
            // });
            RuntimeDependencies.Add(Path.Combine(SoDir, "libAppCore.so"));
            RuntimeDependencies.Add(Path.Combine(SoDir, "libWebCore.so"));
            RuntimeDependencies.Add(Path.Combine(SoDir, "libUltralightCore.so"));
            RuntimeDependencies.Add(Path.Combine(SoDir, "libUltralight.so"));
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "resources", "cacert.pem"));

            CopyToBinaries(Path.Combine(SoDir, "libAppCore.so"), Target);
            CopyToBinaries(Path.Combine(SoDir, "libWebCore.so"), Target);
            CopyToBinaries(Path.Combine(SoDir, "libUltralightCore.so"), Target);
            CopyToBinaries(Path.Combine(SoDir, "libUltralight.so"), Target); // Original had Ultralight.so, assuming libUltralight.so
            CopyToBinaries(Path.Combine(ModuleDirectory, "resources", "cacert.pem"), Target);

            // Inspector assets are optional (depends on SDK distribution).
            string InspectorPakPath = Path.Combine(ModuleDirectory, "resources", "inspector_resources.pak");
            if (File.Exists(InspectorPakPath))
            {
                RuntimeDependencies.Add(InspectorPakPath);
                CopyToBinaries(InspectorPakPath, Target);
            }
        }

        // Shared resources needed by Ultralight (ICU data and SSL certs)
        RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "resources", "icudt67l.dat"));
        RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "resources", "cacert.pem"));
    }
}
