#include "Drone.h"
#include "src/core/World.h"
#include "src/Debug.h"

namespace cm
{
	Drone::Drone()
	{
		SetType(EntityType::DRONE);
	}

	Drone::~Drone()
	{

	}

	void Drone::Update(const real32 &dt)
	{
		// @NOTE: Order is important
		Movement(dt);
		Rotating(dt);
		Banking(dt);
	}

	void Drone::Movement(const real32 &dt)
	{
		Basisf basis = transform.GetBasis();
		basis.forward.y = 0.0f;
		basis.right.y = 0.0f;
		basis.upward = Vec3f(0, 1, 0);

		basis.forward = Normalize(basis.forward);
		basis.right = Normalize(basis.right);

		Vec3f right = Vec3f(1, 0, 0);
		Vec3f upward = Vec3f(0, 1, 0);
		Vec3f forward = Vec3f(0, 0, 1);

		acceleration = Vec3f(0);

		real32 speed = 140.0f;
		if (KeyInput::GetKeyHeldDown(KeyCode::W))
		{
			acceleration += (forward * speed);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::S))
		{
			acceleration += (-1.0f * forward * speed);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::A))
		{
			acceleration += (-1.0f * right * speed);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::D))
		{
			acceleration += (right * speed);
		}

		if (KeyInput::GetKeyHeldDown(KeyCode::Q))
		{
			acceleration += (-1.0f * basis.right * speed);
		}
		if (KeyInput::GetKeyHeldDown(KeyCode::E))
		{
			acceleration += (basis.right * speed);
		}

		acceleration.x += -14.0f * velocity.x;
		acceleration.z += -14.0f * velocity.z;

		velocity = acceleration * dt + velocity;

		velocity = ClampMag(velocity, max_speed);

		transform.position = velocity * dt + transform.position;
	}


	void Drone::Banking(const real32 dt)
	{
#if 1
		Basisf basis = transform.GetBasis();
		//basis.forward.y = 0.0f;
		//basis.right.y = 0.0f;
		//basis.upward = Vec3f(0, 1, 0);

		//basis.forward = Normalize(basis.forward);
		//basis.right = Normalize(basis.right);
#else
		Basisf basis = Transform().GetBasis();
#endif;
		Mat3f ori = basis.mat;

		Vec3f up = Vec3f(0, 1, 0);
		Vec3f motion = Normalize(velocity);

		real32 d = Dot(basis.forward, Vec3f(0, 0, 1));

		Vec3f axis_of_rotation = Cross(up, motion);

		//Debug::Push(basis, transform.position);
		//Debug::Push(transform.position, transform.position + motion);
		//Debug::Push(transform.position, transform.position + axis_of_rotation);

		real32 current_speed = Mag(velocity);
		real32 t = current_speed / max_speed;

		real32 theta = SmootherStep(t) * banking_angle;

		if (!Equal(theta, 0.0f))
			ori = Rotate(ori, theta, axis_of_rotation);

		transform.orientation = Mat3ToQuat(ori);
	}

	void Drone::Rotating(const real32 &dt)
	{
		Ray ray = Camera::GetActiveCamera()->ShootRay(MouseInput::GetCurrentPosition(), Vec2f((real32)Platform::GetClientWidth(), (real32)Platform::GetClientHeight()));
		Plane plane = CreatePlane(transform.position, Vec3f(0, 1, 0));

		real32 t;
		if (RaycastPlane(ray, plane, &t))
		{
			Vec3f point = TravelDownRay(ray, t);
			LookAt(point);
		}
	}

	void Drone::Start()
	{
		SetMesh(GameState::GetAssetTable()->FindMeshInstance("SM_Prop_Atm_01"));
		transform.position = Vec3f(0, 3, 0);
		transform.orientation = EulerToQuat(Vec3f(0, 90, 0));
	}
}
