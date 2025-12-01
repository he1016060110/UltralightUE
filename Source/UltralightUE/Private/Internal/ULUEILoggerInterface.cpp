#include "ULUEILoggerInterface.h"

// Define the log category
DEFINE_LOG_CATEGORY(LogUltralightUE);

namespace ultralightue
{
    void ULUEILoggerInterface::LogMessage(ultralight::LogLevel log_level, const ultralight::String& message)
    {
        // Convert Ultralight string to FString - use utf8().data() as c_str() no longer exists in SDK 1.4
        FString MessageStr = FString(UTF8_TO_TCHAR(message.utf8().data()));
        
        // Log the message using the Unreal Engine logging system
        switch (log_level)
        {
            case ultralight::LogLevel::Error:
                UE_LOG(LogUltralightUE, Error, TEXT("%s"), *MessageStr);
                break;
            case ultralight::LogLevel::Warning:
                UE_LOG(LogUltralightUE, Warning, TEXT("%s"), *MessageStr);
                break;
            case ultralight::LogLevel::Info:
                UE_LOG(LogUltralightUE, Log, TEXT("%s"), *MessageStr);
                break;
            default:
                break;
        }
    }
}