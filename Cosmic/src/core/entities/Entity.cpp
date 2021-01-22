#include "Entity.h"

#include "src/core/World.h"

namespace cm
{
	bool Entity::IsValid()
	{
		int32 stored_id = world->entities.at(index).get()->id;

		return (type != EntityType::INVALID && stored_id == id);
	}

	void Entity::SetParent(Entity *entity)
	{
		Entity *me = this;
		ASSERT(me, "SetParent me is an invalid entities");

		entity->IsValid();


		// @NOTE: Do we have an existing parent
		if (me->parent)
		{
			// @NOTE: Grab the existing parent
			Entity *other_parent = world->entities.at(me->parent).get();
			if (other_parent)
			{
				// @NOTE: prev -> me -> next
				Entity *prev = nullptr;
				Entity *next = nullptr;

				// @NOTE: Find prev and next
				Entity *child = world->entities.at(other_parent->child).get();
				while (child->index)
				{
					if (child->sibling == me->index)
					{
						prev = child;
					}
					if (me->sibling == child->index)
					{
						next = child;
					}
					child = world->entities.at(child->sibling).get();
				}

				if (!prev && !next) // @NOTE: me was the only child
				{
					other_parent->child = 0;
				}
				else if (prev && !next) // @NOTE: me was the last child
				{
					prev->sibling = 0;
				}
				else if (!prev && next) // @NOTE: me the the first child
				{
					other_parent->child = next->index;
				}
				else if (prev && next) // @NOTE: me was a middle child
				{
					prev->sibling = next->index;
				}
				else
				{
					ASSERT(0, "SetParent went wrong");
				}
			}
			else
			{
				ASSERT(0, "SetParent went wrong");
			}
		}

		if (entity)
		{
			Entity *parent = entity;
			me->parent = parent->index;

			// @NOTE: He new parent has child place me at the back
			if (parent->child)
			{
				Entity *child = world->entities.at(parent->child).get();

				while (child)
				{
					if (child->sibling)
					{
						child = world->entities.at(child->sibling).get();
					}
					else
					{
						break;
					}
				}

				child->sibling = me->index;
			}
			else // @NOTE: New parent has no child
			{
				parent->child = me->index;
			}
		}
		else
		{
			me->parent = 0;
		}

		me->sibling = 0;
	}

	void Entity::SetChild(Entity *entity)
	{
		if (entity)
		{
			entity->SetParent(entity);
		}
	}

	Entity *Entity::GetParent() const
	{
		return world->entities.at(parent).get();
	}

	Entity *Entity::GetFirstChild() const
	{
		return world->entities.at(child).get();
	}

	Entity *Entity::GetSibling() const
	{
		return  world->entities.at(sibling).get();
	}

	std::vector<Entity*> Entity::GetChildren() const
	{
		std::vector<Entity*> children;

		Entity *child = GetFirstChild();
		if (child->IsValid())
		{
			children.push_back(child);

			Entity *sibling = child->GetSibling();

			while (sibling->IsValid())
			{
				children.push_back(sibling);

				sibling = child->GetSibling();
			}
		}

		return children;
	}

	cm::Transform Entity::GetGlobalTransform() const
	{
		Mat4f m = GetGlobalTransformMatrix();

		return Transform(m);
	}

	cm::Mat4f Entity::GetLocalTransformMatrix() const
	{
		return transform.CalculateTransformMatrix();
	}

	cm::Mat4f Entity::GetGlobalTransformMatrix() const
	{
		Mat4f me = transform.CalculateTransformMatrix();

		Entity *next = GetParent();

		while (next && next->IsValid())
		{
			Mat4f other = next->GetLocalTransformMatrix();

			me = me * other;

			next = next->GetParent();
		}

		return me;
	}

	cm::AABB Entity::GetGlobalBoundingBox() const
	{
		Transform global_transform = GetGlobalTransform();
		return UpdateAABB(local_bounding_box,
			global_transform.position, global_transform.orientation, global_transform.scale);
	}

