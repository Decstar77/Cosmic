#include "DXShader.h"
#include "DirectX11Renderer.h"

namespace cm
{
	void DXShader::Bind(GraphicsContext *gc) const
	{
		GETDEUBBGER();

		DXINFO(gc->context->VSSetShader(vs_shader, nullptr, 0));
		DXINFO(gc->context->PSSetShader(ps_shader, nullptr, 0));
		DXINFO(gc->context->IASetInputLayout(layout));
	}

	DXShader::DXShader()
	{

	}

	DXShader::~DXShader()
	{

	}
}