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

#include "CoreMinimal.h"
#include "Templates/UniquePtr.h"

// Forward declare log category (defined in ULUEILoggerInterface.cpp)
DECLARE_LOG_CATEGORY_EXTERN(LogUltralightUE, Log, All);

namespace ultralightue
{
    // Forward declaration
    class ULUEILoggerInterface;

    /// @brief Simple class for developers to assign logging interfaces to the plugin.
    /// NOTE: By default, the plugin will log to the console like normal, using color codes like: red, yellow, and white.
    class ULTRALIGHTUE_API ULUELogInterface
    {
    public:
        ULUELogInterface();
        // Destructor must be defined in cpp file where ULUEILoggerInterface is complete
        virtual ~ULUELogInterface();

        /// @brief Logs an error within the developer defined interface.
        /// @param details What the error message will contain.
        virtual void LogError(FString details);

        /// @brief Logs a warning within the developer defined interface.
        /// @param details What the warning message will contain.
        virtual void LogWarning(FString details);

        /// @brief Logs info within the developer defined interface.
        /// @param details What the info message will contain.
        virtual void LogInfo(FString details);

        /// @brief Returns the internal Ultralight logger used to bridge to UE logging.
        class ULUEILoggerInterface* GetLogger() const;

    private:
        TUniquePtr<ULUEILoggerInterface> LoggerInterface;
    };
}
