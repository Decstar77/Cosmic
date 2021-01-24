#pragma once
#include "entities/Entity.h"
#include "src/serialization/Assets.h"
#include "Physics.h"
#include "Player.h"
#include "entities/Environment.h"

namespace cm
{

	// @NOTE: Base, Spacial, Renderable, Camera, Collider, RigidBody, Sound, Particels
//
//	struct BaseEntity
//	{
//		String name;
//		EntityType type;
//
//		int32 id;
//		int32 index;
//		int32 parent;
//		int32 sibling;
//		int32 child;
//
//		bool has_spacial;
//		bool has_camera;
//		bool has_collider;
//		bool has_rigidbody;
//		bool has_rendererable;
//		bool has_turret;
//		bool has_custom;
//
//		inline bool InUse() { return id; }
//	};
//
//	extern inline String ToString(const EntityType &camera);
//	extern inline EntityType StringToEntityType(const String &entry);
//
//	struct Spacial
//	{
//		//AABB cached_bounding_box;	// @NOTE: The world space bounding box that is cached when GetBoundingBox is called
//		bool has_moved_this_frame;
//		AABB bounding_box;			// @NOTE: The bounding box in objects local space with no tranlsation, rotation or scale
//		Transform transform;
//	};
//
//	struct Renderable
//	{
//		MeshEntry mesh_entry;
//		MaterialInstance material;
//	};
//
//	struct RigidBody
//	{
//		Sphere collider;
//		real32 mass;
//		Vec3f velocity;
//		Vec3f acceleration;
//		Vec3f forces;
//	};
//

