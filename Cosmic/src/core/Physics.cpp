
#include "Physics.h"
#include "World.h"
#include "src/Debug.h"
namespace cm
{
	void PhysicsEntity::Integrate(const real32 &dt)
	{
		if (invmass <= 0) return;

		if (physics_type == PhysicsEntityType::POINTMASS)
		{
			Vec3f accel = acceration + forces * invmass;

			veloctiy += accel * dt;

			veloctiy = veloctiy * Pow(damping, dt);

			transform.position += veloctiy * dt;

			forces = Vec3f(0);
		}
		else if (physics_type == PhysicsEntityType::RIGIDBODY)
		{
			Vec3f accel = acceration + forces * invmass;
			Vec3f angular_accel = torque * world_inverse_inertia_tensor;

			veloctiy = veloctiy + accel * dt;
			rotation = rotation + angular_accel * dt;

			// Impose drag.
			veloctiy = veloctiy * Pow(damping, dt);
			rotation = rotation * Pow(damping, dt);

			// Adjust positions
			// Update linear position.
			transform.position += veloctiy * dt;

			// Update angular position.
			Quatf q = Quatf(rotation.x * dt, rotation.y * dt, rotation.z * dt, 1.0f);
			transform.orientation = Normalize(transform.orientation * q);

			//q = q * transform.orientation;
			//transform.orientation.w += q.w * ((real32)0.5);
			//transform.orientation.x += q.x * ((real32)0.5);
			//transform.orientation.y += q.y * ((real32)0.5);
			//transform.orientation.z += q.z * ((real32)0.5);

			CalculateDerivedData();
			forces = Vec3f(0);
			torque = Vec3f(0);
		}
	}

	void PhysicsEntity::AddImpulse(const Vec3f &force)
	{
		veloctiy = force * 0.016f * invmass + veloctiy;
	}

	void PhysicsEntity::AddForce(const Vec3f &force)
	{
		forces += force;
	}

	void PhysicsEntity::AddForceAtPoint(const Vec3f &force, const Vec3f &point)
	{
		Vec3f local_point = Vec3f(Vec4f(point, 1.0f) * Inverse(transform_matrix));

		forces += force;
		Vec3f t = Cross(local_point, force);
		torque += t;
	}

	void PhysicsEntity::CalculateDerivedData()
	{
		transform.orientation = Normalize(transform.orientation);
		transform_matrix = transform.CalculateTransformMatrix();

		real32 m = GetMass();

		Mat3f inertia_tensor;
		inertia_tensor.row0.x = (1.0f / 12.0f)* m *(1.0f + 1.0f);
		inertia_tensor.row1.y = (1.0f / 12.0f)* m *(1.0f + 1.0f);
		inertia_tensor.row2.z = (1.0f / 12.0f)* m *(1.0f + 1.0f);

		inverse_inertia_tensor = Inverse(inertia_tensor);

		Mat3f rot = Mat3f(transform_matrix);
		world_inverse_inertia_tensor = rot * inverse_inertia_tensor * Inverse(rot);
	}

	PhysicsEntity::PhysicsEntity()
	{
		type = EntityType::ENVIRONMENT;
		behaviour = EntityBehaviour::PHYSICS_ENTITY;
		physics_type = PhysicsEntityType::POINTMASS;
		this->simulate_physics = true;
		SetCollider(CreateSphere(Vec3f(0), 0.05f));

		//force_generators.push_back(std::make_unique<DragForce>(DragForce()));

	}

	PhysicsEntity::~PhysicsEntity()
	{

	}

	void DragForce::AddForce(const real32 &dt, PhysicsEntity *entity)
	{
		real32 k1 = 0.88f;
		real32 k2 = 0.12f;

		real32 m = Mag(entity->veloctiy);
		if (!Equal(m, 0.0f))
		{
			real32 d = -(k1 * m + k2 * m *m);
			Vec3f force = (entity->veloctiy / m) * d;
			entity->forces += force;
		}
	}

	void Connection::ApplyForce(const real32 &dt)
	{
		switch (type)
		{
		case cm::ConnectionType::SPRING:
			ApplySpring();
			break;
		case cm::ConnectionType::STIFF_SPRING:
			ApplyStiffSpring();
			break;
		case cm::ConnectionType::ROD:
			ApplyRod();
			break;
		case cm::ConnectionType::CABLE:
			ApplyCable();
			break;
		case cm::ConnectionType::ELASTIC:
			ApplyElasitic();
			break;
		}
	}

	Connection::Connection()
		: type(ConnectionType::INVALID), mode(ConnectionMode::INVALID)
	{

	}

