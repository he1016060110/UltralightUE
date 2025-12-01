/*
 *   Copyright (c) 2023 Mikael Aboagye & Ultralight Inc.
 *   All rights reserved.
 *   Updated for Ultralight SDK 1.4.0 API compatibility.
 */

#include "Rendering/ULUERenderTarget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTarget.h"
#include "TextureResource.h"
#include "RenderingThread.h"
#include "RHICommandList.h"
#include "ULUEUltralightIncludes.h"

UULUERenderTarget::UULUERenderTarget()
    : RenderTarget(nullptr)
    , Width(0)
    , Height(0)
{
}

UULUERenderTarget::~UULUERenderTarget()
{
    // Cleanup handled by UE garbage collection for UObject-based members
}

void UULUERenderTarget::Initialize(UTextureRenderTarget2D* InRenderTarget)
{
    RenderTarget = InRenderTarget;
    if (RenderTarget)
    {
        Width = RenderTarget->SizeX;
        Height = RenderTarget->SizeY;
        RenderTarget->UpdateResourceImmediate(true);
    }
}

void UULUERenderTarget::OnUltralightDraw(ultralight::Bitmap* Bitmap)
{
    if (!RenderTarget || !Bitmap)
    {
        return;
    }

    // Resize the RT if Ultralight resized
    if (RenderTarget->SizeX != static_cast<int32>(Bitmap->width()) ||
        RenderTarget->SizeY != static_cast<int32>(Bitmap->height()))
    {
        Width = Bitmap->width();
        Height = Bitmap->height();
        RenderTarget->ResizeTarget(Width, Height);
        RenderTarget->UpdateResourceImmediate(false);
    }

    // Copy pixel data out before unlocking (Ultralight requires this)
    const uint32 SourceRowBytes = Bitmap->row_bytes();
    const SIZE_T TotalSize = static_cast<SIZE_T>(SourceRowBytes) * static_cast<SIZE_T>(Bitmap->height());
    if (TotalSize == 0)
    {
        return;
    }
    PixelData.SetNumUninitialized(TotalSize);

    const void* LockedPixels = Bitmap->LockPixels();
    if (!LockedPixels)
    {
        PixelData.Reset();
        return;
    }

    // Ultralight uses BGRA, UE uses RGBA - need to swap R and B channels
    const uint8* SourcePixels = static_cast<const uint8*>(LockedPixels);
    const uint32 PixelCount = Bitmap->width() * Bitmap->height();

    for (uint32 i = 0; i < PixelCount; ++i)
    {
        const uint32 SrcOffset = i * 4;
        const uint32 DstOffset = i * 4;

        // Swap B and R (BGRA -> RGBA)
        PixelData[DstOffset + 0] = SourcePixels[SrcOffset + 2]; // R = B
        PixelData[DstOffset + 1] = SourcePixels[SrcOffset + 1]; // G = G
        PixelData[DstOffset + 2] = SourcePixels[SrcOffset + 0]; // B = R
        PixelData[DstOffset + 3] = SourcePixels[SrcOffset + 3]; // A = A
    }

    Bitmap->UnlockPixels();

    // Upload to render target on the render thread
    FTextureRenderTargetResource* TargetResource = RenderTarget->GameThread_GetRenderTargetResource();
    if (!TargetResource)
    {
        PixelData.Reset();
        return;
    }

    const uint32 CopyWidth = Bitmap->width();
    const uint32 CopyHeight = Bitmap->height();
    TArray<uint8> PixelCopy = PixelData; // copy for thread safety

    ENQUEUE_RENDER_COMMAND(CopyUltralightToRT)(
        [TargetResource, PixelCopy = MoveTemp(PixelCopy), SourceRowBytes, CopyWidth, CopyHeight](FRHICommandListImmediate& RHICmdList) mutable
        {
            FRHITexture* TextureRHI = TargetResource->GetRenderTargetTexture();
            if (!TextureRHI || PixelCopy.Num() == 0)
            {
                return;
            }

            const FUpdateTextureRegion2D UpdateRegion(0, 0, 0, 0, CopyWidth, CopyHeight);
            RHICmdList.UpdateTexture2D(TextureRHI, 0, UpdateRegion, SourceRowBytes, PixelCopy.GetData());
        });
}

void UULUERenderTarget::UpdateUETexture()
{
    if (!RenderTarget || PixelData.Num() == 0)
    {
        return;
    }
    
    // Legacy helper kept for compatibility; OnUltralightDraw enqueues the render-thread update.
    RenderTarget->UpdateResourceImmediate(false);
}
