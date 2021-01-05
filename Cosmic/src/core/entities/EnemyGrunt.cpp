#include "EnemyGrunt.h"
#include "../World.h"


namespace cm
{

	EnemyGrunt::EnemyGrunt()
	{
		this->type = EntityType::ENEMY_DUDE;
	}

	EnemyGrunt::~EnemyGrunt()
	{

	}

	void EnemyGrunt::Start()
	{
		SetMesh(GameState::GetAssetTable()->FindMeshEntry("SM_Character_Junky_Male_01"));
		SetCollider(Collider(CreateCapsuleFromBotTop(Vec3f(0.0f, 0.2f, 0.0f), Vec3f(0, 1.4, 0), 0.4f)));

	}

	void EnemyGrunt::Update(const real32 &dt)
	{
#if 0
		nav_agent.FindPathTo(transform.position, world->player->transform.position);

		Vec3f waypoint = nav_agent.GetCurrentWapoint();
		Vec3f dir = Normalize(waypoint - transform.position);
		transform.position += dir * 6.0f * dt;
		LookAt(waypoint);
		transform.orientation = transform.orientation * EulerToQuat(Vec3f(0, 180, 0));

		if (Equal(transform.position, waypoint, 0.5f))
		{
			nav_agent.PassedWaypoint();
		}
#endif
	}

}
