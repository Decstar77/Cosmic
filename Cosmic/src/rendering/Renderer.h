#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"

namespace cm
{
	class Identifiable
	{

	};

	class MeshInstance
	{
	public:
		MeshInstance() {};
		MeshInstance(uint32 index) { this->index = index; }
		inline uint32 GetIndex() const { return index; }
		inline bool IsValid() const { return index != UINT32_MAX; }

	private:
		uint32 index = UINT32_MAX;
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