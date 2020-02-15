#pragma once
#include <cstdint>

namespace Nimble {
	constexpr size_t MAX_SYSTEM_COUNT = 256;
	constexpr size_t MAX_COMPONENT_COUNT = 128;
	constexpr size_t MAX_ENTITY_COUNT = 1'000'000;

	class SystemID {
	public:
		template<typename>
		static size_t GetUnique() noexcept {
			static const size_t val = GetID();
			return val;
		}
	private:
		static size_t GetID() noexcept {
			static size_t val = 0;
			return val++;
		}
	};

	class ComponentID {
	public:
		template<typename>
		static size_t GetUnique() noexcept {
			static const size_t val = GetID();
			return val;
		}
	private:
		static size_t GetID() noexcept {
			static size_t val = 0;
			return val++;
		}
	};

	struct EntityID {
		uint32_t index : 24;
		uint32_t version : 8;

		EntityID() : index(0), version(0) {}
		EntityID(uint32_t id, uint32_t version) : index(id), version(version) {};

		bool operator == (const EntityID& other) const { return index == other.index; }
		bool operator != (const EntityID& other) const { return index != other.index; }
	};

	struct Entity {
		EntityID id;

		Entity()
		{
			id = { 0xFFFF'FFFF, 0xFFFF'FFFF };
		}
		Entity(EntityID id) : id(id) {}

		bool operator == (const Entity& other) const { return (id == other.id); }
		bool operator != (const Entity& other) const { return (id != other.id); }
	};

	inline const Entity NULL_ENTITY{ {0xFFFF'FFFF, 0xFFFF'FFFF} };

	struct ComponentPool {
		virtual ~ComponentPool() = default;
		virtual void RemoveIfExists(EntityID id) = 0;
	};

	struct EventTagComponent {};
}