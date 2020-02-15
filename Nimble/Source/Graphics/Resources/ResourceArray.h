#pragma once

#include <EASTL/array.h>
#include <EASTL/deque.h>
#include <EASTL/type_traits.h>

#include "ResourceHandle.h"
#include "../../Application/Logging.h"

namespace Nimble
{
	template<typename T, size_t Size>
	struct ResourceArray
	{
		// 65535 is reserved for the null handle
		static_assert(Size < eastl::numeric_limits<uint16_t>::max());

		uint16_t resourceIndex = 0;
		eastl::array<T, Size> elements{};
		eastl::array<uint16_t, Size> versions{};
		eastl::deque<uint16_t> freeList;

		template<typename U>
		T& operator[](ResourceHandle<U> handle)
		{
			static_assert(eastl::is_same<T, U>::value, "Resource has a different type than the resource handle passed!");
			NIMBLE_ASSERT(handle.index != eastl::numeric_limits<uint16_t>::max() && handle.version != eastl::numeric_limits<uint16_t>::max() && "Null handle passed!");
			NIMBLE_ASSERT(handle.index < Size && "Resource array out of bounds!");
			NIMBLE_ASSERT(handle.version == versions[handle.index] && "Handle passed does not match internal handle!");
			return elements[handle.index];
		}

		template<typename U>
		void DestroyResource(ResourceHandle<U> handle)
		{
			static_assert(eastl::is_same<T, U>::value, "Resource has a different type than the resource handle passed!");
			NIMBLE_ASSERT(handle.index != eastl::numeric_limits<uint16_t>::max() && handle.version != eastl::numeric_limits<uint16_t>::max() && "Null handle passed!");
			NIMBLE_ASSERT(handle.index < Size && "Resource array out of bounds!");
			NIMBLE_ASSERT(handle.version == versions[handle.index] && "Handle passed does not match internal handle!");
			versions[handle.index]++;
			freeList.push_back(handle.index);
		}

		ResourceHandle<T> GetHandle(uint16_t index)
		{
			NIMBLE_ASSERT(index < Size && versions[index] < Size && "Handle index out of bounds!");
			return ResourceHandle<T> { index, versions[index] };
		}

		ResourceHandle<T> GetNextHandle()
		{
			uint16_t next = resourceIndex;
			if (freeList.empty())
			{
				resourceIndex++;
			}
			else
			{
				next = freeList.front();
				freeList.pop_front();
			}
			NIMBLE_ASSERT(next < Size && versions[next] < Size && "Resource array full, too many elements!");
			return ResourceHandle<T> { next, versions[next] };
		}
	};
}