	cm::Collider Entity::GetGlobalCollider() const
	{
		Collider collider = local_collider;

		switch (local_collider.type)
		{
		case ColliderType::PLANE:
			break;
		case ColliderType::SPHERE:
		{
			Transform m = GetGlobalTransform();
			collider = CreateSphere(m.position, GetSphereRadius(collider.sphere));
			break;
		}
		case ColliderType::AABB:
			collider.aabb = UpdateAABB(collider.aabb, transform.position, transform.orientation, transform.scale);
			break;

		case ColliderType::OBB:
			collider.obb.center += transform.position;
			collider.obb.basis = transform.GetBasis(); // @NOTE: Not concatinating this ie combining an offset
			break;
		case ColliderType::CAPSULE:
		{
			Mat4 m = GetGlobalTransformMatrix();
			collider.capsule.bot = Vec3f(Vec4f(collider.capsule.bot, 1.0f) * m);
			collider.capsule.top = Vec3f(Vec4f(collider.capsule.top, 1.0f) * m);

		} break;
		case ColliderType::MESH:
		{
			Mat4 m = GetGlobalTransformMatrix();

			for (int32 i = 0; i < local_collider.mesh.size(); i++)
			{
				Triangle *triangle = &collider.mesh.at(i);

				triangle->v0 = Vec3f(Vec4f(triangle->v0, 1.0f) * m);
				triangle->v2 = Vec3f(Vec4f(triangle->v2, 1.0f) * m);
				triangle->v1 = Vec3f(Vec4f(triangle->v1, 1.0f) * m);
			}
		}break;
		}

		return collider;
	}

	void Entity::SetMesh(const MeshEntry &mesh)
	{
		this->mesh = mesh;
		local_bounding_box = GameState::GetAssetTable()->GetMeshMetaData(mesh).bounding_box;
	}

	Entity::Entity()
		: type(EntityType::INVALID), behaviour(EntityBehaviour::UPDATABLE_ENTITY), name("UNTITLED ENTITY"), mesh(-1),
		active(true), simulate_physics(false), should_draw(true),
		parent(NULL_ENTITY_INDEX), child(NULL_ENTITY_INDEX), sibling(NULL_ENTITY_INDEX)
	{

	}

	Entity::Entity(const EntityType &type, const String &name /*= "UNTITLED ENTITY"*/)
		: type(type), behaviour(EntityBehaviour::UPDATABLE_ENTITY),
		name(name), mesh(-1), active(true), simulate_physics(false), should_draw(true),
		parent(NULL_ENTITY_INDEX), child(NULL_ENTITY_INDEX), sibling(NULL_ENTITY_INDEX)
	{

	}

	Entity::Entity(const EntityType &type, const Transform &transform, const String &name /*= "UNTITLED ENTITY"*/)
		: type(type), behaviour(EntityBehaviour::UPDATABLE_ENTITY), transform(transform), name(name), mesh(-1),
		active(true), simulate_physics(false), should_draw(true),
		parent(NULL_ENTITY_INDEX), child(NULL_ENTITY_INDEX), sibling(NULL_ENTITY_INDEX)
	{

	}

	Entity::~Entity()
	{

	}

	cm::Entity *EntityReference::Get()
	{
		return world->entities.at(index).get();
	}

	bool EntityReference::IsValid()
	{
		if (world)
		{
			Entity* entity = world->entities.at(index).get();

			if (entity->type != EntityType::INVALID && entity->id == id)
			{
				return true;
			}
		}

		return false;
	}

	EntityReference::EntityReference()
		: world(nullptr), index(NULL_ENTITY_INDEX), id(NULL_ENTITY_ID)
	{

	}

	EntityReference::EntityReference(const int32 index, const int32 id, World *world)
		: world(world), index(index), id(id)
	{

	}

	EntityReference::~EntityReference()
	{

	}

}