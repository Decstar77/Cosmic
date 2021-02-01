#pragma once
#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"
#include "src/math/CosmicGeometry.h"

#include "src/rendering/Renderer.h"
#include "src/serialization/Assets.h"

namespace cm
{
	class World;
	class Entity;

	INTROSPECTED_ENUM(EntityBehaviour) {
		INVALID = 0,
			BASE_ENTITY,
			TRANSIENT_ENTITY,
			UPDATABLE_ENTITY,
			PHYSICS_ENTITY,
			COUNT
	};

	INTROSPECTED_ENUM_FUNCTIONS(EntityBehaviour);

	INTROSPECTED_ENUM(EntityType) {
		INVALID = 0,
			PARTICEL_EMITTER,
			ENVIRONMENT,
			CAMERA,
			POINTLIGHT,
			SPOTLIGHT,
			DIRECTIONALLIGHT,
			PLAYER,
			ENEMY_DUDE,
			TURRET,
			DRONE,
			COUNT
	};

	INTROSPECTED_ENUM_FUNCTIONS(EntityType);

#define NULL_ENTITY_INDEX 0
#define NULL_ENTITY_ID 0

	enum class EntityFlags : uint32 // @NOTE: Do we really care about this small small optimziation
	{
		ACTIVE = SET_BIT(1),
		HAS_COLLIDER = SET_BIT(2),
	};

	class EntityReference
	{
	public:
		Entity *Get();
		bool IsValid();

	public:
		EntityReference();
		EntityReference(const int32 index, const int32 id, World *world);
		~EntityReference();

		inline bool operator ==(const EntityReference &ref)
		{
			return this->id == ref.id;
		}

	private:
		int32 index;
		int32 id;
		World *world;
	};

	class Entity
	{
	public:
		bool active;
		bool should_draw;
		bool simulate_physics;

		EntityType type;
		EntityBehaviour behaviour;

		String name;

		Transform transform;

	public:
		inline int32 GetId() { return id; }
		inline EntityReference CreateEntityReference() { return EntityReference(index, id, world); }

		inline void SetName(const String &name) { this->name = name; }
		inline String GetName() { return name; }

		inline void SetType(const EntityType &type) { this->type = type; }
		inline EntityType GetType() { return type; }

		bool IsValid();

		void SetParent(Entity *entity);
		void SetChild(Entity *entity);

		Entity *GetParent() const;
		Entity *GetFirstChild() const;
		Entity *GetLastChild() const; // @TODO:
		Entity *GetSibling() const;
		std::vector<Entity*> GetChildren() const;

		inline void SetPosition(const Vec3f &position) { this->transform.position = position; }
		inline Vec3f GetPosition() const { return transform.position; }
		Vec3f GetGlobalPosition() const;

		inline void SetOrientation(const Quatf &orientation) { this->transform.orientation = orientation; }
		inline Quatf GetOrientation() const { return this->transform.orientation; }
		Quatf GetGlobalOrientation() const; // @TODO:

		inline void SetScale(const Vec3f &scale) { this->transform.scale = scale; }
		inline Vec3f GetScale() const { return this->transform.scale; }
		Vec3f GetGlobalScale() const; // @TODO:

		inline void LookAt(const Vec3f &position) { this->transform.LookAtLH(position); }

		inline void SetTransform(const Transform &transform) { this->transform = transform; }

		inline Transform GetLocalTransform() const { return transform; }
		Transform GetGlobalTransform() const;

		Mat4f GetLocalTransformMatrix() const;
		Mat4f GetGlobalTransformMatrix() const;

		inline void SetBoundingBox(const AABB &box) { this->local_bounding_box = box; }

		inline AABB GetLocalBoundingBox() const { return local_bounding_box; }
		AABB GetGlobalBoundingBox() const;

		// @NOTE: Compontent ?
		inline bool HasCollider() { return local_collider.type != ColliderType::INVALID; }
		inline void SetCollider(const Collider &collider) { this->local_collider = collider; }
		inline Collider GetCollider() const { return local_collider; }
		Collider GetGlobalCollider() const;

		inline bool HasMesh() { return mesh.IsValid(); }
		inline MeshInstance GetMesh() const { return mesh; }
		void SetMesh(const MeshInstance &mesh);

		virtual void Start() {};
		virtual void Update(const real32 &dt) {};
		virtual void Finish(const real32 &dt) {};

	public:
		Entity();
		Entity(const EntityType &type, const String &name = "UNTITLED ENTITY");
		Entity(const EntityType &type, const Transform &transform, const String &name = "UNTITLED ENTITY");
		virtual ~Entity();

	protected:
		World *world;

		Collider local_collider;
		Collider cached_world_collider;

		AABB local_bounding_box;

	private:
		MeshInstance mesh;

		int32 id;
		int32 index;

		int32 parent;
		int32 sibling;
		int32 child;

		friend class EntityReference;
		friend class World;
	};
}
