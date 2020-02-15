#include "NimblePCH.h"

#include "EntityManager.h"
#include "SystemDispatch.h"

namespace Nimble {
	EntityManager::EntityManager() : systemDispatch(eastl::make_unique<SystemDispatch>(this)) {}

	EntityManager::~EntityManager() {
		systemDispatch->Shutdown();
		systemDispatch.reset();
		for (auto&& pool : componentPools) {
			// We can break at the first null-pointer since component pools
			// cannot be unregistered (destroyed) at runtime and thus when we
			// encounter a nullptr, no pools are left in the array.
			if (pool.get() == nullptr) break;
			pool.reset();
		}
	}

	Entity EntityManager::Create() {
		// If the freelist is empty, just add a new entity with version 1
		if (entityFreeList.empty()) {
			auto entity = Entity(EntityID(nextEntityId, entityVersions[nextEntityId]));
			sparse[entity.id.index] = static_cast<uint32_t>(entities.size());
			entities.push_back(entity);

			nextEntityId++;
			return entity;
		}
		// Otherwise we use a free index from the freelist
		else {
			auto index = entityFreeList.front();
			entityFreeList.pop_front();

			auto entity = Entity(EntityID(index, entityVersions[index]));
			sparse[index] = static_cast<uint32_t>(entities.size());
			entities.push_back(entity);
			return entity;
		}
	}

	void EntityManager::Destroy(const Entity& entity) {
		// Destroy component pools one by one
		for (auto&& pool : componentPools) {
			// We can break at the first null-pointer since component pools
			// cannot be unregistered (destroyed) at runtime and thus when we
			// encounter a nullptr, no pools are left in the array.
			if (pool.get() == nullptr) break;
			pool->RemoveIfExists(entity.id);
		}

		// Remove by swap and pop
		auto lastEntity = entities.back();
		sparse[lastEntity.id.index] = sparse[entity.id.index];
		eastl::swap(entities[sparse[entity.id.index]], lastEntity);
		entities.pop_back();
		sparse[entity.id.index] = 0xFFFFFFFF;

		entityVersions[entity.id.index]++;
		entityFreeList.push_front(entity.id.index);
	}

	bool EntityManager::IsValid(const Entity& entity) {
		return entity.id.version == entityVersions[entity.id.index];
	}
}