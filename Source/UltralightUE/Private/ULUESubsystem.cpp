/*
 * Ultralight runtime subsystem responsible for ticking the renderer and creating views.
 */

#include "ULUESubsystem.h"
#include "Rendering/ULUERenderer.h"
#include "Rendering/ULUEView.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Containers/Ticker.h"

using namespace ultralightue;

void UUltralightSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("UltralightUE")))
	{
		ResourceRoot = Plugin->GetBaseDir();
	}
	else
	{
		ResourceRoot = FPaths::ProjectPluginsDir() / TEXT("UltralightUE");
	}

	EnsureRenderer();
}

void UUltralightSubsystem::Deinitialize()
{
	if (TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
		TickHandle.Reset();
	}

	if (Renderer.IsValid())
	{
		Renderer->Shutdown();
		Renderer.Reset();
	}

	Super::Deinitialize();
}

UUltralightView* UUltralightSubsystem::CreateView(int32 Width, int32 Height, bool bTransparent, const FString& InitialURL, UTextureRenderTarget2D* ExistingTarget)
{
	if (!EnsureRenderer())
	{
		return nullptr;
	}

	TSharedPtr<FULUEView> NativeView = Renderer->CreateView(FIntPoint(Width, Height), bTransparent, InitialURL, this, ExistingTarget);
	if (!NativeView.IsValid())
	{
		return nullptr;
	}

	UUltralightView* Wrapper = NewObject<UUltralightView>(this);
	Wrapper->InitializeNative(Renderer.ToSharedRef(), NativeView);
	return Wrapper;
}

void UUltralightSubsystem::DestroyView(UUltralightView* View)
{
	if (!View)
	{
		return;
	}

	View->ReleaseNative();
}

bool UUltralightSubsystem::EnsureRenderer()
{
	if (!Renderer.IsValid())
	{
		Renderer = MakeShared<FULUERenderer>();
	}

	if (Renderer->IsInitialized())
	{
		if (!TickHandle.IsValid())
		{
			TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UUltralightSubsystem::Tick));
		}
		return true;
	}

	const bool bInitialized = Renderer->Initialize(ResourceRoot, FSAccess::FSA_Native, nullptr);
	if (bInitialized && !TickHandle.IsValid())
	{
		TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UUltralightSubsystem::Tick));
	}
	if (bInitialized)
	{
		ResourceRoot = Renderer->GetResourceRoot();
	}
	return bInitialized;
}

bool UUltralightSubsystem::Tick(float DeltaSeconds)
{
	if (Renderer.IsValid())
	{
		Renderer->Tick(DeltaSeconds);
	}
	return true;
}
