/*
 *   Copyright (c) 2023 Mikael Aboagye & Ultralight Inc.
 *   Runtime renderer + view manager for Ultralight UE integration.
 */

#include "Rendering/ULUERenderer.h"
#include "Rendering/ULUERenderTarget.h"
#include "Rendering/ULUEGPUDriver.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "AppCore/Platform.h"
#include "Ultralight/platform/Surface.h"

using namespace ultralightue;

namespace
{
	// Convert FString to Ultralight::String (UTF-8).
	inline ultralight::String ToUltralightString(const FString& InString)
	{
		return ultralight::String(TCHAR_TO_UTF8(*InString));
	}
}

/* -------------------------------------------------------------------------- */
/*                              FULUEView                                     */
/* -------------------------------------------------------------------------- */

FULUEView::FULUEView(const TWeakPtr<FULUERenderer>& InOwner, ultralight::RefPtr<ultralight::View> InView, UULUERenderTarget* InTarget)
	: Owner(InOwner)
	, View(MoveTemp(InView))
	, Target(InTarget)
	, Size(FIntPoint::ZeroValue)
{
	if (View)
	{
		Size = FIntPoint(View->width(), View->height());
	}
}

FULUEView::~FULUEView() = default;

void FULUEView::LoadURL(const FString& URL)
{
	if (View)
	{
		UE_LOG(LogUltralightUE, Log, TEXT("Loading URL: %s"), *URL);
		View->LoadURL(ToUltralightString(URL));
	}
}

void FULUEView::LoadHTML(const FString& HTML, const FString& VirtualURL)
{
	if (View)
	{
		UE_LOG(LogUltralightUE, Log, TEXT("Loading HTML (VirtualURL: %s, Length: %d)"), *VirtualURL, HTML.Len());
		View->LoadHTML(ToUltralightString(HTML), ToUltralightString(VirtualURL), true);
	}
}

void FULUEView::Resize(const FIntPoint& InSize)
{
	if (!View || InSize == Size)
	{
		return;
	}

	Size = InSize;
	View->Resize(Size.X, Size.Y);

	if (UULUERenderTarget* RenderTarget = Target.Get())
	{
		if (UTextureRenderTarget2D* Texture = RenderTarget->GetRenderTarget())
		{
			Texture->ResizeTarget(Size.X, Size.Y);
		}
	}
}

void FULUEView::SetFocused(bool bFocused)
{
	if (!View)
	{
		return;
	}

	if (bFocused)
	{
		View->Focus();
	}
	else
	{
		View->Unfocus();
	}
}

void FULUEView::PaintIfNeeded()
{
	if (!View || !Target.IsValid())
	{
		return;
	}

	CopySurfaceToTarget();
}

void FULUEView::InjectMouseEvent(ultralight::MouseEvent::Type Type, const FVector2D& Position, ultralight::MouseEvent::Button Button, uint32 /*Modifiers*/)
{
	if (!View)
	{
		return;
	}

	ultralight::MouseEvent Event;
	Event.type = Type;
	Event.x = FMath::RoundToInt(Position.X);
	Event.y = FMath::RoundToInt(Position.Y);
	Event.button = Button;
	View->FireMouseEvent(Event);
}

void FULUEView::InjectScroll(const FVector2D& ScrollDelta, bool bByPage, uint32 /*Modifiers*/)
{
	if (!View)
	{
		return;
	}

	ultralight::ScrollEvent Event;
	Event.type = bByPage ? ultralight::ScrollEvent::kType_ScrollByPage : ultralight::ScrollEvent::kType_ScrollByPixel;
	Event.delta_x = FMath::RoundToInt(ScrollDelta.X);
	Event.delta_y = FMath::RoundToInt(ScrollDelta.Y);
	View->FireScrollEvent(Event);
}

void FULUEView::InjectKeyEvent(const ultralight::KeyEvent& Event)
{
	if (View)
	{
		View->FireKeyEvent(Event);
	}
}

UTextureRenderTarget2D* FULUEView::GetRenderTarget() const
{
	return Target.IsValid() ? Target->GetRenderTarget() : nullptr;
}

UULUERenderTarget* FULUEView::GetRenderTargetWrapper() const
{
	return Target.Get();
}

void FULUEView::CopySurfaceToTarget()
{
	if (!View)
	{
		return;
	}

	ultralight::Surface* Surface = View->surface();
	if (!Surface)
	{
		return;
	}

	// Check if there's anything to paint
	bool bHasDirtyBounds = !Surface->dirty_bounds().IsEmpty();

	// Always try to render on first few frames or when dirty
	bool bForceRender = (FramesSinceCreation < 30); // Force render for first 30 frames
	FramesSinceCreation++;

	if (!bHasDirtyBounds && !bForceRender)
	{
		return;
	}

	// Safely cast to BitmapSurface
	auto* BitmapSurface = static_cast<ultralight::BitmapSurface*>(Surface);
	if (!BitmapSurface)
	{
		return;
	}

	ultralight::RefPtr<ultralight::Bitmap> Bitmap = BitmapSurface->bitmap();
	if (Bitmap && Bitmap.get() && Target.IsValid())
	{
		Target->OnUltralightDraw(Bitmap.get());
	}

	Surface->ClearDirtyBounds();
}

/* -------------------------------------------------------------------------- */
/*                            FULUERenderer                                   */
/* -------------------------------------------------------------------------- */

