#pragma once
#include "Entity.h"
#include "Camera.h"

namespace cm
{

	class GameCamera : public Entity
	{
	public:
		Camera *camera;

		GameCamera();
		~GameCamera();


		virtual void Start() override;
		virtual void Update(const real32 &dt) override;

	};

}
