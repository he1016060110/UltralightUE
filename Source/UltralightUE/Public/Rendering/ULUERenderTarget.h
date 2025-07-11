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
#include "Engine/TextureRenderTarget2D.h"
#include <Ultralight/RenderTarget.h> // Assuming this is the correct include from Ultralight SDK
#include "ULUERenderTarget.generated.h" // For UCLASS macro if this becomes a UObject

// Forward declarations
namespace ultralight { class Bitmap; }

namespace ultralightue
{
	/**
	 * Represents an Unreal Engine render target that can be used by Ultralight.
	 * This class would typically wrap a UTextureRenderTarget2D and implement ultralight::RenderTarget.
	 *
	 * If this needs to be a UObject to be managed by UE's GC and be available in Blueprints:
	 */
	UCLASS(BlueprintType)
	class ULTRALIGHTUE_API UULUERenderTarget : public UObject // Or a more specific base if needed
	{
		GENERATED_BODY()

	public:
		UULUERenderTarget();
		virtual ~UULUERenderTarget();

		// Method to initialize with a UE Render Target
		UFUNCTION(BlueprintCallable, Category = "UltralightUE")
		void Initialize(UTextureRenderTarget2D* InRenderTarget);

		// Method to get the underlying UE Render Target
		UFUNCTION(BlueprintPure, Category = "UltralightUE")
		UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

		// This would be the ultralight::View's RenderTarget, which Ultralight will draw to.
		// This class would then need to implement ultralight::RenderTarget interface
		// and copy data from Ultralight's bitmap to the UTextureRenderTarget2D.
		// For simplicity, this example doesn't fully implement ultralight::RenderTarget,
		// but shows the structure. A more complete implementation would require
		// creating a separate class that inherits from ultralight::RenderTarget and is managed by this UObject.

		// Example: Called by Ultralight when it wants to draw to this target
		// This is a conceptual representation. Actual implementation would involve
		// being a delegate for Ultralight's rendering process or implementing ultralight::GPUDriver.
		void OnUltralightDraw(ultralight::Bitmap* Bitmap);

	private:
		UPROPERTY(Transient) // Transient if this UObject is just a wrapper and the RT is managed elsewhere
		TObjectPtr<UTextureRenderTarget2D> RenderTarget;

		// Helper to copy bitmap data to the UTextureRenderTarget2D
		void UpdateUETexture();

		// Buffer to hold pixel data from Ultralight before updating UE texture
		// This might be needed depending on how data is passed from Ultralight
		TArray<uint8> PixelData;
		uint32 Width;
		uint32 Height;
	};
}