#pragma once

#include <EASTL/array.h>
#include <EASTL/chrono.h>
#include <EASTL/deque.h>
#include <EASTL/tuple.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/vector.h>

#include "../Application/Logging.h"
#include "EntityTypes.h"
#include "EntityCommandBuffer.h"
#include "SparseSet.h"

namespace Nimble {
	class SystemDispatch;
	/*
	Note on entity manager behaviour:
	On release builds registering a component twice, adding a component twice
	or removing a non-existent component results in undefined behaviour
	In debug-mode asserts should catch this
	*/
	class EntityManager {
	public:
		eastl::unique_ptr<SystemDispatch> systemDispatch;
		eastl::array<eastl::unique_ptr<ComponentPool>, MAX_COMPONENT_COUNT> componentPools;

		EntityManager();
		~EntityManager();

		Entity Create();
		void Destroy(const Entity& entity);
		bool IsValid(const Entity& entity);

		template<typename C>
		SparseSet<C>* GetComponentStoragePtr() const {
			size_t componentID = ComponentID::GetUnique<C>();
			return reinterpret_cast<SparseSet<C>*>(componentPools[componentID].get());
		}

		template<typename C>
		void RegisterComponent() {
			size_t componentID = ComponentID::GetUnique<C>();
			auto componentStorage = GetComponentStoragePtr<C>();
			NIMBLE_ASSERT(componentStorage == nullptr && "Failed to register component: Component is already registered with the entity manager\n");
			componentPools[componentID] = eastl::make_unique<SparseSet<C>>();
		}

		template<typename... Cs>
		void RegisterComponents() {
			(RegisterComponent<Cs>(), ...);
		}

		template<typename C>
		void AddComponent(const Entity& entity) const {
			auto* componentStorage = GetComponentStoragePtr<C>();
			NIMBLE_ASSERT(entity != NULL_ENTITY && "Cannot add component to a null entity!");
			NIMBLE_ASSERT(componentStorage != nullptr && "Failed to add component: Component isn't registered with the entity manager\n");
			componentStorage->Insert(entity.id);
		}

		// Aggregate initialization
		template<typename C>
		void AddComponent(const Entity& entity, C&& t) const {
			auto* componentStorage = GetComponentStoragePtr<C>();
			NIMBLE_ASSERT(entity != NULL_ENTITY && "Cannot add component to a null entity!");
			NIMBLE_ASSERT(componentStorage != nullptr && "Failed to add component: Component isn't registered with the entity manager\n");
			componentStorage->Insert(entity.id, eastl::move(t));
		}

		// Copy initialization
		template<typename C>
		void AddComponent(const Entity& entity, C& t) const {
			auto* componentStorage = GetComponentStoragePtr<C>();
			NIMBLE_ASSERT(entity != NULL_ENTITY && "Cannot add component to a null entity!");
			NIMBLE_ASSERT(componentStorage != nullptr && "Failed to add component: Component isn't registered with the entity manager\n");
			componentStorage->Insert(entity.id, t);
		}

		template<typename... Cs>
		void AddComponents(const Entity& entity) const {
			(AddComponent<Cs>(entity), ...);
		}

		template<typename C>
		C* const GetComponentPtr(const Entity& entity) const {
			return GetComponentStoragePtr<C>()->GetComponentPtr(entity.id);
		}

		template<typename C>
		bool HasComponent(const Entity& entity) const {
			return GetComponentStoragePtr<C>()->HasComponent(entity.id);
		}

		template<typename C>
		void RemoveComponent(const Entity& entity) const {
			auto* componentStorage = GetComponentStoragePtr<C>();
			NIMBLE_ASSERT(componentStorage != nullptr && "Failed to remove component: Component isn't registered with the entity manager\n");
			componentStorage->Remove(entity.id);
		}

		template<typename C>
		using SparseSetPtr = SparseSet<C>*;

