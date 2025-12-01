/*
 * Game instance subsystem that owns the Ultralight renderer and ticks it.
 */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Containers/Ticker.h"
#include "ULUESubsystem.generated.h"

class UUltralightView;
class FULUERenderer;

UCLASS()
class ULTRALIGHTUE_API UUltralightSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Create a view and optionally supply an existing RenderTarget. */
	UFUNCTION(BlueprintCallable, Category = "Ultralight")
	UUltralightView* CreateView(int32 Width, int32 Height, bool bTransparent, const FString& InitialURL = TEXT(""), UTextureRenderTarget2D* ExistingTarget = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Ultralight")
	void DestroyView(UUltralightView* View);

	UFUNCTION(BlueprintPure, Category = "Ultralight")
	FString GetResourceRoot() const { return ResourceRoot; }

private:
	bool EnsureRenderer();
	bool Tick(float DeltaSeconds);

	TSharedPtr<FULUERenderer> Renderer;
	FString ResourceRoot;
	FTSTicker::FDelegateHandle TickHandle;
};
