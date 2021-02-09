#pragma once

#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"
#include "DirectX11.h"

namespace cm
{
	class DXRenderTarget
	{
		friend class GraphicsContext;

	public:
		ID3D11Texture2D* texture = nullptr;
		ID3D11Texture2D* depth_texture = nullptr;

		ID3D11RenderTargetView* render_target = nullptr;
		ID3D11DepthStencilView* depth_target = nullptr;

		ID3D11ShaderResourceView* shader_view = nullptr;

		int32 width;
		int32 height;

	public:
		void Bind(GraphicsContext *gc);
		void Create(GraphicsContext *gc, const int32 &width, const int32 &height, const bool depth_buffer = true);
		void Clear(GraphicsContext *gc, const Vec4f &colour);

	public:
		DXRenderTarget() {}
		~DXRenderTarget() {}
	};
}