		template<typename... Components, typename F>
		void ForEach(F fun) {
			eastl::tuple<SparseSetPtr<Components>...> pools{ GetPool<Components>()... };

			// Get the smallest pool to iterate from as a base
			eastl::vector<uint32_t> smallest = eastl::get<0>(pools)->entityFromComponentIndices;
			eastl::apply([&](auto& ...value) {(..., UpdateSmallest(value, smallest)); }, pools);

			for (auto entityIndex : smallest) {
				const bool allValid = ((eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex] != 0xFFFF'FFFF) && ...);
				if (!allValid) continue;

				fun(entities[sparse[entityIndex]], eastl::get<SparseSet<Components>*>(pools)->dense[eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex]]...);

				(RemoveTag<Components>(entities[sparse[entityIndex]]), ...);
			}
		}

		template<typename... Components, typename F>
		void ForEachWithCommandBuffer(F fun) {
			eastl::tuple<SparseSetPtr<Components>...> pools{ GetPool<Components>()... };

			// Get the smallest pool to iterate from as a base
			eastl::vector<uint32_t> smallest = eastl::get<0>(pools)->entityFromComponentIndices;
			eastl::apply([&](auto& ...value) {(..., UpdateSmallest(value, smallest)); }, pools);

			// Command buffer passed to the lambda, for recording commands that has to run after the for loop
			EntityCommandBuffer commandBuffer(this);

			for (auto entityIndex : smallest) {
				const bool allValid = ((eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex] != 0xFFFF'FFFF) && ...);
				if (!allValid) continue;

				fun(entities[sparse[entityIndex]], &commandBuffer, eastl::get<SparseSet<Components>*>(pools)->dense[eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex]]...);

				(RemoveTag<Components>(entities[sparse[entityIndex]]), ...);
			}

			// Play back commands
			commandBuffer.PlayCommands();
		}

		template<typename... Components, typename F, typename T>
		void ForEachAddTag(F fun, T tag) {
			eastl::tuple<SparseSetPtr<Components>...> pools{ GetPool<Components>()... };

			// Get the smallest pool to iterate from as a base
			eastl::vector<uint32_t> smallest = eastl::get<0>(pools)->entityFromComponentIndices;
			eastl::apply([&](auto& ...value) {(..., UpdateSmallest(value, smallest)); }, pools);

			for (auto entityIndex : smallest) {
				const bool allValid = ((eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex] != 0xFFFF'FFFF) && ...);
				if (!allValid) continue;

				fun(entities[sparse[entityIndex]], eastl::get<SparseSet<Components>*>(pools)->dense[eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex]]...);

				(RemoveTag<Components>(entities[sparse[entityIndex]]), ...);
				AddTag<T>(entities[sparse[entityIndex]]);
			}
		}

		template<typename... Components, typename F>
		void ParForEach(F fun) {
			eastl::tuple<SparseSetPtr<Components>...> pools{ GetPool<Components>()... };

			// Get the smallest pool to iterate from as a base
			eastl::vector<uint32_t> smallest = eastl::get<0>(pools)->entityFromComponentIndices;
			eastl::apply([&](auto& ...value) {(..., UpdateSmallest(value, smallest)); }, pools);

			// TODO: MAKE PARALLEL
			for (auto entityIndex : smallest) {
				const bool allValid = ((eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex] != 0xFFFF'FFFF) && ...);
				if (!allValid) return;

				fun(entities[sparse[entityIndex]], eastl::get<SparseSet<Components>*>(pools)->dense[eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex]]...);
			}

			for (auto entityIndex : smallest) {
				(RemoveTag<Components>(entities[sparse[entityIndex]]), ...);
			}
		}

