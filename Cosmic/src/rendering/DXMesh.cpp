#include "DXMesh.h"
#include "DirectX11Renderer.h"

namespace cm
{
	void DXMesh::Bind(GraphicsContext *gc) const
	{
		GETDEUBBGER();

		uint32 offset = 0;
		DXINFO(gc->context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride_bytes, &offset));
		DXINFO(gc->context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0));
	}

	DXMesh::DXMesh()
	{
	}

	DXMesh::~DXMesh()
	{
	}
}