#include "Turret.h"
#include "../World.h"

namespace cm
{
	Turret::Turret()
	{
		type = EntityType::TURRET;
	}

	Turret::~Turret()
	{

	}

	void Turret::Start()
	{
		SetMesh(GameState::GetAssetTable()->FindMeshInstance("SM_Prop_Drone_Insect_01"));
	}

	void Turret::Update(const real32 &dt)
	{

	}

}