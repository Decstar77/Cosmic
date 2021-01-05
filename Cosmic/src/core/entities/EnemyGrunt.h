#pragma once
#include "Entity.h"
#include "src/core/NavMesh.h"
namespace cm
{
	class EnemyGrunt : public Entity
	{
	public:
		NavAgent nav_agent;

	public:
		virtual void Start() override;
		virtual void Update(const real32 &dt) override;

	public:
		EnemyGrunt();
		~EnemyGrunt();

	};
}