	Connection::Connection(const ConnectionType &type, const real32 &strength_coef, const real32 &rest_length,
		EntityReference ref_self) : type(type), mode(ConnectionMode::INVALID),
		strength_coef(strength_coef), rest_length(rest_length), ref_self(ref_self)
	{

	}

	Connection::~Connection()
	{

	}

	void Connection::ApplySpring()
	{
		switch (mode)
		{
		case cm::ConnectionMode::ENTITY_CONNECTED:
		{
			PhysicsEntity *self = reinterpret_cast<PhysicsEntity*>(ref_self.Get());
			PhysicsEntity *other = reinterpret_cast<PhysicsEntity*>(ref_other.Get());

			if (self->IsValid() && other->IsValid())
			{
				Vec3f dif = other->GetPosition() - self->GetPosition();
				real32 mag = Mag(dif);
				if (!Equal(mag, 0.0f))
				{
					Vec3f dir = dif / mag;
					Vec3f force = strength_coef * (rest_length - mag) * dir;

					other->forces += force;
					self->forces -= force;
				}
			}
		} break;

		case cm::ConnectionMode::ENTITY_ANCHORED:
		{
			PhysicsEntity *self = reinterpret_cast<PhysicsEntity*>(ref_self.Get());
			PhysicsEntity *other = reinterpret_cast<PhysicsEntity*>(ref_other.Get());

			if (self->IsValid() && other->IsValid())
			{
				Vec3f dif = other->GetPosition() - self->GetPosition();
				real32 mag = Mag(dif);
				if (!Equal(mag, 0.0f))
				{
					Vec3f dir = dif / mag;
					Vec3f force = strength_coef * (rest_length - mag) * dir;

					self->forces -= force;
				}
			}

		} break;

		case cm::ConnectionMode::FIXED_ANCHORED:
		{
			PhysicsEntity *self = reinterpret_cast<PhysicsEntity*>(ref_self.Get());

			if (self->IsValid())
			{
				Vec3f dif = anchor_point - self->GetPosition();
				real32 mag = Mag(dif);
				if (!Equal(mag, 0.0f))
				{
					Vec3f dir = dif / mag;
					Vec3f force = strength_coef * (rest_length - mag) * dir;

					self->forces -= force;
				}
			}
		} break;

		default:
			break;
		}
	}

	void Connection::ApplyStiffSpring()
	{
		switch (mode)
		{
		case cm::ConnectionMode::ENTITY_CONNECTED:
		{

		} break;

		case cm::ConnectionMode::ENTITY_ANCHORED:
		{
			//PhysicsEntity *self = reinterpret_cast<PhysicsEntity*>(ref_self.Get());
			//PhysicsEntity *other = reinterpret_cast<PhysicsEntity*>(ref_other.Get());

			//if (self->IsValid() && other->IsValid() && self->GetMass() != 0)
			//{
			//	Vec3 m = other->GetPosition() - self->GetPosition();
			//	real32 damping = rest_length;
			//	real32 gamma = 0.5f * Sqrt(4.0f * strength_coef - damping * damping);

			//	if (Equal(gamma, 0.0f)) return;

			//	Vec3f c = m * (damping / (2.0f * gamma)) + self->veloctiy *(1.0f / gamma);

			//	real32 dt = 0.016f;

			//	Vec3f t = m * Cos(gamma * dt) + c * Sin(gamma * dt) * exp(-0.5f * dt * damping);

			//	Vec3f accel = (m - t) * (1.0f / (dt * dt)) - self->veloctiy * dt;
			//	self->forces += accel * self->GetMass();
			//}

		} break;

		case cm::ConnectionMode::FIXED_ANCHORED:
		{

		} break;

		default:
			break;
		}
	}

	void Connection::ApplyRod()
	{
		switch (mode)
		{
		case cm::ConnectionMode::ENTITY_CONNECTED:
		{
			PhysicsEntity *self = reinterpret_cast<PhysicsEntity*>(ref_self.Get());
			PhysicsEntity *other = reinterpret_cast<PhysicsEntity*>(ref_other.Get());

			if (self->IsValid() && other->IsValid() && self->GetMass() > 0)
			{
				real32 l2 = rest_length;
				real32 d2 = Distance(other->GetPosition(), self->GetPosition());
				if (!Equal(l2, d2))
				{
					CollisionRegistry::Collision col;
					col.restitution = rod_restitution;
					col.entity_a = self;
					col.entity_b = other;

					col.info.penetration = Abs(d2 - l2);
					col.info.normal = static_cast<real32>(Sign(d2 - l2)) * Normalize(other->GetPosition() - self->GetPosition());
					CollisionRegistry::instance->collisions.push_back(col);
				}
			}
		} break;

		case cm::ConnectionMode::ENTITY_ANCHORED:
		{

		} break;
		case cm::ConnectionMode::FIXED_ANCHORED:
		{

		} break;

		default:
			break;
		}
	}

