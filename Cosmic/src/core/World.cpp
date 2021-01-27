#include "World.h"
#include "src/Debug.h"

namespace cm
{


	//
	//	void Player::Update()
	//	{
	//		static real32 t = 0;
	//		if (KeyInput::GetKeyHeldDown(KeyCode::LEFT_SHIFT))
	//		{
	//			t += 0.01f;
	//			t = Clamp(t, 0.0f, 1.0f);
	//			max_speed = Lerp(max_speed, 20.0f, t);
	//		}
	//		else
	//		{
	//			t -= 0.01f;
	//			t = Clamp(t, 0.0f, 1.0f);
	//			max_speed = Lerp(max_speed, 10.0f, t);
	//		}
	//
	//		PlayerLook();
	//		PlayerMove();
	//	}
	//
	//	void Player::Init(World *world)
	//	{
	//		this->world = world;
	//		this->character_length = 1.8f;
	//		this->character_eye_height = 1.6f;
	//
	//		this->speed = 0.0f;
	//		this->walk_speed = 9.0f;
	//		this->run_speed = 14.0f;
	//		this->max_speed = 20.0f;
	//
	//		this->grounded = true;
	//		this->jump_power = 30.0f;
	//		this->gravity_amount = 45.0f;
	//
	//		this->walk_accel = 1.45f;
	//		this->look_speed = 0.05f;
	//
	//
	//		this->pitch = 0.0f;
	//		this->yaw = 90.0f;
	//
	//		collider.center = Vec3f(0, character_length / 2.0f, 0);
	//		collider.radius = Vec3f(0.2f, character_length / 2.0f, 0.2f);
	//
	//		//collider = CreateCapsuleFromBaseTip(Vec3f(0), Vec3f(0, this->character_length, 0), 0.54f);
	//
	//		spacial = world->AddSpacial("player_position", Transform());
	//		//weapon = world->AddSpacial("player_weapon", Transform(1, 0, 0));
	//		weapon = world->AddRenderable("player_weapon", Transform(Vec3f(0.2, -0.25f, 0.451), Vec3f(0, 180, 0)),
	//			GameState::GetAssetTable()->FindMeshEntry("SM_Wep_Rifle_Small_01"));
	//		camera = world->AddCamera("player camera", Transform(Vec3f(0, character_eye_height, 0)), CameraType::PERSPECTIVE);
	//
	//		camera.CameraSetFarPlane(100.0f);
	//
	//		camera.SetParent(&spacial);
	//		weapon.SetParent(&camera);
	//
	//		camera.CameraSetAsMainCamera();
	//	}
	//
	//	void Player::PlayerLook()
	//	{
	//		Vec2f delta = MouseInput::GetMouseDelta();
	//
	//		real32 nyaw = this->yaw - delta.x * this->look_speed;
	//		real32 npitch = this->pitch + delta.y * this->look_speed;
	//
	//		npitch = Clamp(npitch, -89.0f, 89.0f);
	//		this->yaw = Lerp(this->yaw, nyaw, 0.67f);
	//		this->pitch = Lerp(this->pitch, npitch, 0.67f);
	//
	//		Vec3f direction;
	//		direction.x = Cos(DegToRad(this->yaw)) * Cos(DegToRad(this->pitch));
	//		direction.y = Sin(DegToRad(this->pitch));
	//		direction.z = Sin(DegToRad(this->yaw)) * Cos(DegToRad(this->pitch));
	//
	//		Transform transform = camera.SpacialGetLocalTransform();
	//
	//		Mat4f result = LookAtLH(transform.position, transform.position + direction, Vec3f(0, 1, 0));
	//
	//		camera.SpacialSetOrientation(Mat4ToQuat(result));
	//	}
	//
	//	Vec3f Player::SweepForward(const Vec3f &pos, const Vec3f &velo, real32 t)
	//	{
	//		return pos + velo * t;
	//	}
	//
	//	Vec2f Player::QuadracticSolve(const real32 &a, const real32 &b, const real32 &c)
	//	{
	//		Vec2f result(2.0f, 2.0f);
	//
	//		real32 discm = b * b - 4.0f * a * c;
	//		real32 aa = 2.0f * a;
	//		if (discm > 0 && !Equal(aa, 0.0f))
	//		{
	//			result.x = (-b + discm) / aa;
	//			result.y = (-b - discm) / aa;
	//		}
	//
	//		return result;
	//	}
	//
	//	void Player::PlayerMove()
	//	{
	//		Transform player_transform = spacial.SpacialGetLocalTransform();
	//		Basisf basis = camera.SpacialGetLocalTransform().GetBasis();
	//
	//		real32 dtime = 0.016f;
	//
	//		basis.forward.y = 0.0f;
	//		basis.right.y = 0.0f;
	//		basis.upward = Vec3f(0, 1, 0);
	//
	//		basis.forward = Normalize(basis.forward);
	//		basis.right = Normalize(basis.right);
	//
	//
	//		acceleration = Vec3f(0);
	//
	//		speed = 140.f;
	//		if (KeyInput::GetKeyHeldDown(KeyCode::W))
	//		{
	//			acceleration += (basis.forward * speed);
	//		}
	//		if (KeyInput::GetKeyHeldDown(KeyCode::S))
	//		{
	//			acceleration += (-1.0f * basis.forward * speed);
	//		}
	//		if (KeyInput::GetKeyHeldDown(KeyCode::A))
	//		{
	//			acceleration += (-1.0f * basis.right * speed);
	//		}
	//		if (KeyInput::GetKeyHeldDown(KeyCode::D))
	//		{
	//			acceleration += (basis.right * speed);
	//		}
	//
	//		/*real32 vmag = Mag(velocity);
	//		if (!Equal(vmag, 0.0f))
	//		{
	//			real32 vvmag = Min(vmag, max_speed);
	//			velocity = (velocity / vmag) * vvmag;
	//		}
	//*/
	//		acceleration = ClampMag(acceleration, speed);
	//
	//		acceleration += -14.0f * velocity;
	//
	//		velocity = acceleration * dtime + velocity;
	//
	//		bool move = true;
	//		{
	//			Vec3f inv = Vec3f(1.0f) / collider.radius;
	//			Vec3f pos = player_transform.position * inv;
	//			Vec3f vel = 0.5f * acceleration * dtime * dtime + velocity * dtime * inv;
	//
	//			MeshCollider col = wall.GetCollider().mesh;
	//			Debug::Push(col);
	//
	//			real32 ft = 2.0f;
	//
	//			for (Triangle tri : col)
	//			{
	//				Triangle local_tri = CreateTriangle(tri.v0 * inv, tri.v1 * inv, tri.v2 * inv);
	//				Plane plane = CreatePlane(local_tri);
	//
	//				real32 cp = GetPlaneScalar(plane);
	//
	//				real32 nume = 1.0f - Dot(plane.normal, pos) + cp;
	//				real32 demo = Dot(plane.normal, vel);
	//
	//				if (Equal(demo, 0.0f))
	//				{
	//					real32 d = Distance(pos, plane.center);
	//					if (d < 1.0f)
	//					{
	//						Vec3f npos = SweepForward(pos, vel, 0.0f);
	//						npos = npos * collider.radius;
	//						LOG("HERE1");
	//						//Vec3f back = Normalize(-1.0f * vel) * 0.1f;
	//						move = false;
	//						player_transform.position = npos;
	//					}
	//				}
	//				else
	//				{
	//					real32 t = nume / demo;
	//					if (t > 0 && t < 1)
	//					{
	//
	//						//@NOTE: We hit the plane now test if we're in the triangle
	//						Vec3f npos = SweepForward(pos, vel, t);
	//						Vec3f world_npos = npos * collider.radius;
	//						if (IsPointInsideTriangle(local_tri, npos))
	//						{
	//							ft = Min(t, ft);
	//
	//							Vec3f r = vel;
	//							real32 d = Dot(r, plane.normal);
	//							Vec3f reflect = r - d * plane.normal;
	//
	//							vel = vel * collider.radius;
	//							velocity = reflect * collider.radius;
	//
	//							Vec3f back = Normalize(-1.0f * vel) * 0.01f;
	//
	//							move = false;
	//							player_transform.position = world_npos + velocity + back;
	//						}
	//						else
	//						{
	//							{
	//								real32 a = Dot(vel, vel);
	//								real32 b = 2.0f * (Dot(vel, pos - local_tri.v0));
	//								real32 c = MagSqrd(local_tri.v0 - pos) - 1;
	//
	//								Vec2f roots = QuadracticSolve(a, b, c);
	//								t = Min(roots.x, roots.y);
	//							}
	//							{
	//								real32 a = Dot(vel, vel);
	//								real32 b = 2.0f * (Dot(vel, pos - local_tri.v1));
	//								real32 c = MagSqrd(local_tri.v1 - pos) - 1;
	//
	//								Vec2f roots = QuadracticSolve(a, b, c);
	//								t = Min(roots.x, roots.y, t);
	//							}
	//							{
	//								real32 a = Dot(vel, vel);
	//								real32 b = 2.0f * (Dot(vel, pos - local_tri.v2));
	//								real32 c = MagSqrd(local_tri.v2 - pos) - 1;
	//
	//								Vec2f roots = QuadracticSolve(a, b, c);
	//								t = Min(roots.x, roots.y, t);
	//							}
	//
	//							npos = SweepForward(pos, vel, t);
	//							world_npos = npos * collider.radius;
	//
	//							ft = Min(t, ft);
	//
	//							Vec3f r = vel;
	//							real32 d = Dot(r, plane.normal);
	//							Vec3f reflect = r - d * plane.normal;
	//
	//							vel = vel * collider.radius;
	//							velocity = reflect * collider.radius;
	//
	//							Vec3f back = Normalize(-1.0f * vel) * 0.01f;
	//
	//							move = false;
	//							player_transform.position = world_npos + velocity + back;
	//						}
	//					}
	//				}
	//			}
	//
	//			if (ft < 2.0f)
	//			{
	//
	//				////Vec3f r = vel;
	//				////real32 d = Dot(r, plane.normal);
	//				////Vec3f reflect = r - d * plane.normal;
	//
	//				////vel = vel * collider.radius;
	//				////velocity = reflect * collider.radius;
	//				//velocity = 0;
	//
	//				//Vec3f npos = SweepForward(pos, vel, ft);
	//				//Vec3f world_npos = npos * collider.radius;
	//
	//				//Vec3f back = Normalize(-1.0f * vel) * 0.01f;
	//
	//				//move = false;
	//				//player_transform.position = world_npos + velocity + back;
	//			}
	//
	//		}
	//
	//		if (move)
	//		{
	//			//player_transform.position = 0.5f * acceleration * dtime * dtime + velocity * dtime + player_transform.position;
	//			player_transform.position = velocity * dtime + player_transform.position;
	//		}
	//
	//
	//		//velocity += acceleration;
	//
	//		//// 		real32 vvmag = Min(vmag, max_speed);
	//		//// 
	//		//// 		if (!Equal(vmag, 0.0f))
	//		//// 		{
	//		//// 			velocity = (velocity / vmag) * vvmag;
	//		//// 		}
	//
	//		//real32 drag = 0.9f;
	//		//velocity = velocity * Pow(drag, dtime);
	//
	//
	//		//Vec3f friction = -1.0f * velocity;
	//		//friction.x = Pow(friction.x, (vmag / max_speed));
	//		//friction.y = Pow(friction.y, (vmag / max_speed));
	//		//friction.z = Pow(friction.z, (vmag / max_speed));
	//
	//
	//		//spacial.transform.position += ri.velocity * delta_time;
	//
	//
	//		//std::vector<EntityReference> entities = world->CreateCollection([](EntityReference ref) {
	//		//	return ref.HasCollider();
	//		//});
	//
	//		//for (EntityReference &ref : entities)
	//		//{
	//		//	OBB col = ref.GetCollider().obb;
	//		//}
	//
	//		//Collider sphere_collider = CreateSphere(player_transform.position + Vec3f(0, collider.radius, 0), collider.radius);
	//		//Collider mesh_collider = test_collider.GetCollider();
	//
	//		//IntersectionInfo info = {};
	//		//if (CheckIntersection(sphere_collider.sphere, mesh_collider.mesh, &info))
	//		//{
	//		//	//entity_sphere.SpacialAddPosition(info.normal * info.penetration);
	//
	//		//	player_transform.position += info.normal * info.penetration;
	//
	//		//	Debug::Push(info.close_point, info.close_point + info.normal);
	//		//}
	//
	//		//Ray floor_ray = CreateRay(player_transform.position + Vec3f(0, 0.1f, 0), Vec3f(0, -1, 0));
	//		////AABB floor_collider = CreateAABBFromCenterRadius(0, Vec3f(10, 0, 10));
	//		//Plane floor_plane = CreatePlane(0, Vec3f(0, 1, 0));
	//		//Plane other_plane = CreatePlane(Vec3f(0, 0, 3), Vec3f(0, 1, -1));
	//		//Debug::Push(other_plane);
	//		//Debug::Push(floor_ray);
	//
	//		//real32 dist = 0.0f;
	//		//if (RaycastPlane(floor_ray, floor_plane, &dist))
	//		//{
	//		//	if (dist > 0.15f)
	//		//	{
	//
	//		//	}
	//		//	player_transform.position.y -= dist + 0.1f;
	//		//}
	//		//else
	//		//{
	//		//	floor_ray.direction = -1.0f * floor_ray.direction;
	//		//	if (RaycastPlane(floor_ray, floor_plane, &dist))
	//		//	{
	//		//		player_transform.position.y += dist - 0.1f;
	//		//	}
	//		//}
	//		//if (move)
	//		spacial.SpacialSetTransform(player_transform);
	//	}
	//
	//	Player::Player()
	//	{
	//		//camera.transform.parent = &this->transform;
	//		//camera.transform.position = Vec3f(0, character_eye_height, 0);
	//	}
	//
	//	Player::~Player()
	//	{
	//
	//	}
	//
	//	EntityReference World::CreateEntityReference(BaseEntity *entity)
	//	{
	//		EntityReference ref = {};
	//		ref.index = entity->index;
	//		ref.id = entity->id;
	//		ref.world = this;
	//
	//		return ref;
	//	}
	//
	//
	//	std::vector<EntityReference> World::CreateCollection(bool(*comparator)(EntityReference) /*= nullptr*/)
	//	{
	//		std::vector<EntityReference> collection; // @TODO: Can we make an educated guess on the size that this is going to be that we reserve from the start
	//		int32 entities_chommped = 0;
	//
	//		for (int32 i = 1; i < entities.size(); i++)
	//		{
	//			BaseEntity *base = &entities.at(i);
	//			if (base->InUse())
	//			{
	//				entities_chommped++;
	//
	//				EntityReference ref = CreateEntityReference(base);
	//
	//				if (comparator)
	//				{
	//					if (comparator(ref))
	//					{
	//						collection.push_back(ref);
	//					}
	//				}
	//				else
	//				{
	//					collection.push_back(ref);
	//				}
	//			}
	//
	//			// @NOTE: Early out
	//			if (entities_chommped >= entity_count)
	//			{
	//				break;
	//			}
	//		}
	//
	//		return collection;
	//	}
	//
	//	BaseEntity* World::GetBaseEntity(EntityReference entity)
	//	{
	//		BaseEntity *base = nullptr;
	//
	//		if (entity.IsValid())
	//		{
	//			base = &entities.at(entity.index);
	//		}
	//
	//		return base;
	//	}
	//
	//	EntityReference World::AddEntity(const String &name)
	//	{
	//		int32 index = GetNextFree();
	//		entity_count++;
	//
	//		BaseEntity entity = {};
	//
	//		entity.id = entity_next_id;
	//		entity.index = index;
	//		entity.name = name;
	//
	//		entity_next_id++;
	//		entity.type = EntityType::INVALID;
	//
	//		// @NOTE: Add the base entity;
	//		entities.at(entity.index) = entity;
	//
	//		EntityReference ref = CreateEntityReference(&entity);
	//
	//		return ref;
	//	}
	//
	//	EntityReference World::AddSpacial(const String &name, const Transform &transform)
	//	{
	//		EntityReference entity = AddEntity(name);
	//		entities.at(entity.index).has_spacial = true;
	//
	//		Spacial *spacial = &spacials.at(entity.index);
	//		spacial->transform = transform;
	//		spacial->bounding_box = CreateAABBFromCenterRadius(transform.position, Vec3f(1));
	//
	//		return entity;
	//	}
	//
	//	EntityReference World::AddCollider(const String &name, const Transform &transform, const Collider &collider)
	//	{
	//		EntityReference entity = AddSpacial(name, transform);
	//
	//		entities.at(entity.index).has_collider = true;
	//		colliders.at(entity.index) = collider;
	//
	//		entity.ComputeCollider();
	//
	//		return entity;
	//	}
	//
	//	EntityReference World::AddCamera(const String &name, const Transform &transform, const CameraType &type)
	//	{
	//		EntityReference entity = AddSpacial(name, transform);
	//		entities.at(entity.index).has_camera = true;
	//		spacials.at(entity.index).bounding_box = CreateAABBFromCenterRadius(transform.position, Vec3f(0.1f));
	//
	//		Camera *camera = &cameras.at(entity.index);
	//		camera->type = type;
	//		camera->dfovy = 45.0f;
	//		camera->aspect = 1.77f;
	//		camera->znear = 0.05f;
	//		camera->zfar = 1000.0f;
	//
	//		if (!active_camera_index)
	//		{
	//			active_camera_index = entity.index;
	//		}
	//
	//		return entity;
	//	}
	//
	//	EntityReference World::AddRenderable(const String &name, const Transform &transform, const MeshEntry &mesh_entry)
	//	{
	//		EntityReference entity = AddSpacial(name, transform);
	//		entities.at(entity.index).has_rendererable = true;
	//		renderables.at(entity.index).mesh_entry = mesh_entry;
	//
	//
	//		AABB mesh_bounding_box = GameState::GetAssetTable()->GetMeshMetaData(mesh_entry).bounding_box;
	//		spacials.at(entity.index).bounding_box = mesh_bounding_box;
	//
	//
	//		return entity;
	//	}
	//
	//	EntityReference World::AddRigidbody(const String &name, const Transform &transform, const MeshEntry &mesh_entry)
	//	{
	//		EntityReference entity = AddRenderable(name, transform, mesh_entry);
	//		entities.at(entity.index).has_rigidbody = true;
	//		rigidbodies.at(entity.index).collider = CreateSphere(transform.position, 2.0f);
	//		rigidbodies.at(entity.index).mass = 20.0f;
	//
	//		return entity;
	//	}
	//
	//	EntityReference World::AddEnvironmentEntity(const String &name, const Transform &transform, const MeshEntry &mesh_entry)
	//	{
	//		EntityReference entity = AddRenderable(name, transform, mesh_entry);
	//		entities.at(entity.index).type = EntityType::ENVIRONMENT;
	//		entities.at(entity.index).has_collider = true;
	//
	//		AABB mesh_bounding_box = GameState::GetAssetTable()->GetMeshMetaData(mesh_entry).bounding_box;
	//
	//		colliders.at(entity.index).type = ColliderType::OBB;
	//		colliders.at(entity.index).obb = CreateOBB(GetAABBCenter(mesh_bounding_box), GetAABBRadius(mesh_bounding_box));
	//
	//		return entity;
	//	}
	//
	//	EntityReference World::AddTurrentEntity(const String &name, const Transform &transform)
	//	{
	//		EntityReference entity = AddRenderable(name, transform, GameState::GetAssetTable()->FindMeshEntry("cube"));
	//		entities.at(entity.index).type = EntityType::TURRET;
	//
	//		return entity;
	//	}
	//
	//	Spacial* World::GetEntitySpacial(const EntityReference &entity)
	//	{
	//		Spacial *result = nullptr;
	//		int32 id = entities.at(entity.index).id;
	//
	//		if (id == entity.id)
	//		{
	//			result = &spacials.at(entity.index);
	//		}
	//
	//		return result;
	//	}
	//
	//	void World::RemoveEntity(const EntityReference &entity)
	//	{
	//		// @TODO: Check for active cam
	//		// @TODO: remove children too!!
	//		int32 id = entities.at(entity.index).id;
	//		if (id == entity.id)
	//		{
	//			entities.at(entity.index) = {};
	//			freelist.at(free_count) = entity.index;
	//			free_count++;
	//		}
	//
	//		ToString(EntityType::COUNT);
	//	}
	//
	//
	//	void World::SetActiveCamera(EntityReference *entity)
	//	{
	//		ASSERT(entity->IsValid(), "Camera entity is not valid");
	//		ASSERT(entity->HasCamera(), "Camera entity is not a camera");
	//		active_camera_index = entity->index;
	//	}
	//
	//	void World::SetMainCamera(EntityReference *entity)
	//	{
	//		ASSERT(entity->IsValid(), "Camera entity is not valid");
	//		ASSERT(entity->HasCamera(), "Camera entity is not a camera");
	//		main_camera_index = entity->index;
	//	}
	//
	//	void World::UpdateEntities()
	//	{
	//		player.Update();
	//		std::vector<EntityReference> valid_entities = CreateCollection();
	//
	//		for (EntityReference &entity : valid_entities)
	//		{
	//			if (entity.GetType() == EntityType::TURRET)
	//			{
	//				Spacial *spacial = &spacials.at(entity.index);
	//				spacial->transform.LookAtLH(player.spacial.SpacialGetPosition());
	//			}
	//		}
	//
	//		StepPhysics();
	//	}
	//
	//	void World::PostUpdate()
	//	{
	//		std::vector<EntityReference> valid_entities = CreateCollection();
	//
	//		for (EntityReference &entity : valid_entities)
	//		{
	//			Spacial *spacial = &spacials.at(entity.index);
	//			spacial->has_moved_this_frame = false;
	//		}
	//	}
	//
	//	void World::StepPhysics()
	//	{
	//		std::vector<EntityReference> entities = CreateCollection([](EntityReference ref) {return ref.HasRigidBody(); });
	//		for (EntityReference &ref : entities)
	//		{
	//			Spacial &spacial = spacials.at(ref.index);
	//			RigidBody &ri = rigidbodies.at(ref.index);
	//
	//			ri.forces.y += ri.mass * -9.81f;
	//
	//			ri.velocity += ri.forces / ri.mass * delta_time;
	//			spacial.transform.position += ri.velocity * delta_time;
	//
	//			ri.forces = Vec3f(0);
	//		}
	//
	//		for (EntityReference &ref : entities)
	//		{
	//			Spacial &spacial = spacials.at(ref.index);
	//			RigidBody &ri = rigidbodies.at(ref.index);
	//
	//			if (spacial.transform.position.y < 0.0f)
	//			{
	//				//ri.forces.y = 1000.0f;
	//			}
	//		}
	//
	//	}
	//
	//	World::World()
	//	{
	//		entity_next_id = 0;
	//		main_camera_index = 0;
	//		active_camera_index = 0;
	//		entity_count = -1; // @NOTE: Offset the invalid entity
	//		free_count = MAX_ENTITY_COUNT;
	//
	//		for (int32 i = 0; i < MAX_ENTITY_COUNT; i++)
	//		{
	//			freelist.at(i) = MAX_ENTITY_COUNT - i - 1;
	//			entities.at(i) = {};
	//		}
	//
	//		AddEntity("INVALID ENTITY");
	//
	//		player.Init(this);
	//	}
	//
	//	World::~World()
	//	{
	//
	//	}
	//
	//	int32 World::GetNextFree()
	//	{
	//		free_count--;
	//		ASSERT(free_count >= 0, "We don't have enough space for another entity");
	//
	//		int32 index = freelist.at(free_count);
	//
	//		return index;
	//	}
	//
	//	void World::UpdateEntity(int32 index)
	//	{
	//		Spacial *result = &spacials.at(index);
	//		//result->transform.LookAtLH(player.transform.position);
	//	}
	//
	//	EntityReference World::GetActiveCamera()
	//	{
	//		EntityReference camera = {};
	//
	//		if (active_camera_index)
	//		{
	//			BaseEntity *entity = &entities.at(active_camera_index);
	//
	//			camera = CreateEntityReference(entity);
	//		}
	//		else
	//		{
	//			// TODO: Log warrning
	//		}
	//
	//		return camera;
	//	}
	//
	//	cm::EntityReference World::GetMainCamera()
	//	{
	//		EntityReference camera = {};
	//
	//		if (main_camera_index)
	//		{
	//			BaseEntity *entity = &entities.at(main_camera_index);
	//
	//			camera = CreateEntityReference(entity);
	//		}
	//		else
	//		{
	//			// TODO: Log warrning
	//		}
	//
	//		return camera;
	//	}
	//
	//	static void DoToXMLTree(XMLElement *el, EntityCollection collection)
	//	{
	//		for (EntityReference &ref : collection)
	//		{
	//			String name = ref.GetName();
	//			std::replace(name.begin(), name.end(), ' ', '_');
	//
	//			XMLElement *xml_child = el->AddChild(name);
	//
	//			xml_child->AddChild(ToString(ref.GetType()));
	//
	//			if (ref.HasSpacial())
	//			{
	//				XMLElement *comp = xml_child->AddChild("Spacial");
	//				comp->AddAttribute("CoreComponent", "true");
	//
	//				XMLElement *pos = comp->AddChild("Position");
	//				XMLElement *ort = comp->AddChild("Orientation");
	//				XMLElement *scl = comp->AddChild("Scale");
	//
	//				Transform t = ref.SpacialGetLocalTransform();
	//				pos->data = ToString(t.position);
	//				ort->data = ToString(t.orientation);
	//				scl->data = ToString(t.scale);
	//			}
	//
	//			if (ref.HasRendererable())
	//			{
	//				XMLElement *comp = xml_child->AddChild("Renderering");
	//				comp->AddAttribute("CoreComponent", "true");
	//
	//				XMLElement *mesh = comp->AddChild("Mesh ");
	//				mesh->data = GameState::GetAssetTable()->mesh_meta_data.at(ref.GetMesh()).name;
	//			}
	//
	//			if (ref.HasCamera())
	//			{
	//				XMLElement *comp = xml_child->AddChild("Camera");
	//				comp->AddAttribute("CoreComponent", "true");
	//
	//				Camera cam = ref.CameraGetCamera();
	//
	//				XMLElement *c1 = comp->AddChild("Type", ToString(cam.type));
	//				XMLElement *c2 = comp->AddChild("Dfov", std::to_string(cam.dfovy));
	//				XMLElement *c3 = comp->AddChild("Aspect", std::to_string(cam.aspect));
	//				XMLElement *c4 = comp->AddChild("ZFar", std::to_string(cam.zfar));
	//				XMLElement *c5 = comp->AddChild("ZNear", std::to_string(cam.znear));
	//			}
	//
	//			if (ref.HasChild())
	//			{
	//				DoToXMLTree(xml_child, ref.GetChildren());
	//			}
	//
	//		}
	//	}
	//
	//	XMLTree World::ToXMLTree()
	//	{
	//		XMLTree xml_tree("World");
	//		XMLElement *root = xml_tree.GetRoot();
	//
	//		std::vector<EntityReference> xml_entities = CreateCollection([](EntityReference ref) {
	//			return !ref.HasParent();
	//		});
	//
	//
	//		DoToXMLTree(root, xml_entities);
	//
	//		return xml_tree;
	//	}
	//
	//	bool EntityReference::IsValid()
	//	{
	//		bool result = false;
	//
	//		if (id)
	//		{
	//			int32 world_id = world->entities.at(index).id;
	//			result = id == world_id;
	//		}
	//
	//		return result;
	//
	//	}
	//
	//	bool EntityReference::HasParent()
	//	{
	//		BaseEntity *entity = world->GetBaseEntity(*this); // @NOTE: Should'nt be null based on assumptions
	//
	//		ASSERT(entity, "HasParent was called on an invalid entity");
	//
	//		return entity->parent;
	//	}
	//
	//	bool EntityReference::HasChild()
	//	{
	//		BaseEntity *entity = world->GetBaseEntity(*this); // @NOTE: Should'nt be null based on assumptions
	//
	//		ASSERT(entity, "HasChild was called on an invalid entity");
	//
	//		return entity->child;
	//	}
	//
	//	bool EntityReference::HasSibling()
	//	{
	//		BaseEntity *entity = world->GetBaseEntity(*this); // @NOTE: Should'nt be null based on assumptions
	//
	//		ASSERT(entity, "HasSibling was called on an invalid entity");
	//
	//		return entity->sibling;
	//	}
	//
	//	bool EntityReference::HasSpacial()
	//	{
	//		bool result = false;
	//
	//		if (IsValid())
	//		{
	//			result = world->entities.at(index).has_spacial;
	//		}
	//		else
	//		{
	//			// @TODO: Log warrning
	//		}
	//
	//		return result;
	//	}
	//
	//	bool EntityReference::HasCamera()
	//	{
	//		bool result = false;
	//
	//		if (IsValid())
	//		{
	//			result = world->entities.at(index).has_camera;
	//		}
	//		else
	//		{
	//			// @TODO: Log warrning
	//		}
	//
	//		return result;
	//	}
	//
	//	bool EntityReference::HasCollider()
	//	{
	//		CHECK_ENTITY();
	//		//ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		return world->entities.at(index).has_collider;
	//	}
	//
	//	bool EntityReference::HasRendererable()
	//	{
	//		CHECK_ENTITY();
	//		//ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		return world->entities.at(index).has_rendererable;
	//	}
	//
	//	int32 EntityReference::GetId()
	//	{
	//		int32 id = 0;
	//
	//		BaseEntity *me = world->GetBaseEntity(*this);
	//
	//		if (me)
	//		{
	//			id = me->id;
	//		}
	//
	//		return id;
	//	}
	//
	//	String EntityReference::GetName()
	//	{
	//		String name = "INVALID";
	//		BaseEntity *me = world->GetBaseEntity(*this);
	//
	//		if (me)
	//		{
	//			name = me->name;
	//		}
	//
	//		return name;
	//	}
	//
	//	EntityType EntityReference::GetType()
	//	{
	//		return world->entities.at(index).type;
	//	}
	//
	//	void EntityReference::SetParent(EntityReference *entity)
	//	{
	//		BaseEntity *me = world->GetBaseEntity(*this);
	//
	//		ASSERT(me, "SetParent me is an invalid entities");
	//
	//		// @NOTE: Do we have an existing parent
	//		if (me->parent)
	//		{
	//			// @NOTE: Grab the existing parent
	//			BaseEntity *other_parent = &world->entities.at(me->parent);
	//			if (other_parent)
	//			{
	//				// @NOTE: prev -> me -> next
	//				BaseEntity *prev = nullptr;
	//				BaseEntity *next = nullptr;
	//
	//				// @NOTE: Find prev and next
	//				BaseEntity *child = &world->entities.at(other_parent->child);
	//				while (child->index)
	//				{
	//					if (child->sibling == me->index)
	//					{
	//						prev = child;
	//					}
	//					if (me->sibling == child->index)
	//					{
	//						next = child;
	//					}
	//					child = &world->entities.at(child->sibling);
	//				}
	//
	//				if (!prev && !next) // @NOTE: me was the only child
	//				{
	//					other_parent->child = 0;
	//				}
	//				else if (prev && !next) // @NOTE: me was the last child
	//				{
	//					prev->sibling = 0;
	//				}
	//				else if (!prev && next) // @NOTE: me the the first child
	//				{
	//					other_parent->child = next->index;
	//				}
	//				else if (prev && next) // @NOTE: me was a middle child
	//				{
	//					prev->sibling = next->index;
	//				}
	//				else
	//				{
	//					ASSERT(0, "SetParent went wrong");
	//				}
	//			}
	//			else
	//			{
	//				ASSERT(0, "SetParent went wrong");
	//			}
	//		}
	//
	//		if (entity)
	//		{
	//			BaseEntity *parent = world->GetBaseEntity(*entity);
	//			me->parent = parent->index;
	//
	//			// @NOTE: He new parent has child place me at the back
	//			if (parent->child)
	//			{
	//				BaseEntity *child = &world->entities.at(parent->child);
	//
	//				while (child)
	//				{
	//					if (child->sibling)
	//					{
	//						child = &world->entities.at(child->sibling);
	//					}
	//					else
	//					{
	//						break;
	//					}
	//				}
	//
	//				child->sibling = me->index;
	//			}
	//			else // @NOTE: New parent has no child
	//			{
	//				parent->child = me->index;
	//			}
	//		}
	//		else
	//		{
	//			me->parent = 0;
	//		}
	//
	//		me->sibling = 0;
	//	}
	//
	//	void EntityReference::AddChild(EntityReference *entity)
	//	{
	//		ASSERT(IsValid(), "AddChild entity is invalid");
	//		entity->SetParent(this);
	//	}
	//
	//	EntityReference EntityReference::GetParent()
	//	{
	//		EntityReference ref = {};
	//
	//		BaseEntity *me = world->GetBaseEntity(*this); // @NOTE: Can't be null due to assumptions
	//
	//		ASSERT(me, "GetParent was called on an invalid entity");
	//
	//		ASSERT(HasParent(), "GetParent was called but we don't have a parent");
	//
	//		BaseEntity *parent = &world->entities.at(me->parent);
	//
	//		ref = world->CreateEntityReference(parent);
	//
	//		return ref;
	//	}
	//
	//	EntityReference EntityReference::GetFirstChild()
	//	{
	//		BaseEntity *me = world->GetBaseEntity(*this); // @NOTE: Can't be null due to assumptions
	//
	//		ASSERT(me, "GetFirstChild was called on an invalid entity");
	//
	//		ASSERT(HasChild(), "GetFirstChild was called but we don't have a child");
	//
	//		BaseEntity *child = &world->entities.at(me->child);
	//
	//		EntityReference ref = world->CreateEntityReference(child);
	//
	//		return ref;
	//	}
	//
	//	EntityReference EntityReference::GetSibling()
	//	{
	//		BaseEntity *me = world->GetBaseEntity(*this); // @NOTE: Can't be null due to assumptions
	//
	//		ASSERT(me, "GetSibling was called on an invalid entity");
	//
	//		ASSERT(HasSibling(), "GetSibling was called but we don't have a sibling");
	//
	//		BaseEntity *sib = &world->entities.at(me->sibling);
	//
	//		EntityReference ref = world->CreateEntityReference(sib);
	//
	//		return ref;
	//	}
	//
	//	EntityCollection EntityReference::GetChildren()
	//	{
	//		CHECK_ENTITY();
	//
	//		EntityCollection collection;
	//
	//		if (HasChild())
	//		{
	//			EntityReference child = GetFirstChild();
	//			collection.emplace_back(child);
	//
	//			while (child.HasSibling())
	//			{
	//				EntityReference sib = child.GetSibling();
	//				collection.emplace_back(sib);
	//				child = sib;
	//			}
	//		}
	//
	//		return collection;
	//	}
	//
	//	Transform EntityReference::SpacialGetLocalTransform()
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		Transform t = world->spacials.at(index).transform;
	//
	//		return t;
	//	}
	//
	//	Transform EntityReference::SpacialGetGlobalTransform()
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		Mat4f global_mat = SpacialGetGlobalTransformMatrix();
	//
	//		Transform t = Transform(global_mat);
	//
	//		return t;
	//	}
	//
	//	Mat4f EntityReference::SpacialGetLocalTransformMatrix()
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		Mat4f mat = world->spacials.at(index).transform.CalculateTransformMatrix();
	//
	//		return mat;
	//	}
	//
	//	Mat4f EntityReference::SpacialGetGlobalTransformMatrix()
	//	{
	//		ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		Mat4f me = Mat4f();
	//
	//		if (IsValid())
	//		{
	//			me = SpacialGetLocalTransformMatrix();
	//			if (HasParent())
	//			{
	//				EntityReference next = GetParent();
	//
	//				while (next.IsValid())
	//				{
	//					Mat4f other = next.SpacialGetLocalTransformMatrix();
	//
	//					me = me * other;
	//
	//					if (next.HasParent())
	//					{
	//						next = next.GetParent();
	//					}
	//					else
	//					{
	//						break;
	//					}
	//				}
	//			}
	//		}
	//		else
	//		{
	//			// @TODO: Log warring
	//		}
	//
	//		return me;
	//	}
	//
	//	void EntityReference::SpacialSetTransform(const Transform &transform)
	//	{
	//		CHECK_ENTITY();
	//
	//		Spacial *spacial = &world->spacials.at(index);
	//
	//		spacial->transform = transform;
	//		spacial->has_moved_this_frame = true;
	//	}
	//
	//	void EntityReference::SpacialAddPosition(const Vec3f &position)
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		Spacial *spacial = &world->spacials.at(index);
	//
	//		spacial->transform.position += position;
	//		spacial->has_moved_this_frame = true;
	//	}
	//
	//	Vec3f EntityReference::SpacialGetPosition()
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		Vec3f position = world->spacials.at(index).transform.position;
	//
	//		return position;
	//	}
	//
	//	AABB EntityReference::SpacialGetGlobalBoundingBox()
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		Spacial *spacial = &world->spacials.at(index);
	//		Transform transform = SpacialGetGlobalTransform();
	//
	//		AABB bb = UpdateAABB(spacial->bounding_box,
	//			transform.position, transform.orientation, transform.scale);
	//
	//
	//		return bb;
	//	}
	//
	//	AABB EntityReference::SpacialGetLocalBoundingBox()
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		AABB bb = world->spacials.at(index).bounding_box;
	//
	//		return bb;
	//	}
	//
	//	void EntityReference::SpacialSetPosition(const Vec3f &position)
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		Spacial *spacial = &world->spacials.at(index);
	//		spacial->transform.position = position;
	//		spacial->has_moved_this_frame = true;
	//	}
	//
	//	void EntityReference::SpacialSetOrientation(const Quatf &orientation)
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasSpacial(), "Entity doesn't have spacial component");
	//
	//		Spacial *spacial = &world->spacials.at(index);
	//
	//		spacial->transform.orientation = orientation;
	//		spacial->has_moved_this_frame = true;
	//	}
	//
	//	void EntityReference::SetMesh(const MeshEntry &entry)
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasSpacial(), "EntityReference::SetMesh, Entity doesn't have a spacal");
	//		world->entities.at(index).has_rendererable = true;
	//		world->renderables.at(index).mesh_entry = entry;
	//
	//		AABB mesh_bounding_box = GameState::GetAssetTable()->GetMeshMetaData(entry).bounding_box;
	//
	//		Spacial *spacial = &world->spacials.at(index);
	//		spacial->bounding_box = mesh_bounding_box;
	//	}
	//
	//	MeshEntry EntityReference::GetMesh()
	//	{
	//		CHECK_ENTITY();
	//
	//		return world->renderables.at(index).mesh_entry;
	//	}
	//
	//	Mat4f EntityReference::CameraCalculateProjectionMatrix()
	//	{
	//		ASSERT(HasCamera(), "Entity doesn't have camera component");
	//
	//		Mat4f proj = Mat4f();
	//
	//		if (IsValid())
	//		{
	//			Camera *camera = &world->cameras.at(index);
	//			Mat4f projection_matrix = PerspectiveLH(DegToRad(camera->dfovy), camera->aspect, camera->znear, camera->zfar);
	//
	//			return projection_matrix;
	//		}
	//		else
	//		{
	//			// @TODO: Log warning;
	//		}
	//
	//		return proj;
	//	}
	//
	//	Mat4f EntityReference::CameraCalculateViewMatrix()
	//	{
	//		ASSERT(HasCamera(), "Entity doesn't have camera component");
	//
	//		Mat4f view = Mat4f();
	//
	//		if (IsValid())
	//		{
	//			Mat4f mat = SpacialGetGlobalTransformMatrix();
	//
	//			view = Inverse(mat);
	//		}
	//		else
	//		{
	//			// @TODO: Log warning;
	//		}
	//
	//		return view;
	//	}
	//
	//	Camera EntityReference::CameraGetCamera()
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasCamera(), "Entity doesn't have camera component");
	//
	//		return world->cameras.at(index);
	//	}
	//
	//	void EntityReference::CameraSetActiveCamera()
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasCamera(), "Entity doesn't have camera component");
	//
	//		world->SetActiveCamera(this);
	//	}
	//
	//	void EntityReference::CameraSetAsMainCamera()
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasCamera(), "Entity doesn't have camera component");
	//
	//		world->SetMainCamera(this);
	//	}
	//
	//	void EntityReference::CameraSetFarPlane(const real32 &zfar)
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasCamera(), "Entity doesn't have camera component");
	//
	//		world->cameras.at(index).zfar = zfar;
	//	}
	//
	//	Ray EntityReference::CameraShootRay(const Vec2f &mouse_position, const Vec2f &window_dimenions)
	//	{
	//		ASSERT(HasCamera(), "Entity doesn't have camera component");
	//
	//		Ray ray;
	//
	//		if (IsValid())
	//		{
	//			Mat4f proj = CameraCalculateProjectionMatrix();
	//			Mat4f view = CameraCalculateViewMatrix();
	//			Transform transform = SpacialGetLocalTransform();
	//
	//			Vec4f normal_coords = GetNormalisedDeviceCoordinates(window_dimenions.x, window_dimenions.y, mouse_position.x, mouse_position.y);
	//			Vec4f view_coords = ToViewCoords(proj, normal_coords);
	//
	//			// @NOTE: This 1 ensure we a have something pointing in to the screen
	//			view_coords = Vec4f(view_coords.x, view_coords.y, 1, 0);
	//			Vec3f world_coords = ToWorldCoords(view, view_coords);
	//
	//
	//			ray.origin = transform.position;
	//			ray.direction = Normalize(Vec3f(world_coords.x, world_coords.y, world_coords.z));
	//		}
	//		else
	//		{
	//			// @TODO: Log warning;
	//		}
	//
	//		return ray;
	//	}
	//
	//	Collider EntityReference::GetCollider()
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasCollider(), "Entity doesn't have collder component");
	//
	//		Spacial *spacial = &world->spacials.at(index);
	//		Collider *cached_collider = &world->cached_colliders.at(index);
	//
	//		if (spacial->has_moved_this_frame)
	//		{
	//			ComputeCollider();
	//		}
	//
	//		return *cached_collider;
	//	}
	//
	//	void EntityReference::MoveAndCollide(const Vec3f &delta)
	//	{
	//		CHECK_ENTITY();
	//		ASSERT(HasCollider(), "Entity doesn't have collder component");
	//
	//
	//
	//
	//
	//	}
	//
	//	bool EntityReference::HasRigidBody()
	//	{
	//		CHECK_ENTITY();
	//
	//		return world->entities.at(index).has_rigidbody;
	//	}
	//
	//	void EntityReference::ComputeCollider()
	//	{
	//		Spacial *spacial = &world->spacials.at(index);
	//		Collider *cached_collider = &world->cached_colliders.at(index);
	//		Collider *local_collider = &world->colliders.at(index);
	//
	//		cached_collider->type = local_collider->type;
	//		switch (local_collider->type)
	//		{
	//		case ColliderType::PLANE:
	//			cached_collider->plane.center = spacial->transform.position;
	//			cached_collider->plane.normal = local_collider->plane.normal;
	//			//cached_collider->plane.normal = spacial->transform.GetBasis().forward;
	//			break;
	//		case ColliderType::SPHERE:
	//			cached_collider->sphere.data = Vec4f(spacial->transform.position, local_collider->sphere.data.w);
	//			break;
	//		case ColliderType::AABB:
	//			cached_collider->aabb = UpdateAABB(local_collider->aabb, spacial->transform.position, spacial->transform.orientation);
	//			break;
	//		case ColliderType::OBB:
	//			cached_collider->obb.center = spacial->transform.position;
	//			cached_collider->obb.basis = spacial->transform.GetBasis();
	//			break;
	//		case ColliderType::CAPSULE:
	//			break;
	//		case ColliderType::MESH:
	//
	//			Mat4 transform = spacial->transform.CalculateTransformMatrix();// @TODO: Should be global ?
	//
	//			*cached_collider = *local_collider;
	//
	//			for (int32 i = 0; i < cached_collider->mesh.size(); i++)
	//			{
	//				Triangle *triangle = &cached_collider->mesh.at(i);
	//
	//				triangle->v0 = Vec3f(Vec4f(triangle->v0, 1.0f) * transform);
	//				triangle->v1 = Vec3f(Vec4f(triangle->v1, 1.0f) * transform);
	//				triangle->v2 = Vec3f(Vec4f(triangle->v2, 1.0f) * transform);
	//			}
	//
	//			break;
	//		}
	//	}
	//
	//	void WorldRenderer::Render(World *world)
	//	{
	//		EntityReference camera = world->GetActiveCamera();
	//		Mat4f view = camera.CameraCalculateViewMatrix();
	//		Mat4f proj = camera.CameraCalculateProjectionMatrix();
	//
	//		entities = world->CreateCollection([](EntityReference ref) {
	//			return ref.HasRendererable();
	//		});
	//	}
	//
	//	WorldRenderer::WorldRenderer()
	//	{
	//
	//	}
	//
	//	WorldRenderer::~WorldRenderer()
	//	{
	//
	//	}

	AssetTable *GameState::GetAssetTable()
	{
		ASSERT(instance, "Game state doesn't exist");

		return instance->asset_table;
	}

	cm::World* GameState::GetActiveWorld()
	{
		ASSERT(instance, "Game state doesn't exist");

		return instance->active_world;
	}


	void GameState::SetActiveWorld(World *world)
	{
		ASSERT(instance, "Game state doesn't exist");

		instance->active_world = world;
	}

	GameState::GameState()
	{
		ASSERT(instance == nullptr, "Game state already exists");
		instance = this;
	}

	static void DoToXMLTree(XMLElement *el, std::vector<Entity*> collection)
	{
		for (Entity *ref : collection)
		{
			String name = ref->GetName();
			std::replace(name.begin(), name.end(), ' ', '_');

			XMLElement *xml_child = el->AddChild("Entity");
			xml_child->AddAttribute("Type", ToString(ref->GetType()));
			xml_child->AddAttribute("Name", name);

			// @NOTE: Transform
			{
				XMLElement *comp = xml_child->AddChild("Transform");
				XMLElement *pos = comp->AddChild("Position");
				XMLElement *ort = comp->AddChild("Orientation");
				XMLElement *scl = comp->AddChild("Scale");

				Transform t = ref->GetLocalTransform();
				pos->data = ToString(t.position);
				ort->data = ToString(t.orientation);
				scl->data = ToString(t.scale);
			}

			// @NOTE: Rendering
			{
				XMLElement *comp = xml_child->AddChild("Renderering");
				MeshInstance instance = ref->GetMesh();
				XMLElement *mesh = comp->AddChild("MeshEntry");
				mesh->data = instance.meta_data.name;
			}

			// @NOTE: Bounding box
			{
				XMLElement *comp = xml_child->AddChild("BoundingBox");
				AABB box = ref->GetLocalBoundingBox();
				comp->AddChild("Min", ToString(box.min));
				comp->AddChild("Max", ToString(box.max));
			}

			// @NOTE: Collider
			{
				XMLElement *comp = xml_child->AddChild("Collider");

				if (ref->HasCollider())
				{
					Collider collider = ref->GetCollider();
					comp->AddAttribute("Type", ToString(collider.type));

					switch (collider.type)
					{
					case ColliderType::PLANE:
						comp->AddChild("Center", ToString(collider.plane.center));
						comp->AddChild("Normal", ToString(collider.plane.normal));
						break;
					case ColliderType::SPHERE:
						comp->AddChild("CenterRad", ToString(collider.sphere.data));
						break;
					case ColliderType::AABB:
						comp->AddChild("Min", ToString(collider.aabb.min));
						comp->AddChild("Max", ToString(collider.aabb.max));
						break;
					case ColliderType::OBB:
						comp->AddChild("Center", ToString(collider.obb.center));
						comp->AddChild("Extents", ToString(collider.obb.extents));
						comp->AddChild("Orientation", ToString(Mat3ToQuat(collider.obb.basis.mat)));
						break;
					case ColliderType::CAPSULE:
						comp->AddChild("Bot", ToString(collider.capsule.bot));
						comp->AddChild("Top", ToString(collider.capsule.top));
						comp->AddChild("Radius", std::to_string(collider.capsule.radius));
						break;
					case ColliderType::MESH:
						break;
					}
				}
			}

			switch (ref->GetType())
			{
			case EntityType::ENVIRONMENT:
			{
				break;
			}
			case EntityType::CAMERA:
			{
				Camera *cam = reinterpret_cast<Camera*>(ref);
				XMLElement *comp = xml_child->AddChild("CameraAttributes");

				XMLElement *c1 = comp->AddChild("CameraType", ToString(cam->camera_type));
				XMLElement *c2 = comp->AddChild("Dfov", std::to_string(cam->dfovy));
				XMLElement *c3 = comp->AddChild("Aspect", std::to_string(cam->aspect));
				XMLElement *c4 = comp->AddChild("ZFar", std::to_string(cam->zfar));
				XMLElement *c5 = comp->AddChild("ZNear", std::to_string(cam->znear));

				break;
			}
			case EntityType::PLAYER:
				break;
			case EntityType::ENEMY_DUDE:
				break;
			case EntityType::TURRET:
				break;
			default:
				LOG("CreateFromXMLTree: UNKONEN ENTIY LOADING");
				break;
			}

			//{
			//	XMLElement *comp = xml_child->AddChild("Camera");
			//	comp->AddAttribute("CoreComponent", "true");

			//	Camera cam = ref.CameraGetCamera();


			//}

			if (ref->GetFirstChild()->IsValid())
			{
				DoToXMLTree(xml_child, ref->GetChildren());
			}

		}
	}

	cm::XMLTree World::ToXMLTree()
	{
		XMLTree xml_tree("World");
		XMLElement *root = xml_tree.GetRoot();

		std::vector<Entity*> xml_entities = CreateCollection([](Entity *entity) {
			return !entity->GetParent()->IsValid();
		});

		DoToXMLTree(root, xml_entities);

		return xml_tree;
	}

	void World::CreateFromXMLTree(XMLTree *tree)
	{
		Reset();

		XMLElement* root = tree->GetRoot();

		std::list<XMLElement> entities = root->GetChildren();

		for (XMLElement &entity : entities)
		{
			EntityType type;
			String name;
			std::vector<XMLAttribute> attributes = entity.GetAttributes();
			for (XMLAttribute &attribute : attributes)
			{
				if (attribute.name == "Type")
				{
					type = StringToEntityType(attribute.data);
				}
				else if (attribute.name == "Name")
				{
					name = attribute.data;
				}
			}

			std::list<XMLElement> properties = entity.GetChildren();

			switch (type)
			{
			case EntityType::ENVIRONMENT:
			{
				Environment *entity = CreateEntity<Environment>();

				entity->SetName(name);
				entity->SetType(type);

				for (XMLElement &property : properties)
				{
					if (property.tag_name == "Transform")
					{
						std::list<XMLElement> transform_data = property.GetChildren();
						XMLElement pos = transform_data.front();
						transform_data.pop_front();

						entity->SetPosition(StringToVec3<real32>(pos.data));

						XMLElement ori = transform_data.front();
						transform_data.pop_front();

						entity->SetOrientation(StringToQuat<real32>(ori.data));

						XMLElement scl = transform_data.front();
						transform_data.pop_front();

						entity->SetScale(StringToVec3<real32>(scl.data));
					}
					else if (property.tag_name == "Renderering")
					{
						std::list<XMLElement> data = property.GetChildren();
						if (data.size() > 0)
						{
							XMLElement mesh = data.front();
							data.pop_front();
							entity->SetMesh(GameState::GetAssetTable()->FindMeshInstance(mesh.data));
						}
					}
					else if (property.tag_name == "BoundingBox")
					{
						std::list<XMLElement> data = property.GetChildren();
						XMLElement smin = data.front();
						data.pop_front();

						XMLElement smax = data.front();
						data.pop_front();

						Vec3f min = StringToVec3<real32>(smin.data);
						Vec3f max = StringToVec3<real32>(smax.data);

						entity->SetBoundingBox(CreateAABBFromMinMax(min, max));
					}
					else if (property.tag_name == "Collider")
					{
						std::vector<XMLAttribute> attribs = property.GetAttributes();

						if (attribs.size() > 0)
						{
							String stype = attribs.at(0).data;
							ColliderType type = StringToColliderType(stype);

							std::list<XMLElement> data = property.GetChildren();
							switch (type)
							{
							case ColliderType::PLANE:
								ASSERT(0, "TODO");
								break;
							case ColliderType::SPHERE:ASSERT(0, "TODO");
								break;
							case ColliderType::AABB:ASSERT(0, "TODO");
								break;
							case ColliderType::OBB:ASSERT(0, "TODO");
								break;
							case ColliderType::CAPSULE:
							{
								XMLElement sbot = data.front();
								data.pop_front();

								XMLElement stop = data.front();
								data.pop_front();

								XMLElement srad = data.front();
								data.pop_front();

								Vec3f bot = StringToVec3<real32>(sbot.data);
								Vec3f top = StringToVec3<real32>(stop.data);

								real32 rad = std::stof(srad.data);

								entity->SetCollider(Collider(CreateCapsuleFromBotTop(bot, top, rad)));
							}
							break;
							case ColliderType::MESH:
								Collider collider = GameState::GetAssetTable()->GetEditableMesh(entity->GetMesh()).GetMeshCollider();
								entity->SetCollider(collider);
								break;
							};
						}
					}
				}

			}break;
			case EntityType::CAMERA:
				break;
			case EntityType::PLAYER:
				break;
			case EntityType::ENEMY_DUDE:
				break;
			case EntityType::TURRET:
				break;
			default:
				LOG("CreateFromXMLTree: UNKONEN ENTIY LOADING");
				break;
			}

		}
	}

	void World::Reset()
	{
		while (freelist.size() > 0)
		{
			freelist.pop();
		}

		for (int32 i = 0; i < entities.size(); i++)
		{
			entities.at(i) = (std::make_unique<Entity>());
			entities.at(i)->world = this;
			freelist.push(i);
		}

		freelist.pop();		// @NOTE: Remove null entity, DOESNOT WORK UNLESS NULL_ENTITY_INDEX is 0!!!
		entity_count = 0;	// @NOTE: We don't count the null entity towards the count
		id_counter = NULL_ENTITY_ID + 1;

		entities.at(NULL_ENTITY_INDEX)->name = "NULL ENTITY";
		entities.at(NULL_ENTITY_INDEX)->index = NULL_ENTITY_INDEX;
		entities.at(NULL_ENTITY_INDEX)->id = NULL_ENTITY_ID;
	}

	void World::UpdatePhysicsEntities(const real32 &dt, PhysicsSolver *solver)
	{
		std::vector<Entity *> valid_entities = CreateCollection([](Entity *ent) {
			return ent->behaviour == EntityBehaviour::PHYSICS_ENTITY;
		});

		for (Entity *entity : valid_entities)
		{
			PhysicsEntity *phs = reinterpret_cast<PhysicsEntity*>(entity);
			phs->Integrate(dt);
		}

		solver->Solve();
	}

	void World::UpdateEntities(const real32 &dt)
	{
		std::vector<Entity *> valid_entities = CreateCollection();

		for (Entity *entity : valid_entities)
		{
			entity->Update(dt);
		}
	}

	std::vector<Entity *> World::CreateCollection(bool(*comparator)(Entity *entity) /*= nullptr*/)
	{
		std::vector<Entity *> collection; // @TODO: Can we make an educated guess on the size that this is going to be that we reserve from the start
		int32 entities_chommped = 0;

		for (int32 i = 1; i < entities.size(); i++)
		{
			Entity *entity = entities.at(i).get();

			if (entity->IsValid())
			{
				entities_chommped++;

				if (comparator)
				{
					if (comparator(entity))
					{
						collection.push_back(entity);
					}
				}
				else
				{
					collection.push_back(entity);
				}
			}

			// @NOTE: Early out
			if (entities_chommped >= entity_count)
			{
				break;
			}
		}

		return collection;
	}

	World::World()
	{
		for (int32 i = 0; i < entities.size(); i++)
		{
			entities.at(i) = (std::make_unique<Entity>());
			entities.at(i)->world = this;
			freelist.push(i);
		}

		freelist.pop();		// @NOTE: Remove null entity, DOESNOT WORK UNLESS NULL_ENTITY_INDEX is 0!!!
		entity_count = 0;	// @NOTE: We don't count the null entity towards the count
		id_counter = NULL_ENTITY_ID + 1;

		entities.at(NULL_ENTITY_INDEX)->name = "NULL ENTITY";
		entities.at(NULL_ENTITY_INDEX)->index = NULL_ENTITY_INDEX;
		entities.at(NULL_ENTITY_INDEX)->id = NULL_ENTITY_ID;
	}

	World::~World()
	{

	}

}

