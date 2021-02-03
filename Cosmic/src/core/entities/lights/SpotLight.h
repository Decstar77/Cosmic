#pragma once
#include "src/core/entities/Entity.h"

namespace cm
{
	class SpotLight : public Entity // @Display
	{
	public:
		bool emitting = true;

		real32 strength = 10.0f;			// @Display
		real32 innner = 12.0f;				// @Display
		real32 outter = 15.0f;				// @Display
		Vec3f colour = Vec3f(0.8f);			// @Display-Colour3

		SpotLight()
		{
			type = EntityType::SPOTLIGHT;
			SetName("SpotLight");
			SetBoundingBox(CreateAABBFromCenterRadius(Vec3f(0), Vec3f(0.1f)));
		}
	};
}