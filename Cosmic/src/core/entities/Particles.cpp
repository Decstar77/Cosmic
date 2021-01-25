#include "Particles.h"
#include "src/core/World.h"

#include "src/Debug.h"

namespace cm
{
	Particle::Particle()
	{
		velocity = RandomPointOnUnitHemisphere() * RandomReal<real32>(velocity_range_start, velocity_range_end);
	}

	Particle::~Particle()
	{

	}

	bool Particle::Update(const real32 &dt)
	{
		//Vec3f accel = acceleration;

		acceleration.y = -gravity;

		velocity += acceleration * dt;

		position += velocity * dt;

		life_seconds -= dt;

		Debug::Push(position, 0.01f);

		return life_seconds < 0;
	}

	void ParticleEmitter::Start()
	{
		count = 0;
		amount = 1000;
		//partices.resize(100);

		this->SetName("Particel emitter");
		this->SetType(EntityType::PARTICEL_EMITTER);

		this->SetCollider(CreateAABBFromCenterRadius(Vec3f(0), Vec3f(1)));
		this->SetMesh(GameState::GetAssetTable()->FindMeshEntry("cube"));

		transform.scale = Vec3(0.1f);
		transform.position.y = 5.0f;
	}

	void ParticleEmitter::Update(const real32 &dt)
	{
		if (partices.size() < amount)
		{
			partices.emplace_back();
		}

		for (int32 i = 0; i < partices.size(); i++)
		{
			Particle &particle = partices.at(i);

			if (particle.Update(dt))
			{
				particle = Particle();
				particle.position = this->transform.position;
			}
		}
	}

}
