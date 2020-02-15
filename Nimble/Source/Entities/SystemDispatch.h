#pragma once

#include <cstdint>

#include <EASTL/array.h>
#include <EASTL/type_traits.h>
#include <EASTL/unique_ptr.h>

#include "EntityManager.h"

namespace Nimble {
	// SFINAE Detector
	namespace SFINAE
	{
		namespace detail
		{
			template <template <typename> typename Op, typename T, typename = void>
			struct is_detected : eastl::false_type {};

			template <template <typename> typename Op, typename T>
			struct is_detected<Op, T, eastl::void_t<Op<T>>> : eastl::true_type {};
		}
		template <template <typename> typename Op, typename T>
		static constexpr bool is_detected_v = detail::is_detected<Op, T>::value;
	}

	template <class S>
	using has_init = decltype(eastl::declval<S>().Init());

	class SystemDispatch {
	public:
		EntityManager* const entityManager;
		size_t systemCount = 0;
		eastl::array<eastl::unique_ptr<ComponentSystem>, MAX_SYSTEM_COUNT> systems;

		SystemDispatch(EntityManager* const entityManager) : entityManager(entityManager) {}

		void Shutdown() {
			for (int i = 0; i < systemCount; i++) {
				systems[i].reset();
			}
		}

		template<typename S>
		void RegisterSystem() {
			static_assert(eastl::is_base_of_v<ComponentSystem, S>, "All systems must inherit from ComponentSystem");
			size_t systemID = SystemID::GetUnique<S>();
			systems[systemID] = eastl::make_unique<S>();
			systems[systemID]->entityManager = entityManager;
			systems[systemID]->name = ""; // typeid(S).name();
			if constexpr (SFINAE::is_detected_v<has_init, S>) {
				reinterpret_cast<S*>(systems[systemID].get())->Init();
			}
			systemCount++;
		}

		template<typename S, typename... Args>
		void RunSystem(float deltaTime, Args&&... args) {
			size_t systemID = SystemID::GetUnique<S>();
			if (systems[systemID] == nullptr) {
				RegisterSystem<S>();
			}
			auto* ptr = reinterpret_cast<S*>(systems[systemID].get());
			ptr->UpdateStart();
			ptr->Update(deltaTime, args...);
			ptr->UpdateEnd();
		}
	};
}
