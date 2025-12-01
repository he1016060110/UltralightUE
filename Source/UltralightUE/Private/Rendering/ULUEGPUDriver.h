/*
 * Minimal GPU driver implementation for Ultralight.
 * Since we're using CPU rendering (BitmapSurface), this driver is mostly empty.
 * However, Ultralight requires a GPUDriver to be set for proper initialization.
 */

#pragma once

#include "CoreMinimal.h"
#include "ULUEUltralightIncludes.h"

namespace ultralightue
{

/**
 * Empty GPU driver implementation for CPU rendering mode.
 * Ultralight requires a GPUDriver even when using BitmapSurfaceFactory.
 */
class ULUEGPUDriver : public ultralight::GPUDriver
{
public:
	ULUEGPUDriver() = default;
	virtual ~ULUEGPUDriver() = default;

	// GPU Driver interface - all methods are empty since we use CPU rendering
	virtual void BeginSynchronize() override {}
	virtual void EndSynchronize() override {}

	virtual uint32_t NextTextureId() override { return NextId++; }
	virtual void CreateTexture(uint32_t texture_id, ultralight::RefPtr<ultralight::Bitmap> bitmap) override {}
	virtual void UpdateTexture(uint32_t texture_id, ultralight::RefPtr<ultralight::Bitmap> bitmap) override {}
	virtual void DestroyTexture(uint32_t texture_id) override {}

	virtual uint32_t NextRenderBufferId() override { return NextId++; }
	virtual void CreateRenderBuffer(uint32_t render_buffer_id, const ultralight::RenderBuffer& buffer) override {}
	virtual void DestroyRenderBuffer(uint32_t render_buffer_id) override {}

	virtual uint32_t NextGeometryId() override { return NextId++; }
	virtual void CreateGeometry(uint32_t geometry_id, const ultralight::VertexBuffer& vertices, const ultralight::IndexBuffer& indices) override {}
	virtual void UpdateGeometry(uint32_t geometry_id, const ultralight::VertexBuffer& vertices, const ultralight::IndexBuffer& indices) override {}
	virtual void DestroyGeometry(uint32_t geometry_id) override {}

	virtual void UpdateCommandList(const ultralight::CommandList& list) override {}

private:
	uint32_t NextId = 1;
};

} // namespace ultralightue