	//
	//	extern inline String ToString(const CameraType &camera);
	//	extern inline CameraType StringToCameraType(const String &entry);
	//
	//	struct Camera
	//	{
	//		CameraType type;
	//		real32 dfovy;
	//		real32 aspect;
	//		real32 znear;
	//		real32 zfar;
	//	};
	//
	//
	//#define CustomEntityClass(class_name) class class_name
	//#define EntityFunctions(class_name) static void Start(World *world, class_name *me); \
	//									static void Update(World *world, class_name *me); \
	//									static void End(World *world, class_name *me); \
	//
	//	class Turret
	//	{
	//		real32 ammo;
	//
	//		EntityFunctions(Turret);
	//	};
	//
	//
	//#define INVALID_ENTITY_INDEX 0
	//#define CHECK_ENTITY() ASSERT(IsValid(), __FUNCTION_NAME__ " was called on an invalid entity")
	//
	//	typedef std::vector<EntityReference> EntityCollection;
	//
	//	class EntityReference // @TODO: const functions, then change DoToXMLTree to take const &collection
	//	{
	//	public:
	//		//************************************
	//		// Standard functions
	//		//************************************
	//
	//		bool IsValid();
	//
	//		bool HasParent();	// @NOTE: Assumes entity is valid
	//		bool HasChild();	// @NOTE: Assumes entity is valid
	//		bool HasSibling();	// @NOTE: Assumes entity is valid
	//
	//		bool HasSpacial();
	//		bool HasCamera();		// @NOTE: If this is true it is guaranteed to have a spacial
	//		bool HasCollider();		// @NOTE: If this is true it is guaranteed to have a spacial. @TODO: get rid of this for trigger areas
	//		bool HasRendererable(); // @NOTE: If this is true it is guaranteed to have a spacial
	//		bool HasRigidBody();	// @NOTE: If this is true it is guaranteed to have a spacial
	//
	//		int32 GetId();
	//		String GetName();
	//		EntityType GetType();
	//
	//		EntityReference Copy();
	//
	//		void SetParent(EntityReference *entity);
	//		void AddChild(EntityReference *entity);
	//
	//		EntityReference GetParent();		// @NOTE: Assumes the entity does have a parent, check with HasParent
	//		EntityReference GetFirstChild();	// @NOTE: Assumes the entity does have a child, check with HasChild
	//		EntityReference GetLastChild();		// @NOTE: Assumes the entity does have a child, check with HasChild
	//		EntityReference GetSibling();		// @NOTE: Assumes the entity does have a sibling, check with HasSibling
	//
	//		EntityCollection GetChildren();
	//
	//		inline constexpr bool operator==(const EntityReference &other) { return this->id == other.id; }
	//		inline constexpr bool operator!=(const EntityReference &other) { return this->id != other.id; }
	//
	//		//************************************
	//		// Spacial functions
	//		//************************************
	//
	//		Transform SpacialGetLocalTransform();
	//		Transform SpacialGetGlobalTransform();
	//		Mat4f SpacialGetLocalTransformMatrix();
	//		Mat4f SpacialGetGlobalTransformMatrix();
	//
	//		void SpacialSetTransform(const Transform &transform);
	//
	//		Vec3f SpacialGetPosition();
	//
	//		AABB SpacialGetGlobalBoundingBox();
	//		AABB SpacialGetLocalBoundingBox();
	//
	//		void SpacialAddPosition(const Vec3f &position);
	//		void SpacialSetPosition(const Vec3f &position);
	//
	//		void SpacialSetOrientation(const Quatf &orientation);
	//		void SpacialSetOrientation(const Vec3f &euler);
	//		void SpacialSetScale(const Vec3f &scale);
	//
	//		void SpacialLookAt(const Vec3f &position);
	//
	//		//************************************
	//		// Renderable functions
	//		//************************************
	//
	//		void SetMesh(const MeshEntry &entry);
	//		MeshEntry GetMesh();
	//
	//		//************************************
	//		// Camera functions
	//		//************************************
	//
	//		Camera CameraGetCamera();
	//		Mat4f CameraCalculateProjectionMatrix();
	//		Mat4f CameraCalculateViewMatrix();
	//		void CameraSetActiveCamera();
	//		void CameraSetAsMainCamera();
	//		void CameraSetFarPlane(const real32 &zfar);
	//		Ray CameraShootRay(const Vec2f &mouse_position, const Vec2f &window_dimenions);
	//
	//		//************************************
	//		// Rigidbody functions
	//		//************************************
	//
	//		Collider GetCollider();
	//		void MoveAndCollide(const Vec3f &delta); // @TODO: return velo/hit somtign 
	//		void SetMass(real32 mass);
	//
	//		//************************************
	//		// Turret functions
	//		//************************************
	//
	//		void SetAmmo(real32 ammo);
	//
	//	private:
	//		int32 index;
	//		int32 id;
	//		World *world;
	//
	//	private:
	//		void ComputeCollider();
	//
	//	private:
	//		friend class World;
	//	};
	//
	//	class Player
	//	{
	//	public:
	//		EntityReference spacial;
	//		EntityReference camera;
	//		EntityReference weapon;
	//
	//		Ellipsoid collider;
	//		EntityReference wall;
	//		//Capsule collider;
	//
	//		real32 character_length;
	//		real32 character_eye_height;
	//
	//		real32 speed;
	//		real32 walk_speed;
	//		real32 run_speed;
	//		real32 max_speed;
	//
	//
	//		bool grounded;
	//		real32 jump_power;
	//		real32 gravity_amount;
	//
	//		real32 walk_accel;
	//		real32 look_speed;
	//
	//		real32 pitch;
	//		real32 yaw;
	//
	//		Vec3f velocity;
	//		Vec3f acceleration;
	//
	//	public:
	//		void Update();
	//
	//		void Init(World *world);
	//
	//		Player();
	//		~Player();
	//
	//		void PlayerLook();
	//		void PlayerMove();
	//		Vec3f SweepForward(const Vec3f &pos, const Vec3f &velo, real32 t);
	//		Vec2f QuadracticSolve(const real32 &a, const real32 &b, const real32 &c);
	//
	//
	//	private:
	//		World *world;
	//	};
	//
	//
	//	struct EnemyDude
	//	{
	//		real32 health;
	//		real32 ammo;
	//		bool dead;
	//
	//		static void Update(World *world, EnemyDude *me, EntityReference ref)
	//		{
	//
	//		}
	//	};
	//
	//	struct EntityCustomData
	//	{
	//		union
	//		{
	//			EnemyDude enemy_dude;
	//		};
	//
	//		void Start(World *world, EntityReference ref) {}
	//		void Update(World *world, EntityReference ref)
	//		{
	//			EntityType type = ref.GetType();
	//			switch (type)
	//			{
	//			case EntityType::INVALID:
	//				break;
	//			case EntityType::ENVIRONMENT:
	//				break;
	//			case EntityType::TURRET:
	//				break;
	//			case EntityType::ENEMY_DUDE:
	//			{
	//				EnemyDude::Update(world, &enemy_dude, ref);
	//			}
	//			break;
	//			default:
	//				break;
	//			}
	//		}
	//
	//		void End(World *world, EntityReference ref);
	//	};
	//
	//

