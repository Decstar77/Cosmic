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
		Vec3f colour = Vec3f(0.8f);		// @Display

		PointLight()
		{
			type = EntityType::POINTLIGHT;
			SetName("PointLight");
			SetBoundingBox(CreateAABBFromCenterRadius(Vec3f(0), Vec3f(0.1f)));
		}
	};

	class SpotLight : public Entity // @Display
	{
	public:
		bool emitting = true;

		real32 strength = 10.0f;		// @Display
		real32 theta = 1.0f;			// @Display
		Vec3f colour = Vec3f(0.8f);		// @Display
		Vec3f direction;				// @Display

		SpotLight()
		{
			type = EntityType::SPOTLIGHT;
			SetName("SpotLight");
			SetBoundingBox(CreateAABBFromCenterRadius(Vec3f(0), Vec3f(0.1f)));
		}
	};

	class DirectionalLight : public Entity // @Display
	{
	public:
		bool emitting = true;

		real32 strength = 10.0f;			// @Display
		real32 radius = 1.0f;				// @Display
		Vec3f colour = Vec3f(0.8f);			// @Display
		Vec3f direction = Vec3f(1, 1, 1);	// @Display

		DirectionalLight()
		{
			type = EntityType::DIRECTIONALLIGHT;
			SetName("DirectionalLight");
			SetBoundingBox(CreateAABBFromCenterRadius(Vec3f(0), Vec3f(0.1f)));
		}
	};

}