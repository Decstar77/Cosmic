#pragma once

#include "src/Cosmic.h"
#include "src/core/entities/Entity.h"

namespace cm
{
	class Particle
	{
	public:
		real32 life_seconds = 2.0f;
		real32 gravity = 3.8f;

		Vec3f acceleration;
		Vec3f velocity;
		Vec3f position;

		real32 velocity_range_start = 1.0f;
		real32 velocity_range_end = 6.0f;

	public:

		bool Update(const real32 &dt);

		Particle();
		~Particle();
	};

	class ParticleEmitter : public Entity
	{
	public:
		int32 count;
		int32 amount;
		real32 life_seconds;
		std::vector<Particle> partices;

	public:
		virtual void Start() override;
		virtual void Update(const real32 &dt) override;
	};

}