		template<typename... Components, typename F>
		void ParForEachWithCommandBuffer(F fun) {
			eastl::tuple<SparseSetPtr<Components>...> pools{ GetPool<Components>()... };

			// Get the smallest pool to iterate from as a base
			eastl::vector<uint32_t> smallest = eastl::get<0>(pools)->entityFromComponentIndices;
			eastl::apply([&](auto& ...value) {(..., UpdateSmallest(value, smallest)); }, pools);

			// Command buffer passed to the lambda, for recording commands that has to run after the for loop
			EntityCommandBuffer commandBuffer(this);

			// TODO: MAKE PARALLEL
			for (auto entityIndex : smallest) {
				const bool allValid = ((eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex] != 0xFFFF'FFFF) && ...);
				if (!allValid) return;

				fun(entities[sparse[entityIndex]], &commandBuffer, eastl::get<SparseSet<Components>*>(pools)->dense[eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex]]...);
			}

			for (auto entityIndex : smallest) {
				(RemoveTag<Components>(entities[sparse[entityIndex]]), ...);
			}

			// Play back commands
			commandBuffer.PlayCommands();
		}

		template<typename... Components, typename F, typename T>
		void ParForEachAddTag(F fun, T tag) {
			eastl::tuple<SparseSetPtr<Components>...> pools{ GetPool<Components>()... };

			// Get the smallest pool to iterate from as a base
			eastl::vector<uint32_t> smallest = eastl::get<0>(pools)->entityFromComponentIndices;
			eastl::apply([&](auto& ...value) {(..., UpdateSmallest(value, smallest)); }, pools);

			// TODO: MAKE PARALLEL
			for (auto entityIndex : smallest) {
				const bool allValid = ((eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex] != 0xFFFF'FFFF) && ...);
				if (!allValid) return;

				fun(entities[sparse[entityIndex]], eastl::get<SparseSet<Components>*>(pools)->dense[eastl::get<SparseSet<Components>*>(pools)->sparse[entityIndex]]...);
			}

			for (auto entityIndex : smallest) {
				(RemoveTag<Components>(entities[sparse[entityIndex]]), ...);
				AddTag<T>(entities[sparse[entityIndex]]);
			}
		}

		uint32_t GetEntityVersion(const EntityID id) {
			return entityVersions[id.version];
		}

	private:
		uint32_t nextEntityId = 0;

		eastl::vector<uint32_t> sparse = eastl::vector<uint32_t>(MAX_ENTITY_COUNT, 0xFFFF'FFFF);
		eastl::vector<uint32_t> entityVersions = eastl::vector<uint32_t>(MAX_ENTITY_COUNT, 1);
		eastl::vector<Entity> entities;

		// Holds free entity indices
		eastl::deque<uint32_t> entityFreeList;

		template<typename C>
		SparseSet<C>* const GetPool() const {
			size_t componentID = ComponentID::GetUnique<C>();
			NIMBLE_ASSERT(componentPools[componentID] != nullptr && "Failed to get pool: Component isn't registered with the entity manager\n");
			return reinterpret_cast<SparseSet<C>*>(componentPools[componentID].get());
		}

		template<typename C>
		void UpdateSmallest(SparseSet<C>* current, eastl::vector<uint32_t>& currentSmallest) {
			if (current->entityFromComponentIndices.size() < currentSmallest.size()) {
				currentSmallest = current->entityFromComponentIndices;
			}
		}

		template<typename C>
		void RemoveTag(Entity entity) {
			if constexpr (eastl::is_base_of_v<EventTagComponent, C>) {
				RemoveComponent<C>(entity);
			}
		}

		template<typename C>
		void AddTag(Entity entity) {
			if constexpr (eastl::is_base_of_v<EventTagComponent, C>) {
				AddComponent<C>(entity);
			}
		}
	};

	struct ComponentSystem {
		using clock = eastl::chrono::high_resolution_clock;
		float updateTime = 0.0f;
		eastl::chrono::time_point<clock> tStart;
		const char* name = nullptr;

		EntityManager* entityManager = nullptr;

		virtual ~ComponentSystem() = default;

		void UpdateStart() {
			tStart = clock::now();
		}

		void UpdateEnd() {
			auto tEnd = clock::now();

			// Update deltatime
			updateTime = static_cast<eastl::chrono::duration<float, eastl::ratio<1>>>(tEnd - tStart).count();
		}
	};
}