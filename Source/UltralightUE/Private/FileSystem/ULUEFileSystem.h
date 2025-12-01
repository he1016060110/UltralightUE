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

#pragma once

// CoreMinimal must be first for UE projects
#include "CoreMinimal.h"

// Include Ultralight headers with macro protection
#include "ULUEUltralightIncludes.h"

namespace ultralightue
{
    /// Filesystem access pattern
    enum class FSAccess : uint8
    {
        FSA_Native = 0x01,  /// Native UE FileSystem, should be used with editor modules.
        FSA_Package = 0x02, /// Package FileSystem, should be used at runtime.
    };

    /// @brief Unreal Engine implementation of Ultralight's FileSystem (SDK 1.4.0 API).
    /// 
    /// NOTE: This is an internal class. You should not really be using this directly,
    /// unless you have a special filesystem to set.
    /// 
    /// HOW TO USE: SetFSAccess if you are using the native file system, or reading 
    /// through package files, then provide to ULUEHandler.
    class ULTRALIGHTUE_API ULUEFileSystem : public ultralight::FileSystem
    {
    public:
        ULUEFileSystem();
        virtual ~ULUEFileSystem() override;

        /// Set the filesystem access pattern
        void SetFSAccess(FSAccess InAccessPattern);

        /// Set the base directory for file operations
        void SetBaseDirectory(const FString& InBaseDirectory);

        //~ Begin ultralight::FileSystem Interface
        
        /// Check if a file exists within the file system.
        virtual bool FileExists(const ultralight::String& file_path) override;

        /// Get the mime-type of a file (eg "text/html").
        virtual ultralight::String GetFileMimeType(const ultralight::String& file_path) override;

        /// Get the charset / encoding of a file (eg "utf-8").
        virtual ultralight::String GetFileCharset(const ultralight::String& file_path) override;

        /// Open a file for reading and map it to a Buffer.
        virtual ultralight::RefPtr<ultralight::Buffer> OpenFile(const ultralight::String& file_path) override;
        
        //~ End ultralight::FileSystem Interface

    private:
        /// Map an Ultralight path to an Unreal Engine path
        FString MapPath(const ultralight::String& path) const;

        /// Get mime type from file extension
        static ultralight::String GetMimeTypeFromExtension(const FString& Extension);

        FString BaseDirectory;
        FSAccess AccessPattern = FSAccess::FSA_Native;
    };
}
