#include "DXConstantBuffer.h"
#include "DirectX11Renderer.h"

namespace cm
{
	void DXConstBuffer::CopyAndUpload(GraphicsContext *gc, const std::vector<real32> &data)
	{
		Copy(data);
		Upload(gc);
	}

	void DXConstBuffer::Copy(const std::vector<real32> &data)
	{
		ASSERT(this->data.size() == data.size(), "DXConstBuffer::Copy size not the same");
		this->data = data;
	}

	void DXConstBuffer::CopyInVec3f(const Vec3f &in)
	{
		data.at(copy_ptr) = in.x;
		copy_ptr++;
		data.at(copy_ptr) = in.y;
		copy_ptr++;
		data.at(copy_ptr) = in.z;
		copy_ptr++;
		data.at(copy_ptr) = 0.0f;
		copy_ptr++;
	}

	void DXConstBuffer::Upload(GraphicsContext *gc)
	{
		GETDEUBBGER();
		DXINFO(gc->context->UpdateSubresource(buffer, 0, nullptr, data.data(), 0, 0));
	}

	void DXConstBuffer::Create(GraphicsContext *gc, const uint32 &size_bytes)
	{
		Release();

		GETDEUBBGER();

		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT; //D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = size_bytes;
		desc.StructureByteStride = 0;

		data = std::vector<real32>(size_bytes / sizeof(real32), 0.0f);

		D3D11_SUBRESOURCE_DATA dxdata = {};
		dxdata.pSysMem = data.data();

		DXCHECK(gc->device->CreateBuffer(&desc, &dxdata, &buffer));
	}

	void DXConstBuffer::Release()
	{
		DXRELEASE(buffer);
	}

	void DXConstBuffer::Bind(GraphicsContext *gc, ShaderStage stage, int32 register_index)
	{
		GETDEUBBGER();

		this->stage = stage;
		this->current_register_index = register_index;

		switch (stage)
		{
		case ShaderStage::INVALID:
			break;
		case ShaderStage::VERTEX_SHADER:
			DXINFO(gc->context->VSSetConstantBuffers(register_index, 1, &buffer));
			break;
		case ShaderStage::PIXEL_SHADER:
			DXINFO(gc->context->PSSetConstantBuffers(register_index, 1, &buffer));
			break;
		default:
			break;
		}
	}

	DXConstBuffer::DXConstBuffer()
	{

	}

	DXConstBuffer::~DXConstBuffer()
	{

	}

}