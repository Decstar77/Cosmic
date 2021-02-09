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

		//transform.position = Vec3f(10, 15, -7);
		//transform.orientation = EulerToQuat(Vec3f(45.0f, 0, 0));

		//transform.position = Vec3f(12.5f, 17.5f, -0.2f);
		transform.position = Vec3f(0.0f, 17.5f, -10.2f);
		transform.orientation = Quatf(0.5f, 0.001f, -0.001f, 0.861f);

	}

	void GameCamera::Update(const real32 &dt)
	{
		camera->SetAsActiveCamera();
		camera->SetAsMainCamera();
	}


}
