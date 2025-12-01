/*
 * Blueprint-friendly Ultralight View wrapper.
 */

#pragma once

#include "CoreMinimal.h"
#include "Rendering/ULUERenderTarget.h"
#include "InputCoreTypes.h"
#include "ULUEUltralightIncludes.h"
#include "ULUEView.generated.h"

class FULUEView;
class FULUERenderer;

UENUM(BlueprintType)
enum class EULUEMouseButton : uint8
{
	None  UMETA(DisplayName = "None"),
	Left  UMETA(DisplayName = "Left"),
	Middle UMETA(DisplayName = "Middle"),
	Right UMETA(DisplayName = "Right")
};

/**
 * UObject wrapper for an Ultralight View.
 * Holds onto the render target and provides simple input forwarding helpers.
 */
UCLASS(BlueprintType)
class ULTRALIGHTUE_API UUltralightView : public UObject
{
	GENERATED_BODY()

public:
	void InitializeNative(TSharedRef<FULUERenderer> InRenderer, const TSharedPtr<FULUEView>& InView);
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "Ultralight", meta = (DisplayName = "Load URL"))
	void LoadURL(const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "Ultralight", meta = (DisplayName = "Load HTML"))
	void LoadHTML(const FString& HTML, const FString& VirtualURL = TEXT("about:blank"));

	UFUNCTION(BlueprintCallable, Category = "Ultralight", meta = (DisplayName = "Resize View"))
	void Resize(int32 Width, int32 Height);

	UFUNCTION(BlueprintCallable, Category = "Ultralight", meta = (DisplayName = "Set Focused"))
	void SetFocused(bool bFocused);

	UFUNCTION(BlueprintPure, Category = "Ultralight", meta = (DisplayName = "Get Render Target"))
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

	// Input helpers
	UFUNCTION(BlueprintCallable, Category = "Ultralight|Input", meta = (DisplayName = "Inject Mouse Move"))
	void InjectMouseMove(const FVector2D& Position);

	UFUNCTION(BlueprintCallable, Category = "Ultralight|Input", meta = (DisplayName = "Inject Mouse Button"))
	void InjectMouseButton(const FVector2D& Position, EULUEMouseButton Button, bool bPressed, bool bShift = false, bool bCtrl = false, bool bAlt = false, bool bMeta = false);

	UFUNCTION(BlueprintCallable, Category = "Ultralight|Input", meta = (DisplayName = "Inject Scroll"))
	void InjectScroll(const FVector2D& ScrollDelta, bool bByPage = false, bool bShift = false, bool bCtrl = false, bool bAlt = false, bool bMeta = false);

	UFUNCTION(BlueprintCallable, Category = "Ultralight|Input", meta = (DisplayName = "Inject Key Down"))
	void InjectKeyDown(FKey Key, bool bIsRepeat = false, bool bShift = false, bool bCtrl = false, bool bAlt = false, bool bMeta = false);

	UFUNCTION(BlueprintCallable, Category = "Ultralight|Input", meta = (DisplayName = "Inject Key Up"))
	void InjectKeyUp(FKey Key, bool bShift = false, bool bCtrl = false, bool bAlt = false, bool bMeta = false);

	UFUNCTION(BlueprintCallable, Category = "Ultralight|Input", meta = (DisplayName = "Inject Character"))
	void InjectChar(const FString& Characters, bool bShift = false, bool bCtrl = false, bool bAlt = false, bool bMeta = false);

	// Release native resources without destroying the UObject (used by subsystem shutdown).
	void ReleaseNative();

private:
	TWeakPtr<FULUERenderer> Renderer;
	TSharedPtr<FULUEView> NativeView;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> RenderTarget = nullptr;

	UPROPERTY()
	TObjectPtr<UULUERenderTarget> RenderTargetWrapper = nullptr;

	// Helpers
	static ultralight::MouseEvent::Button ToUltralightButton(EULUEMouseButton Button);
	static int32 GetVirtualKeyCode(const FKey& Key);
	static uint32 BuildModifierFlags(bool bShift, bool bCtrl, bool bAlt, bool bMeta);
	static ultralight::KeyEvent BuildKeyEvent(ultralight::KeyEvent::Type Type, int32 VirtualKey, uint32 Modifiers, bool bIsRepeat);
};
