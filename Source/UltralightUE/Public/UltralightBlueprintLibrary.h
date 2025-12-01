/*
 * Blueprint function library for easy Ultralight testing.
 */

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UltralightBlueprintLibrary.generated.h"

class UUltralightView;
class UTextureRenderTarget2D;

/**
 * Blueprint function library for Ultralight convenience functions.
 */
UCLASS()
class ULTRALIGHTUE_API UUltralightBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Quick test function - creates a view with test HTML content.
	 * @param WorldContextObject - World context for subsystem access
	 * @param Width - View width in pixels
	 * @param Height - View height in pixels
	 * @return The created view with test content loaded
	 */
	UFUNCTION(BlueprintCallable, Category = "Ultralight", meta = (WorldContext = "WorldContextObject", DisplayName = "Create Test View"))
	static UUltralightView* CreateTestView(UObject* WorldContextObject, int32 Width = 1920, int32 Height = 1080);

	/**
	 * Load simple HTML test content into a view.
	 * @param View - The view to load content into
	 * @param Title - Text to display
	 * @param BackgroundColor - CSS color (e.g., "#667eea", "red", "rgb(100,200,50)")
	 */
	UFUNCTION(BlueprintCallable, Category = "Ultralight", meta = (DisplayName = "Load Test HTML"))
	static void LoadTestHTML(UUltralightView* View, const FString& Title = TEXT("Ultralight Test"), const FString& BackgroundColor = TEXT("#667eea"));
};
