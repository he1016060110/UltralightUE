/*
 *   Copyright (c) 2023 Mikael Aboagye & Ultralight Inc.
 *   All rights reserved.
 *   Updated for Ultralight SDK 1.4.0 API compatibility.
 */

#include "FileSystem/ULUEFileSystem.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

namespace ultralightue
{
    ULUEFileSystem::ULUEFileSystem()
        : BaseDirectory(FPaths::ProjectContentDir())
        , ResourceDirectory(BaseDirectory)
        , AccessPattern(FSAccess::FSA_Native)
    {
    }

    ULUEFileSystem::~ULUEFileSystem()
    {
    }

    void ULUEFileSystem::SetFSAccess(FSAccess InAccessPattern)
    {
        AccessPattern = InAccessPattern;
    }

    void ULUEFileSystem::SetBaseDirectory(const FString& InBaseDirectory)
    {
        BaseDirectory = InBaseDirectory;
        if (ResourceDirectory.IsEmpty())
        {
            ResourceDirectory = BaseDirectory;
        }
    }

    void ULUEFileSystem::SetResourceDirectory(const FString& InResourceDirectory)
    {
        ResourceDirectory = InResourceDirectory;
    }

    bool ULUEFileSystem::FileExists(const ultralight::String& file_path)
    {
        FString UEPath = MapPath(file_path);
        IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
        return PlatformFile.FileExists(*UEPath);
    }

    ultralight::String ULUEFileSystem::GetFileMimeType(const ultralight::String& file_path)
    {
        FString UEPath = FString(UTF8_TO_TCHAR(file_path.utf8().data()));
        FString Extension = FPaths::GetExtension(UEPath).ToLower();
        return GetMimeTypeFromExtension(Extension);
    }

    ultralight::String ULUEFileSystem::GetFileCharset(const ultralight::String& file_path)
    {
        // Default to UTF-8 for all text files
        return ultralight::String("utf-8");
    }

    ultralight::RefPtr<ultralight::Buffer> ULUEFileSystem::OpenFile(const ultralight::String& file_path)
    {
        FString UEPath = MapPath(file_path);
        IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

        // Check if file exists
        if (!PlatformFile.FileExists(*UEPath))
        {
            UE_LOG(LogTemp, Warning, TEXT("ULUEFileSystem::OpenFile - File not found: %s"), *UEPath);
            return nullptr;
        }

        // Get file size
        int64 FileSize = PlatformFile.FileSize(*UEPath);
        if (FileSize <= 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("ULUEFileSystem::OpenFile - File is empty or size error: %s"), *UEPath);
            return nullptr;
        }

        // Read the entire file into memory
        TArray<uint8> FileData;
        if (!FFileHelper::LoadFileToArray(FileData, *UEPath))
        {
            UE_LOG(LogTemp, Error, TEXT("ULUEFileSystem::OpenFile - Failed to read file: %s"), *UEPath);
            return nullptr;
        }

        // Create a Buffer from the file data (makes a copy)
        return ultralight::Buffer::CreateFromCopy(FileData.GetData(), FileData.Num());
    }

    FString ULUEFileSystem::MapPath(const ultralight::String& path) const
    {
        FString UltralightPath = FString(UTF8_TO_TCHAR(path.utf8().data()));
        
        // Remove any leading slashes to avoid double slashes
        while (UltralightPath.StartsWith(TEXT("/")))
        {
            UltralightPath = UltralightPath.Mid(1);
        }
        
        // Ultralight built-in resources (cacert/icu data) are expected under the resource directory.
        if (UltralightPath.StartsWith(TEXT("resources")))
        {
            return ResourceDirectory / UltralightPath;
        }

        return BaseDirectory / UltralightPath;
    }

    ultralight::String ULUEFileSystem::GetMimeTypeFromExtension(const FString& Extension)
    {
        // Common web file types
        if (Extension == TEXT("html") || Extension == TEXT("htm"))
        {
            return ultralight::String("text/html");
        }
        else if (Extension == TEXT("css"))
        {
            return ultralight::String("text/css");
        }
        else if (Extension == TEXT("js"))
        {
            return ultralight::String("application/javascript");
        }
        else if (Extension == TEXT("json"))
        {
            return ultralight::String("application/json");
        }
        else if (Extension == TEXT("xml"))
        {
            return ultralight::String("application/xml");
        }
        else if (Extension == TEXT("svg"))
        {
            return ultralight::String("image/svg+xml");
        }
        else if (Extension == TEXT("png"))
        {
            return ultralight::String("image/png");
        }
        else if (Extension == TEXT("jpg") || Extension == TEXT("jpeg"))
        {
            return ultralight::String("image/jpeg");
        }
        else if (Extension == TEXT("gif"))
        {
            return ultralight::String("image/gif");
        }
        else if (Extension == TEXT("webp"))
        {
            return ultralight::String("image/webp");
        }
        else if (Extension == TEXT("ico"))
        {
            return ultralight::String("image/x-icon");
        }
        else if (Extension == TEXT("woff"))
        {
            return ultralight::String("font/woff");
        }
        else if (Extension == TEXT("woff2"))
        {
            return ultralight::String("font/woff2");
        }
        else if (Extension == TEXT("ttf"))
        {
            return ultralight::String("font/ttf");
        }
        else if (Extension == TEXT("otf"))
        {
            return ultralight::String("font/otf");
        }
        else if (Extension == TEXT("txt"))
        {
            return ultralight::String("text/plain");
        }
        
        // Default to binary stream for unknown types
        return ultralight::String("application/octet-stream");
    }
}
