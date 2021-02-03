#pragma once
#include "src/core/entities/Entity.h"

namespace cm
{
	class DirectionalLight : public Entity // @Display
	{
	public:
		bool emitting = true;

		real32 strength = 10.0f;			// @Display
		real32 radius = 1.0f;				// @Display
		Vec3f colour = Vec3f(0.8f);			// @Display-Colour3

		DirectionalLight()
		{
			type = EntityType::DIRECTIONALLIGHT;
			SetName("DirectionalLight");
			SetBoundingBox(CreateAABBFromCenterRadius(Vec3f(0), Vec3f(0.1f)));
		}
	};
}