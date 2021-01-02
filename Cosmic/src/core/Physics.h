#pragma once

#include "src/core/entities/Entity.h"

namespace cm
{
	INTROSPECTED_ENUM(PhysicsEntityType) {
		INVALID = 0,
			POINTMASS,
			RIGIDBODY,
			COUNT
	};

	class PointMass
	{

	};

	class RigidBody
	{

	};

	template<typename T>
	class PEnt : public Entity, public T
	{

	};


	class PhysicsEntity;

	class ForceGenerator
	{
	public:
		virtual void AddForce(const real32 &dt, PhysicsEntity *entity) = 0;
	};

	class DragForce : public ForceGenerator
	{
	public:
		virtual void AddForce(const real32 &dt, PhysicsEntity *entity) override;
	};

	INTROSPECTED_ENUM(ConnectionType)
	{
		INVALID = 0,
			SPRING,
			STIFF_SPRING,
			ROD,
			CABLE,
			ELASTIC,
			COUNT
	};

	INTROSPECTED_ENUM_FUNCTIONS(ConnectionType);

	INTROSPECTED_ENUM(ConnectionMode)
	{
		INVALID = 0,
			ENTITY_CONNECTED,
			ENTITY_ANCHORED,
			FIXED_ANCHORED,
			COUNT
	};

	INTROSPECTED_ENUM_FUNCTIONS(ConnectionMode);

	class Connection
	{
	public:
		ConnectionType type;
		ConnectionMode mode;
		union
		{
			real32 strength_coef;
			real32 rod_restitution;
			real32 cable_restitution;
		};
		real32 rest_length;

		// @NOTE: Assumes ref is physics entity
		EntityReference ref_self;
		EntityReference ref_other;

		Vec3f anchor_point;

		void ApplyForce(const real32 &dt);

	public:
		Connection();
		Connection(const ConnectionType &type, const real32 &strength_coef, const real32 &rest_length,
			EntityReference ref_self);
		~Connection();

	private:
		void ApplySpring();
		void ApplyStiffSpring();
		void ApplyElasitic();

		void ApplyRod();
		void ApplyCable();
	};

	class PhysicsEntity : public Entity
	{
	public:
		PhysicsEntityType physics_type;

		Vec3f veloctiy;
		Vec3f acceration;
		Vec3f rotation;

		real32 damping;

		Vec3f torque;
		Vec3f forces;

		Mat4f transform_matrix;
		Mat3f inverse_inertia_tensor;
		Mat3f world_inverse_inertia_tensor;

	public:

		inline real32 GetInverseMass() { return invmass; }
		inline real32 GetMass() { return (invmass == 0) ? 0 : 1.0f / invmass; }
		inline void SetMass(const real32 &mass) { invmass = (mass == 0) ? 0 : 1.0f / mass; }

		void Integrate(const real32 &dt);

		void AddImpulse(const Vec3f &force);
		void AddForce(const Vec3f &force);

		void AddForceAtPoint(const Vec3f &force, const Vec3f &point);
		void CalculateDerivedData();


	public:
		PhysicsEntity();
		~PhysicsEntity();

	private:
		real32 invmass;

		friend class PhysicsSolver;
		friend class CollisionDetector;
	};

	class ForceRegistry
	{
	public:
		std::vector<std::unique_ptr<ForceGenerator>> force_generators;
		std::vector<Connection> connected_springs;

		void ApplyForces(const real32 dt);

		ForceRegistry();
		~ForceRegistry();

	public:
		inline static ForceRegistry* instance;
	};

	class CollisionRegistry
	{
	public:
		class Collision
		{
		public:
			String name; // @TODO: Debugign ONLY!!

			real32 restitution;
			IntersectionInfo info;
			PhysicsEntity *entity_a;
			PhysicsEntity *entity_b;
			Vec3f move_a;
			Vec3f move_b;

		public:
			void Resolve(const real32 &dt);
			real32 CalculateSeparatingSpeed();
			real32 CalculateSeparatingAccel();

		private:
			void ResolveVelocity(const real32 &dt);
			void ResolveInterpenetration(const real32 &dt);
		};

		std::vector<Collision>collisions;


	public:
		void Resolve(const real32 &dt);

	public:
		CollisionRegistry();
		~CollisionRegistry();

	public:
		inline static CollisionRegistry* instance;
	};

	class CollisionDetector
	{
	public:
		void FindCollisions(World *world, CollisionRegistry *collision_registry, const real32 &dt);

	public:
		CollisionDetector();
		~CollisionDetector();
	};

	class PhysicsSolver
	{
	public:
		void Solve() {		}
	};
}

