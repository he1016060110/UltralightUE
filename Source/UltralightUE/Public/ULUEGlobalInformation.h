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
// Assuming ULUEDefines.h is correctly located in UltralightUELibrary's public include path.
#include "ULUEDefines.h"

// Forward declaration for an Ultralight class, if needed for the GConfig object.
// namespace ultralight { class Config; }

namespace ultralightue
{
	// It seems like this file was intended to hold global information or configurations for the plugin.
	// A common pattern is to have a static class or a singleton to hold such data.
	// For example, if Ultralight requires a global Config object to be set up.

	class ULTRALIGHTUE_API FULUEGlobalInformation // Changed ULTRALIGHTUE_EXPORT to ULTRALIGHTUE_API
	{
	public:
		FULUEGlobalInformation();
		~FULUEGlobalInformation();

		// Example: Static method to get or initialize Ultralight Config
		// static ultralight::Config& GetUltralightConfig();

		// Add other global settings or utility functions relevant to the plugin.

	private:
		// Example: Static instance of Ultralight Config
		// static ultralight::Config UltralightConfigInstance;
		// static bool bUltralightConfigInitialized;
	};
}