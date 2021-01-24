#pragma once
#include "src/core/entities/Entity.h"

namespace cm
{
	class Turret : public Entity // @Display
	{
	public:
		virtual void Start() override;
		virtual void Update(const real32 &dt) override;

	public:
		Turret();
		~Turret();
	};
}
