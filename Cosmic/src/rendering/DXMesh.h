#pragma once

#include "src/Cosmic.h"
#include "DirectX11.h"

namespace cm
{
	class DXMesh
	{
		friend class GraphicsContext;
		friend class DirectXImmediateRenderer;

	public:
		uint32 index = UINT32_MAX; // @TODO: Remove
		uint32 stride_bytes = UINT32_MAX;
		uint32 index_count = UINT32_MAX;
		uint32 vertex_size = UINT32_MAX;
		ID3D11Buffer *vertex_buffer = nullptr;
		ID3D11Buffer *index_buffer = nullptr;

	public:
		inline bool IsValid() { return vertex_buffer && index_buffer; }

		void Bind(GraphicsContext *gc) const;

	public:

		DXMesh();
		~DXMesh();
	};
}