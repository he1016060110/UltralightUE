/*
 *   Copyright (c) 2023 Mikael Aboagye & Ultralight Inc.
 *   All rights reserved.
 *   Updated for Ultralight SDK 1.4.0 API compatibility.
 */

#include "Rendering/ULUERenderTarget.h"
#include "Engine/TextureRenderTarget2D.h"

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
    }
}

void UULUERenderTarget::OnUltralightDraw(ultralight::Bitmap* Bitmap)
{
    if (!RenderTarget || !Bitmap)
    {
        return;
    }
    
    // TODO: Implement bitmap data transfer from Ultralight to UE texture
    // This would typically involve:
    // 1. Locking the Ultralight bitmap pixels
    // 2. Updating the UE render target with the pixel data
    // 3. Unlocking the bitmap
}

void UULUERenderTarget::UpdateUETexture()
{
    if (!RenderTarget || PixelData.Num() == 0)
    {
        return;
    }
    
    // TODO: Implement texture update logic
    // This would use RenderTarget->UpdateResource() or similar
}

