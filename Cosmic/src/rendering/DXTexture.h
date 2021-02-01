#pragma once

#include "src/Cosmic.h"
#include "DirectX11.h"

namespace cm
{
	class DXTexture
	{
		friend class GraphicsContext;
		friend class DirectXImmediateRenderer;

	public:
		ID3D11Texture2D *texture = nullptr;
		ID3D11ShaderResourceView *view = nullptr;
		ID3D11SamplerState *sampler = nullptr;

	public:
		void Bind(GraphicsContext *gc, int32 register_index) const;

	public:
		DXTexture();
		~DXTexture();
	};
}