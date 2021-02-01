#pragma once

#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"
#include "src/core/entities/Entity.h"

namespace cm
{
	template<typename T>
	class Component
	{
	public:
		Entity *entity;
		inline static std::vector<T> component_array;
	};

	class RiComp : public Component<RiComp>
	{

	};
}
