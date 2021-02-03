#pragma once
#include "src/core/entities/Entity.h"

namespace cm
{
	class PointLight : public Entity // @Display
	{
	public:
		bool emitting = true;

		real32 strength = 10.0f;		// @Display
		real32 radius = 1.0f;			// @Display
		Vec3f colour = Vec3f(0.8f);		// @Display-Colour3

		PointLight()
		{
			type = EntityType::POINTLIGHT;
			SetName("PointLight");
			SetBoundingBox(CreateAABBFromCenterRadius(Vec3f(0), Vec3f(0.1f)));
		}
	};
}