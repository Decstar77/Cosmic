#pragma once
#include "src/core/entities/Entity.h"

namespace cm
{
	class DirectionalLight : public Entity // @Display
	{
	public:
		bool emitting = true;				// @Display
		bool cast_shadow = true;			// @Display

		real32 strength = 10.0f;			// @Display
		real32 radius = 1.0f;				// @Display
		Vec3f colour = Vec3f(0.8f);			// @Display-Colour3

		real32 shadow_near = 1.0f;			// @Display
		real32 shadow_far = 50.0f;			// @Display

		real32 shadow_width = 20.0f;		// @Display
		real32 shadow_height = 20.0f;		// @Display

		DirectionalLight()
		{
			type = EntityType::DIRECTIONALLIGHT;
			SetName("DirectionalLight");
			SetBoundingBox(CreateAABBFromCenterRadius(Vec3f(0), Vec3f(0.1f)));

			this->SetPosition(Vec3f(10));

			this->LookAt(Vec3f(0));
		}

		inline Mat4f CalculateLightView()
		{
			Mat4 view_matrix = this->GetGlobalTransformMatrix();
			view_matrix = Inverse(view_matrix);
			return view_matrix;
		}

		inline Mat4f CalculateLightProjection()
		{
			real32 half_width = (shadow_width / 2.0f);
			real32 half_height = (shadow_height / 2.0f);

			return OrthographicLH(-half_width, half_width, half_height, -half_height, shadow_near, shadow_far);
		}

	};
}