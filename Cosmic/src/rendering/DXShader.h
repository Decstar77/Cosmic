#pragma once

#include "src/Cosmic.h"
#include "DirectX11.h"

namespace cm
{
	class DXShader
	{
		friend class GraphicsContext;
		friend class DirectXImmediateRenderer;

	public:
		ID3D11VertexShader *vs_shader = nullptr;
		ID3D11PixelShader *ps_shader = nullptr;
		ID3D11InputLayout *layout = nullptr;

	public:
		void Bind(GraphicsContext *gc) const;

	public:
		DXShader();
		~DXShader();
	};
}