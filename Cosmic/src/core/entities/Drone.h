#pragma once

#include "Entity.h"
#include "Camera.h"

namespace cm
{
	class Drone : public Entity  // @Display
	{
	public:
		Vec3f velocity;					// @Display
		Vec3f acceleration;				// @Display

		real32 max_speed = 10.0f;		// @Display
		real32 banking_angle = 25.0f;	// @Display

		void Movement(const real32 &dt);
		void Banking(const real32 dt);
		void Rotating(const real32 &dt);

	public:
		Drone();
		~Drone();

		virtual void Start() override;
		virtual void Update(const real32 &dt) override;
	};
}

