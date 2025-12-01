/*
 *   Runtime renderer wrapper for Ultralight.
 *   Handles platform wiring, renderer lifetime, and view management.
 */

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "FileSystem/ULUEFileSystem.h"
#include "ULUELogInterface.h"
#include "ULUEUltralightIncludes.h"

class UTextureRenderTarget2D;
class UULUERenderTarget;

namespace ultralightue
{
	class ULUEGPUDriver;
}

/**
 * Internal Ultralight View wrapper. Copies bitmap surfaces into a UE render target.
 */
class FULUEView : public TSharedFromThis<FULUEView>
{
public:
	FULUEView(const TWeakPtr<class FULUERenderer>& InOwner, ultralight::RefPtr<ultralight::View> InView, UULUERenderTarget* InTarget);
	~FULUEView();

	void LoadURL(const FString& URL);
	void LoadHTML(const FString& HTML, const FString& VirtualURL = TEXT("about:blank"));
	void Resize(const FIntPoint& InSize);
	void SetFocused(bool bFocused);
	void PaintIfNeeded();

	void InjectMouseEvent(ultralight::MouseEvent::Type Type, const FVector2D& Position, ultralight::MouseEvent::Button Button, uint32 Modifiers);
	void InjectScroll(const FVector2D& ScrollDelta, bool bByPage, uint32 Modifiers);
	void InjectKeyEvent(const ultralight::KeyEvent& Event);

	UTextureRenderTarget2D* GetRenderTarget() const;
	UULUERenderTarget* GetRenderTargetWrapper() const;
	FIntPoint GetSize() const { return Size; }

private:
	void CopySurfaceToTarget();

	TWeakPtr<class FULUERenderer> Owner;
	ultralight::RefPtr<ultralight::View> View;
	TWeakObjectPtr<UULUERenderTarget> Target;
	FIntPoint Size;
	int32 FramesSinceCreation = 0;
};

/**
 * Manages Ultralight renderer lifecycle and ticking.
 */
class FULUERenderer : public TSharedFromThis<FULUERenderer>
{
public:
	bool Initialize(const FString& PluginBaseDir, ultralightue::FSAccess AccessPattern, ultralightue::ULUELogInterface* InLogInterface = nullptr);
	void Tick(float DeltaTime);
	void Shutdown();

	TSharedPtr<FULUEView> CreateView(const FIntPoint& Size, bool bTransparent, const FString& InitialURL, UObject* Outer, UTextureRenderTarget2D* ExistingRenderTarget = nullptr);
	void DestroyView(const TSharedPtr<FULUEView>& View);

	bool IsInitialized() const { return Renderer.get() != nullptr; }
	const FString& GetResourceRoot() const { return ResourceRoot; }

private:
    void PruneDeadViews();

    TUniquePtr<ultralightue::ULUEFileSystem> FileSystem;
    TUniquePtr<ultralightue::ULUELogInterface> OwnedLogInterface;
    TUniquePtr<ultralightue::ULUEGPUDriver> GPUDriver;
    ultralightue::ULUEILoggerInterface* LoggerBridge = nullptr;

	ultralight::RefPtr<ultralight::Renderer> Renderer;
	ultralight::RefPtr<ultralight::Session> Session;

	TArray<TWeakPtr<FULUEView>> Views;

	FString ResourceRoot;
	FString CachePath;
};