	void Connection::ApplyCable()
	{
		switch (mode)
		{
		case cm::ConnectionMode::ENTITY_CONNECTED:
		{
			PhysicsEntity *self = reinterpret_cast<PhysicsEntity*>(ref_self.Get());
			PhysicsEntity *other = reinterpret_cast<PhysicsEntity*>(ref_other.Get());

			if (self->IsValid() && other->IsValid() && self->GetMass() > 0)
			{
				real32 l2 = rest_length;
				real32 d2 = Distance(other->GetPosition(), self->GetPosition());
				if (d2 > l2)
				{
					CollisionRegistry::Collision col;
					col.restitution = rod_restitution;
					col.entity_a = self;
					col.entity_b = other;

					col.info.penetration = d2 - l2;
					col.info.normal = Normalize(other->GetPosition() - self->GetPosition());

					CollisionRegistry::instance->collisions.push_back(col);
				}
			}
		} break;

		case cm::ConnectionMode::ENTITY_ANCHORED:
		{

		} break;
		case cm::ConnectionMode::FIXED_ANCHORED:
		{

		} break;

		default:
			break;
		}

	}

	void Connection::ApplyElasitic()
	{
		switch (mode)
		{
		case cm::ConnectionMode::ENTITY_CONNECTED:
		{
			PhysicsEntity *self = reinterpret_cast<PhysicsEntity*>(ref_self.Get());
			PhysicsEntity *other = reinterpret_cast<PhysicsEntity*>(ref_other.Get());

			if (self->IsValid() && other->IsValid())
			{
				Vec3f dif = other->GetPosition() - self->GetPosition();
				real32 mag = Mag(dif);

				if (mag <= rest_length) return;

				if (!Equal(mag, 0.0f))
				{
					Vec3f dir = dif / mag;
					Vec3f force = strength_coef * (rest_length - mag) * dir;

					other->forces += force;
					self->forces -= force;
				}
			}
		} break;

		case cm::ConnectionMode::ENTITY_ANCHORED:
		{
			PhysicsEntity *self = reinterpret_cast<PhysicsEntity*>(ref_self.Get());
			PhysicsEntity *other = reinterpret_cast<PhysicsEntity*>(ref_other.Get());

			if (self->IsValid() && other->IsValid())
			{
				Vec3f dif = other->GetPosition() - self->GetPosition();
				real32 mag = Mag(dif);

				if (mag <= rest_length) return;

				if (!Equal(mag, 0.0f))
				{
					Vec3f dir = dif / mag;
					Vec3f force = strength_coef * (rest_length - mag) * dir;

					self->forces -= force;
				}
			}

		} break;

		case cm::ConnectionMode::FIXED_ANCHORED:
		{
			PhysicsEntity *self = reinterpret_cast<PhysicsEntity*>(ref_self.Get());

			if (self->IsValid())
			{
				Vec3f dif = anchor_point - self->GetPosition();
				real32 mag = Mag(dif);

				if (mag <= rest_length) return;

				if (!Equal(mag, 0.0f))
				{
					Vec3f dir = dif / mag;
					Vec3f force = strength_coef * (rest_length - mag) * dir;

					self->forces -= force;
				}
			}
		} break;

		default:
			break;
		}
	}

	void ForceRegistry::ApplyForces(const real32 dt)
	{
		for (std::unique_ptr<ForceGenerator> &gen : force_generators)
		{

		}

		for (Connection &spring : connected_springs)
		{
			if (spring.type == ConnectionType::SPRING
				|| spring.type == ConnectionType::STIFF_SPRING
				|| spring.type == ConnectionType::ELASTIC)
			{
				spring.ApplyForce(dt);
			}
		}
	}

	ForceRegistry::ForceRegistry()
	{
		instance = this;
	}

	ForceRegistry::~ForceRegistry()
	{

	}

