#include "GameCamera.h"
#include "src/core/World.h"

namespace cm
{

	GameCamera::GameCamera()
	{
		SetType(EntityType::CAMERA);
		SetName("GameCamera");
	}

	GameCamera::~GameCamera()
	{

	}

	void GameCamera::Start()
	{
		camera = world->CreateEntity<Camera>();
		camera->SetParent(this);
		camera->SetName("The camera");

		transform.position = Vec3f(0, 15, -10);
		transform.orientation = EulerToQuat(Vec3f(45.0f, 0, 0));
	}

	void GameCamera::Update(const real32 &dt)
	{
		camera->SetAsActiveCamera();
		camera->SetAsMainCamera();
	}


}