	//
	//
	//#define MAX_ENTITY_COUNT 1000
	//	//typename std::vector<EntityReference> EntityCollection;
	//	class World
	//	{
	//	public:
	//		std::array<BaseEntity, MAX_ENTITY_COUNT> entities;
	//		std::array<Spacial, MAX_ENTITY_COUNT> spacials;
	//		std::array<Renderable, MAX_ENTITY_COUNT> renderables;
	//
	//		// @TODO: Should this be a collider list or just a single collider with parents ?. I think these are always in local space. 
	//		// : That way position updates never need to make sure to update colliders. It's just done when retriving them. And we could cache this
	//		std::array<Collider, MAX_ENTITY_COUNT> colliders;
	//		std::array<Collider, MAX_ENTITY_COUNT> cached_colliders;
	//		std::array<Collider, MAX_ENTITY_COUNT> static_colliders; // @TODO: Statics ?!
	//
	//		std::array<RigidBody, MAX_ENTITY_COUNT> rigidbodies;
	//		std::array<Camera, MAX_ENTITY_COUNT> cameras;
	//		std::array<Turret, MAX_ENTITY_COUNT> turrets;
	//		std::array<EntityCustomData, MAX_ENTITY_COUNT> customs;
	//		std::array<int32, MAX_ENTITY_COUNT> freelist;
	//
	//		Player player;
	//		real32 delta_time;
	//
	//	public:
	//		std::vector<EntityReference> CreateCollection(bool(*comparator)(EntityReference) = nullptr);
	//
	//		BaseEntity* GetBaseEntity(EntityReference entity);		  // @NOTE: Priv
	//		EntityReference CreateEntityReference(BaseEntity *entity);// @NOTE: Priv
	//
	//		EntityReference AddEntity(const String &name);
	//		EntityReference AddSpacial(const String &name, const Transform &transform);
	//		EntityReference AddCollider(const String &name, const Transform &transform, const Collider &collider);
	//		EntityReference AddCamera(const String &name, const Transform &transform, const CameraType &type);
	//		EntityReference AddRenderable(const String &name, const Transform &transform, const MeshEntry &mesh_entry);
	//		EntityReference AddRigidbody(const String &name, const Transform &transform, const MeshEntry &mesh_entry);
	//		EntityReference AddEnvironmentEntity(const String &name, const Transform &transform, const MeshEntry &mesh_entry);
	//		EntityReference AddTurrentEntity(const String &name, const Transform &transform);
	//
	//		Spacial* GetEntitySpacial(const EntityReference &entity);
	//		void RemoveEntity(const EntityReference &entity);
	//
	//		void SetActiveCamera(EntityReference *entity);
	//		void SetMainCamera(EntityReference *entity);
	//		EntityReference GetActiveCamera();
	//		EntityReference GetMainCamera();
	//
	//		XMLTree ToXMLTree();
	//
	//		void UpdateEntities();
	//		void PostUpdate();
	//		void StepPhysics();
	//
	//	public:
	//		World();
	//		~World();
	//
	//	private:
	//		int32 free_count;
	//		int32 entity_next_id;
	//		int32 spacial_next_id;
	//
	//		int32 active_camera_index;
	//		int32 main_camera_index;
	//
	//		int32 entity_count;
	//
	//
	//	private:
	//		int32 GetNextFree();
	//
	//		void UpdateEntity(int32 index);
	//
	//
	//		friend class EntityReference;
	//		//friend class WorldRenderer;
	//	};
	//
	//	class Renderer
	//	{
	//		//void CheckMeshes();
	//		virtual void RenderMesh(MeshInstance mesh) = 0;
	//	};
	//
	//	class WorldRenderer
	//	{
	//	public:
	//		void Render(World *world);
	//		std::vector<EntityReference> entities;
	//
	//	public:
	//		WorldRenderer();
	//		~WorldRenderer();
	//
	//	private:
	//		// Renderer
	//		//World *world; //@TODO: We should do a flat copy of a world for mutli threading purposes
	//	};
	//

	//template<typename T>
	//class MemoryAreana
	//{
	//public:
	//	int32 count;
	//	std::vector<T> data;
	//	std::queue<uint32> freelist;

	//public:
	//	uint32 Add(const T &t)
	//	{
	//		if (freelist.size() > 0)
	//		{
	//			uint32 index = freelist.front();
	//			freelist.pop();
	//			data.at(index) = t;
	//		}
	//		else
	//		{
	//			data.emplace_back(t);
	//		}

	//		count++;
	//	}

	//	void Remove(const uint32 &index)
	//	{
	//		freelist.push(index);
	//	}

	//public:

	//	MemoryAreana()
	//		: count(0)
	//	{
	//	}

	//	MemoryAreana(const uint32 &size)
	//		: count(0)
	//	{
	//		data.reserve(size);
	//	}

	//	~MemoryAreana() {}
	//};

	class World
	{
	public:
		int32 entity_count;
		int32 id_counter;
		std::array<std::unique_ptr<Entity>, 1000> entities;
		std::queue<int32> freelist;



	public:
		template<typename T, typename... Args>
		T * CreateEntity(const Args &... args)
		{
			if (freelist.size() != 0)
			{
				int32 index = freelist.front();
				freelist.pop();

				entities.at(index) = std::make_unique<T>(args...);
				entities.at(index)->index = index;
				entities.at(index)->id = id_counter;
				entities.at(index)->world = this;

				T *t = reinterpret_cast<T*>(entities.at(index).get());

				entity_count++;
				id_counter++;

				t->Start();

				return t;
			}
			else
			{
				ASSERT(0, "CreateEntity, not enough space");
				return nullptr;
			}
		}

		XMLTree ToXMLTree();
		void CreateFromXMLTree(XMLTree *tree);
		void Reset();

		void UpdatePhysicsEntities(const real32 &dt, PhysicsSolver *solver);
		void UpdateEntities(const real32 &dt);

		std::vector<Entity *> CreateCollection(bool(*comparator)(Entity *entity) = nullptr);

	public:
		World();
		~World();
	};

	class GameState;

	class GameState
	{
	public:
		AssetTable *asset_table; // @TODO: Unique ptr

	public:

		static World* GetActiveWorld();
		static void SetActiveWorld(World *world);
		static AssetTable *GetAssetTable();

	public:
		GameState();
		~GameState() {};

	private:
		World *active_world; // @TODO: Unique ptr

		inline static GameState *instance = nullptr;
	};
}
