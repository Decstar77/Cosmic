#pragma once
#include "Entity.h"

namespace cm
{
	class Environment : public Entity
	{


	public:
		virtual void Update(const real32 &dt) override;
		virtual void Start() override;

	};
}
