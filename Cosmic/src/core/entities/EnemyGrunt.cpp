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
		//LookAt(world->player->transform.position);
	}

}
