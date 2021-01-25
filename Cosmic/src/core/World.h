#pragma once
#include "src/core/Grid.h"
#include "entities/Entity.h"
#include "src/serialization/Assets.h"
#include "Physics.h"
#include "Player.h"
#include "entities/Environment.h"

namespace cm
{
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

		Grid grid;

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
