#pragma once

#include "src/core/entities/Entity.h"

namespace cm
{
	class PointLight : public Entity // @Display
	{
	public:
		real32 strength = 10.0f;	// @Display
		real32 radius = 100.0f;		// @Display
		Vec3f colour;				// @Display


		PointLight() { type = EntityType::POINTLIGHT; }
	};
}