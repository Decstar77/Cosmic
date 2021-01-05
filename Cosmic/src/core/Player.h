#pragma once
#include "src/Cosmic.h"
#include "src/input/KeyInput.h"
#include "src/input/MouseInput.h"
#include "src/core/entities/Camera.h"
#include "src/core/SoundSystem.h"

namespace cm
{
	class Player : public Entity
	{
	public:
		Camera *camera;
		Entity *weapon; // @TODO: Should this a ref ?

		Capsule collider;
		EntityReference wall;

		Speaker speaker;
		real32 distance_since_last_walk_sound;
		real32 distance_to_make_walk_sound;

		real32 character_length;
		real32 character_eye_height;

		real32 pitch;
		real32 yaw;
		bool grounded;

		Vec3f velocity;
		Vec3f acceleration;

		real32 max_slope_angle;

		real32 walk_speed;
		real32 run_speed;
		real32 resistance;

		real32 jump_power;
		real32 gravity_amount;

		real32 look_speed;

		void PlayerLook(const real32 &dt);
		void PlayerMove(const real32 &dt);

		virtual void Start() override;
		virtual void Update(const real32 &dt) override;

		Vec3f SweepForward(const Vec3f &pos, const Vec3f &velo, real32 t);
		Vec2f QuadracticSolve(const real32 &a, const real32 &b, const real32 &c);
	};
}

