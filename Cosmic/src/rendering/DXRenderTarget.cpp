#include "DXRenderTarget.h"
#include "DirectX11Renderer.h"

namespace cm
{

	void DXRenderTarget::Bind(GraphicsContext *gc)
	{
		GETDEUBBGER();
		DXINFO(gc->context->OMSetRenderTargets(1, &render_target, depth_target));
	}

	void DXRenderTarget::Create(GraphicsContext *gc, const int32 &width, const int32 &height, const bool depth_buffer /*= true*/)
	{
		GETDEUBBGER();

		this->width = width;
		this->height = height;

		D3D11_TEXTURE2D_DESC texture_desc = {};
		texture_desc.Width = width;
		texture_desc.Height = height;
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;
		texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texture_desc.SampleDesc.Count = gc->msaa_sample_count;
		texture_desc.SampleDesc.Quality = 0;
		texture_desc.Usage = D3D11_USAGE_DEFAULT;
		texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture_desc.CPUAccessFlags = 0;
		texture_desc.MiscFlags = 0;

		DXCHECK(gc->device->CreateTexture2D(&texture_desc, NULL, &texture));

		// ----------------------------------------------------------------------------
		// ----------------------------------------------------------------------------

		D3D11_RENDER_TARGET_VIEW_DESC render_target_desc = {};
		render_target_desc.Format = texture_desc.Format;
		render_target_desc.ViewDimension = gc->IsMSAAEnabled() ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		render_target_desc.Texture2D.MipSlice = 0;

		DXCHECK(gc->device->CreateRenderTargetView(texture, &render_target_desc, &render_target));

		// ----------------------------------------------------------------------------
		// ----------------------------------------------------------------------------

		D3D11_SHADER_RESOURCE_VIEW_DESC shader_view_desc = {};
		shader_view_desc.Format = texture_desc.Format;
		shader_view_desc.ViewDimension = gc->IsMSAAEnabled() ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_view_desc.Texture2D.MostDetailedMip = 0;
		shader_view_desc.Texture2D.MipLevels = 1;

		DXCHECK(gc->device->CreateShaderResourceView(texture, &shader_view_desc, &shader_view));

		if (depth_buffer)
		{
			D3D11_TEXTURE2D_DESC depth_ds = {};
			depth_ds.Width = width;
			depth_ds.Height = height;
			depth_ds.MipLevels = 1;
			depth_ds.ArraySize = 1;
			depth_ds.Format = DXGI_FORMAT_D32_FLOAT;
			depth_ds.SampleDesc.Count = gc->msaa_sample_count;
			depth_ds.SampleDesc.Quality = 0;
			depth_ds.Usage = D3D11_USAGE_DEFAULT;
			depth_ds.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			gc->device->CreateTexture2D(&depth_ds, nullptr, &depth_texture);

			D3D11_DEPTH_STENCIL_VIEW_DESC depth_view_dsc = {};
			depth_view_dsc.Format = DXGI_FORMAT_D32_FLOAT;
			depth_view_dsc.ViewDimension = gc->IsMSAAEnabled() ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			depth_view_dsc.Texture2D.MipSlice = 0;
			DXCHECK(gc->device->CreateDepthStencilView(depth_texture, &depth_view_dsc, &depth_target));
		}
	}

	void DXRenderTarget::Clear(GraphicsContext *gc, const Vec4f &colour)
	{
		GETDEUBBGER();

		DXINFO(gc->context->ClearRenderTargetView(render_target, colour.ptr));
		DXINFO(gc->context->ClearDepthStencilView(depth_target, D3D11_CLEAR_DEPTH, 1.0f, 0))
	}
}
