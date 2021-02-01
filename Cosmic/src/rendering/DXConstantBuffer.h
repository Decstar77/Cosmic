#pragma once
#include "src/Cosmic.h"
#include "DirectX11.h"

#include "src/math/CosmicMath.h"

namespace cm
{
	class DXConstBuffer
	{
		friend class GraphicsContext;
		friend class DirectXImmediateRenderer;

	public:

		ID3D11Buffer *buffer = nullptr;
		int32 current_register_index = -1;
		ShaderStage stage = ShaderStage::INVALID;

		uint32 copy_ptr = 0;
		std::vector<real32> data;

	public:
		void CopyAndUpload(GraphicsContext *gc, const std::vector<real32> &data);
		void Copy(const std::vector<real32> &data);

		inline void ResetCopyPtr() { copy_ptr = 0; }
		void CopyInVec3f(const Vec3f &in);

		void Upload(GraphicsContext *gc);

		void Create(GraphicsContext *gc, const uint32 &size_bytes);
		void Release();

		void Bind(GraphicsContext *gc, ShaderStage stage, int32 register_index);

	public:
		DXConstBuffer();
		~DXConstBuffer();

	};

}