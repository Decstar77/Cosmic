#pragma once
#include "Entity.h"

namespace cm
{
	class EnemyGrunt : public Entity
	{
	public:
		virtual void Start() override;
		virtual void Update(const real32 &dt) override;

	public:
		EnemyGrunt();
		~EnemyGrunt();

	};
}
