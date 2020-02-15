#pragma once

#include <EASTL/fixed_vector.h>

#include "EntityTypes.h"
#include "../Application/Logging.h"

namespace Nimble {
	template<typename T>
	class SparseSet : public ComponentPool {
	public:
		eastl::fixed_vector<uint32_t, MAX_ENTITY_COUNT> sparse = eastl::fixed_vector<uint32_t, MAX_ENTITY_COUNT>(MAX_ENTITY_COUNT, 0xFFFF'FFFF);
		//std::vector<uint32_t> sparse_ = std::vector<uint32_t>(INIT_MAX_ENTITIES, 0xFFFF'FFFF);
		
		eastl::vector<T> dense;
		eastl::vector<uint32_t> entityFromComponentIndices;

		SparseSet() {
			//NIMBLE_LOG_INFO("Sparse of type \"%s\" (%zi bytes) created\n", typeid(T).name(), sizeof(T));
		}

		void Insert(EntityID id) {
			NIMBLE_ASSERT(id.index < sparse.size());
			NIMBLE_ASSERT(sparse[id.index] == 0xFFFF'FFFF && "Failed to add component: Component is already part of entity");

			sparse[id.index] = static_cast<uint32_t>(dense.size());
			dense.push_back(T());
			entityFromComponentIndices.push_back(id.index);
		}

		void Insert(EntityID id, T&& t) {
			NIMBLE_ASSERT(id.index < sparse.size());
			NIMBLE_ASSERT(sparse[id.index] == 0xFFFF'FFFF && "Failed to add component: Component is already part of entity");

			sparse[id.index] = static_cast<uint32_t>(dense.size());
			dense.push_back(t);
			entityFromComponentIndices.push_back(id.index);
		}

		void Insert(EntityID id, T& t) {
			NIMBLE_ASSERT(id.index < sparse.size());
			NIMBLE_ASSERT(sparse[id.index] == 0xFFFF'FFFF && "Failed to add component: Component is already part of entity");

			sparse[id.index] = static_cast<uint32_t>(dense.size());
			dense.push_back(t);
			entityFromComponentIndices.push_back(id.index);
		}

		void Remove(EntityID id) {
			NIMBLE_ASSERT(id.index < sparse.size());
			NIMBLE_ASSERT(sparse[id.index] != 0xFFFF'FFFF && "Failed to remove component: Component is not part of the entity");

			// Removal works by swap and pop
			uint32_t lastIndex = FindEntityID(static_cast<uint32_t>(dense.size()) - 1);
			eastl::swap(dense[sparse[id.index]], dense[sparse[lastIndex]]);
			eastl::swap(entityFromComponentIndices[sparse[id.index]], entityFromComponentIndices[sparse[lastIndex]]);
			sparse[lastIndex] = sparse[id.index];
			// id.index is now free to be used (add to free list)
			sparse[id.index] = 0xFFFFFFFF;

			//if constexpr(SFINAE::is_detected_v<has_cleanup, T>) {
			//	dense_.back()->Cleanup();
			//}

			dense.pop_back();
			entityFromComponentIndices.pop_back();
		}

		void RemoveIfExists(EntityID id) override {
			NIMBLE_ASSERT(id.index < sparse.size());
			if (sparse[id.index] == 0xFFFF'FFFF) return;

			// Removal works by swap and pop
			uint32_t lastIndex = FindEntityID(static_cast<uint32_t>(dense.size()) - 1);
			eastl::swap(dense[sparse[id.index]], dense[sparse[lastIndex]]);
			eastl::swap(entityFromComponentIndices[sparse[id.index]], entityFromComponentIndices[sparse[lastIndex]]);
			sparse[lastIndex] = sparse[id.index];
			// id.index is now free to be used (add to free list)
			sparse[id.index] = 0xFFFFFFFF;

			//if constexpr(SFINAE::is_detected_v<has_cleanup, T>) {
			//	dense_.back()->Cleanup();
			//}

			dense.pop_back();
			entityFromComponentIndices.pop_back();
		}

		bool HasComponent(EntityID id) {
			NIMBLE_ASSERT(id.index < sparse.size());
			return sparse[id.index] != 0xFFFF'FFFF;
		}

		T* const GetComponentPtr(EntityID id) {
			NIMBLE_ASSERT(id.index < sparse.size());
			NIMBLE_ASSERT(sparse[id.index] != 0xFFFF'FFFF && "Failed to get component: Component is not part of the entity");

			return &dense[sparse[id.index]];
		}

		uint32_t FindEntityID(uint32_t denseIndex) {
			NIMBLE_ASSERT(denseIndex < entityFromComponentIndices.size());

			return entityFromComponentIndices[denseIndex];
		}

		T* GetRawDataPtr() {
			return dense.data();
		}
	};
}
