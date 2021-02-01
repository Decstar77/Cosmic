#include "DXTexture.h"
#include "DirectX11Renderer.h"


namespace cm
{
	void DXTexture::Bind(GraphicsContext *gc, int32 register_index) const
	{
		GETDEUBBGER();

		DXINFO(gc->context->PSSetShaderResources(register_index, 1, &view));
		DXINFO(gc->context->PSSetSamplers(register_index, 1, &sampler));
	}

	DXTexture::DXTexture()
	{

	}

	DXTexture::~DXTexture()
	{

	}
}