	void CollisionRegistry::Resolve(const real32 &dt)
	{
#if 0
		int32 itteration = 0;
		for (; itteration < collisions.size() * 2.0f; itteration++)
		{
			int32 max_index = -1;
			real32 max_speed = 99999.0f;
			for (int32 i = 0; i < collisions.size(); i++)
			{
				real32 speed = collisions.at(i).CalculateSeparatingSpeed();
				real32 pen = collisions.at(i).info.penetration;
				if (speed < max_speed &&  pen > 0)
				{
					max_index = i;
					max_speed = speed;
				}
			}

			if (max_index == -1) break;

			Collision &col = collisions.at(max_index);

			col.Resolve(dt);

			for (int32 i = 0; i < collisions.size(); i++)
			{
				Collision &oth = collisions.at(i);

				if (col.entity_a == oth.entity_a)
				{
					col.info.penetration -= Dot(col.move_a, col.info.normal);
				}
				else if (col.entity_a == oth.entity_b)
				{
					col.info.penetration -= Dot(col.move_b, col.info.normal);
				}

				if (col.entity_b == oth.entity_a)
				{
					col.info.penetration += Dot(col.move_a, col.info.normal);
				}
				else if (col.entity_b == oth.entity_b)
				{
					col.info.penetration += Dot(col.move_b, col.info.normal);
				}
			}

			//col.Resolve(dt);
			//collisions.pop();
		}

		//	LOG(itteration);
#else
		// @NOTE: Imp
		{
			for (Collision &col : collisions)
			{
				PhysicsEntity *entity_a = col.entity_a;
				PhysicsEntity *entity_b = col.entity_b;

				Vec3f aVel = entity_a->veloctiy;
				Vec3f bVel = entity_b ? entity_b->veloctiy : Vec3f(0.0f);
				Vec3f rVel = bVel - aVel;
				real32 nSpd = Dot(rVel, col.info.normal);

				if (nSpd >= 0) continue;

				real32 aimass = entity_a->GetInverseMass();
				real32 bimass = entity_b ? entity_b->GetInverseMass() : 0.0f;

				real32 e = col.restitution;
				real32 j = -(1.0f + e) * nSpd / (aimass + bimass);
				Vec3f impluse = j * col.info.normal;
				aVel -= impluse * aimass;
				bVel += impluse * bimass;

				entity_a->veloctiy = aVel;
				if (entity_b) entity_b->veloctiy = bVel;
			}
		}

		// @NOTE: Pos
		{
			std::vector<std::pair<Vec3f, Vec3f>> deltas;

			for (Collision &col : collisions)
			{
				PhysicsEntity *entity_a = col.entity_a;
				PhysicsEntity *entity_b = col.entity_b;

				const real32 percent = 0.8f;
				const real32 epsilon = 0.01f;

				real32 aimass = entity_a->GetInverseMass();
				real32 bimass = entity_b ? entity_b->GetInverseMass() : 0.0f;

				Vec3f correction = col.info.normal * percent
					* fmax(col.info.penetration - epsilon, 0.0f)
					/ (aimass + bimass);

				Vec3f deltaA = -aimass * correction;
				Vec3f deltaB = bimass * correction;

				deltas.push_back(std::make_pair(deltaA, deltaB));
			}

			for (int32 i = 0; i < collisions.size(); i++)
			{
				PhysicsEntity *entity_a = collisions.at(i).entity_a;
				PhysicsEntity *entity_b = collisions.at(i).entity_b;

				entity_a->transform.position += deltas[i].first;
				if (entity_b) entity_b->transform.position += deltas[i].second;
			}
		}
#endif

		collisions.clear();
	}

	CollisionRegistry::CollisionRegistry()
	{
		instance = this;
	}

	CollisionRegistry::~CollisionRegistry()
	{

	}

	void CollisionRegistry::Collision::Resolve(const real32 &dt)
	{
		ResolveVelocity(dt);
		ResolveInterpenetration(dt);
	}

	real32 CollisionRegistry::Collision::CalculateSeparatingSpeed()
	{
		Vec3f velo = reinterpret_cast<PhysicsEntity*>(entity_a)->veloctiy -
			reinterpret_cast<PhysicsEntity*>(entity_b)->veloctiy;

		real32 speed = Dot(velo, info.normal);

		return speed;
	}

	real32 CollisionRegistry::Collision::CalculateSeparatingAccel()
	{
		Vec3f acc = reinterpret_cast<PhysicsEntity*>(entity_a)->acceration -
			reinterpret_cast<PhysicsEntity*>(entity_b)->acceration;

		real32 accel = Dot(acc, info.normal);

		return accel;
	}

