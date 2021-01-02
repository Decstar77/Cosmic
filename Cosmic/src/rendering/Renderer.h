#ifndef __RENDERER_H__
#define __RENDERER_H__

// @NOTE: The reason this is here is to help intelli sense, it doesn't actually do anything
//      : in the sense that we are doing a unity build. The header gaurd will protect us.
#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"

namespace cm
{
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

	struct MaterialInstance
	{
		Vec3f diffuse;
		Vec3f specular;
		int32 index;
	};

	/* Please put this is a renderer.h or somtrhing*/
} // namespace cm

#endif // __RENDERER_H__