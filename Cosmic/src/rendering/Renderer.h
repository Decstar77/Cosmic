#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"
#include "src/serialization/FileParsers.h"

namespace cm
{
	class Identifiable
	{

	};

	class MeshInstance
	{
	public:
		MeshMetaData meta_data;
		int32 asset_table_index = -1;
		int32 graphics_table_index = -1;

		int32 index = -1;

		inline bool IsOnGPU() const { return graphics_table_index >= 0; }

	public:
		MeshInstance() {}
		~MeshInstance() {}

	};

	class TextureInstance
	{

	};

	class ShaderInstance
	{
	public:
		ShaderInstance(uint32 index) { this->index = index; }
		inline uint32 GetIndex() { return index; }

	private:
		uint32 index = UINT32_MAX;
	};

	class UniformData
	{

	};

	struct MaterialInstance
	{
		Vec3f diffuse;
		Vec3f specular;
		int32 index;
	};

	class DrawCall
	{
		MeshInstance mesh_instance;
		TextureInstance texture_instance;
		ShaderInstance shader_instance;
		MaterialInstance data;
	};


} // namespace cm

#endif // __RENDERER_H__