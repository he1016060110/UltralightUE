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

    // Ultralight outputs BGRA format (4 bytes per pixel)
    // The image appears to be both horizontally and vertically flipped
    // Perform 180-degree rotation: flip both rows and pixels within each row
    const uint8* SrcPixels = static_cast<const uint8*>(LockedPixels);
    uint8* DstPixels = PixelData.GetData();

    const uint32 PixelSize = 4; // BGRA = 4 bytes per pixel
    const uint32 PixelsPerRow = SourceRowBytes / PixelSize;

    // Start from last row, last pixel
    for (uint32 Row = 0; Row < Height; ++Row)
    {
        const uint8* SrcRow = SrcPixels + (Height - 1 - Row) * SourceRowBytes; // Last row first
        uint8* DstRow = DstPixels + Row * SourceRowBytes;

        // Copy pixels in reverse order within each row
        for (uint32 Col = 0; Col < PixelsPerRow; ++Col)
        {
            const uint8* SrcPixel = SrcRow + (PixelsPerRow - 1 - Col) * PixelSize; // Last pixel first
            uint8* DstPixel = DstRow + Col * PixelSize;

            // Copy 4 bytes (BGRA)
            DstPixel[0] = SrcPixel[0]; // B
            DstPixel[1] = SrcPixel[1]; // G
            DstPixel[2] = SrcPixel[2]; // R
            DstPixel[3] = SrcPixel[3]; // A
        }
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