	void CollisionRegistry::Collision::ResolveVelocity(const real32 &dt)
	{
		real32 separating_speed = CalculateSeparatingSpeed();

		// @NOTE: Not checking IsValid because CalculateSeparatingSpeed does
		PhysicsEntity *ent_a = reinterpret_cast<PhysicsEntity*>(entity_a);
		PhysicsEntity *ent_b = reinterpret_cast<PhysicsEntity*>(entity_b);

		if (separating_speed > 0) return;

		real32 new_separating_speed = -separating_speed * restitution;

		real32 separating_accel = CalculateSeparatingAccel() * dt;

		if (separating_accel < 0)
		{
			new_separating_speed += 0.01f * separating_accel;

			if (new_separating_speed < 0)
			{
				separating_speed = 0;
			}
		}

		real32 deltaVelocity = new_separating_speed - separating_speed;

		// @NOTE: Adding both masses gives. 1/r1 + 1/r2 = (r1 + r2) / (r1 * r2)
		real32 timass = ent_a->GetInverseMass() + ent_b->GetInverseMass();

		real32 imp = deltaVelocity * timass;

		Vec3f impluse = imp * info.normal;

		ent_a->veloctiy += impluse * ent_a->GetInverseMass();
		ent_b->veloctiy -= impluse * ent_b->GetInverseMass();
	}

	void CollisionRegistry::Collision::ResolveInterpenetration(const real32 &dt)
	{
		PhysicsEntity *ent_a = reinterpret_cast<PhysicsEntity*>(entity_a);
		PhysicsEntity *ent_b = reinterpret_cast<PhysicsEntity*>(entity_b);

		if (info.penetration <= 0.0f) return;

		// @NOTE: Adding both masses gives. 1/r1 + 1/r2 = (r1 + r2) / (r1 * r2)
		real32 tmass = ent_a->GetMass() + ent_b->GetMass();

		if (Equal(tmass, 0.0f)) return;

		Vec3f correction = info.normal * info.penetration * (1.0f / tmass);

		move_a = correction * ent_a->GetMass();
		move_b = -1.0f * correction * ent_b->GetMass();

		ent_a->transform.position += move_a;
		ent_b->transform.position += move_b;
	}

	void CollisionDetector::FindCollisions(World *world, CollisionRegistry *collision_registry, const real32 &dt)
	{
		for (Connection &spring : ForceRegistry::instance->connected_springs)
		{
			if (spring.type == ConnectionType::ROD
				|| spring.type == ConnectionType::CABLE)
			{
				spring.ApplyForce(dt);
			}
		}

		// @TODO: Test a physics layer ?
		std::vector<Entity *> valid_entities = world->CreateCollection([](Entity *ent) {
			return (ent->HasCollider()) || (ent->behaviour == EntityBehaviour::PHYSICS_ENTITY);
			//return (ent->behaviour == EntityBehaviour::PHYSICS_ENTITY);
		});

		for (int32 entity_index = 0; entity_index < valid_entities.size(); entity_index++)
		{
			if (valid_entities.at(entity_index)->behaviour == EntityBehaviour::PHYSICS_ENTITY)
			{
				PhysicsEntity *phs1 = reinterpret_cast<PhysicsEntity*>(valid_entities.at(entity_index));

				for (int32 other_index = 0; other_index < valid_entities.size(); other_index++)
				{
					Entity *ent2 = valid_entities.at(other_index);

					if (phs1 == ent2) continue;

					Collider col1 = phs1->GetGlobalCollider();
					Collider col2 = ent2->GetGlobalCollider();

					if (col1.type != ColliderType::SPHERE) continue;

					IntersectionInfo info = {};
					if (col2.type == ColliderType::MESH ? CheckIntersection(col1.sphere, col2.mesh, &info) :
						col2.type == ColliderType::SPHERE ? CheckIntersection(col1.sphere, col2.sphere, &info) :
						false
						)						//if (col2.type == ColliderType::MESH && CheckIntersection(col1, col2.mesh, &info))
					{
						CollisionRegistry::Collision col;
						col.entity_a = phs1;
						col.restitution = 0.9f;
						col.info = info;

						if (ent2->behaviour == EntityBehaviour::PHYSICS_ENTITY)
						{
							col.entity_b = reinterpret_cast<PhysicsEntity*>(ent2);
						}
						else
						{
							col.entity_b = nullptr;
							col.info.normal = col.info.normal * -1.0f;
						}



						col.name = phs1->GetName() + " vs " + ent2->GetName();
						//LOG(col.name);
						//LOG(col.info.penetration);
						//LOGTS(col.info.normal);

						collision_registry->collisions.push_back(col);
					}
				}
			}
		}
	}

	CollisionDetector::CollisionDetector()
	{

	}

	CollisionDetector::~CollisionDetector()
	{

	}

}