bool FULUERenderer::Initialize(const FString& PluginBaseDir, FSAccess AccessPattern, ULUELogInterface* InLogInterface)
{
	if (Renderer.get())
	{
		return true;
	}

	ResourceRoot = FPaths::ConvertRelativePathToFull(FPaths::Combine(PluginBaseDir, TEXT("Source/ThirdParty/UltralightUELibrary")));
	CachePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("UltralightCache"));
	IFileManager::Get().MakeDirectory(*CachePath, true);

	FileSystem = MakeUnique<ULUEFileSystem>();
	// Default UI content path under Content/uicontent, resource path stays with the plugin.
	const FString UiContentRoot = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("uicontent"));
	IFileManager::Get().MakeDirectory(*UiContentRoot, true);
	FileSystem->SetBaseDirectory(UiContentRoot);
	FileSystem->SetResourceDirectory(ResourceRoot);
	FileSystem->SetFSAccess(AccessPattern);

	if (!InLogInterface)
	{
		OwnedLogInterface = MakeUnique<ULUELogInterface>();
		InLogInterface = OwnedLogInterface.Get();
	}
	LoggerBridge = InLogInterface ? InLogInterface->GetLogger() : nullptr;

	// Create minimal GPU driver (required even for CPU rendering)
	GPUDriver = MakeUnique<ultralightue::ULUEGPUDriver>();

	auto& Platform = ultralight::Platform::instance();

	ultralight::Config Config;
	Config.cache_path = ToUltralightString(CachePath);
	Config.resource_path_prefix = "resources/";
	Config.face_winding = ultralight::FaceWinding::Clockwise; // UE/D3D uses clockwise front faces.
	Platform.set_config(Config);

	Platform.set_font_loader(ultralight::GetPlatformFontLoader());
	Platform.set_file_system(FileSystem.Get());
	Platform.set_gpu_driver(GPUDriver.Get());
	Platform.set_surface_factory(ultralight::GetBitmapSurfaceFactory());

	if (LoggerBridge)
	{
		Platform.set_logger(LoggerBridge);
	}

	Renderer = ultralight::Renderer::Create();
	Session = Renderer ? Renderer->default_session() : nullptr;

	UE_LOG(LogUltralightUE, Log, TEXT("Ultralight Renderer initialized: %s"), Renderer.get() ? TEXT("Success") : TEXT("Failed"));

	return Renderer.get() != nullptr;
}

void FULUERenderer::Tick(float DeltaTime)
{
	if (!Renderer.get() || !Session.get())
	{
		return;
	}

	// Prune dead views first
	PruneDeadViews();

	// Only tick renderer if we have active views
	if (Views.Num() == 0)
	{
		return;
	}

	// Safely update renderer
	Renderer->Update();
	Renderer->Render();

	// Paint each view - copy views array to avoid issues during iteration
	TArray<TWeakPtr<FULUEView>> ViewsCopy = Views;
	for (const TWeakPtr<FULUEView>& WeakView : ViewsCopy)
	{
		if (TSharedPtr<FULUEView> View = WeakView.Pin())
		{
			View->PaintIfNeeded();
		}
	}
}

void FULUERenderer::Shutdown()
{
	Views.Empty();
	Session = ultralight::RefPtr<ultralight::Session>();
	Renderer = ultralight::RefPtr<ultralight::Renderer>();

	auto& Platform = ultralight::Platform::instance();
	Platform.set_file_system(nullptr);
	Platform.set_gpu_driver(nullptr);
	Platform.set_logger(nullptr);
	Platform.set_font_loader(nullptr);
	Platform.set_surface_factory(nullptr);

	FileSystem.Reset();
	GPUDriver.Reset();
	OwnedLogInterface.Reset();
	LoggerBridge = nullptr;

	UE_LOG(LogUltralightUE, Log, TEXT("Ultralight Renderer shut down"));
}

TSharedPtr<FULUEView> FULUERenderer::CreateView(const FIntPoint& Size, bool bTransparent, const FString& InitialURL, UObject* Outer, UTextureRenderTarget2D* ExistingRenderTarget)
{
	if (!Renderer || !Outer)
	{
		return nullptr;
	}

	ultralight::ViewConfig ViewConfig;
	ViewConfig.is_transparent = bTransparent;
	ViewConfig.is_accelerated = false; // CPU renderer so we can copy bitmap data to UE.

	ultralight::RefPtr<ultralight::View> NativeView = Renderer->CreateView(Size.X, Size.Y, ViewConfig, Session);
	if (!NativeView)
	{
		return nullptr;
	}

	UTextureRenderTarget2D* RenderTarget = ExistingRenderTarget;
	if (!RenderTarget)
	{
		RenderTarget = NewObject<UTextureRenderTarget2D>(Outer);
		RenderTarget->ClearColor = FLinearColor::Transparent;
		// Use PF_B8G8R8A8 to directly match Ultralight's native BGRA output
		RenderTarget->InitCustomFormat(Size.X, Size.Y, PF_B8G8R8A8, false);
		RenderTarget->UpdateResourceImmediate(true);
	}

	UULUERenderTarget* TargetWrapper = NewObject<UULUERenderTarget>(Outer);
	TargetWrapper->Initialize(RenderTarget);

	TSharedPtr<FULUEView> View = MakeShared<FULUEView>(AsShared(), NativeView, TargetWrapper);
	if (View.IsValid() && !InitialURL.IsEmpty())
	{
		View->LoadURL(InitialURL);
	}

	Views.Add(View);
	return View;
}

void FULUERenderer::DestroyView(const TSharedPtr<FULUEView>& View)
{
	if (!View.IsValid())
	{
		return;
	}

	Views.RemoveAll([&View](const TWeakPtr<FULUEView>& Candidate)
	{
		return Candidate.Pin() == View;
	});
}

void FULUERenderer::PruneDeadViews()
{
	Views.RemoveAll([](const TWeakPtr<FULUEView>& Candidate)
	{
		return !Candidate.IsValid();
	});
}
