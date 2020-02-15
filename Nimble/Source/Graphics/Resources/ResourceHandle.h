#pragma once

#include <cstdint>

namespace Nimble
{
	// At the moment we only support 65534 deletions of the same resource slot
	// And 65534 unique resources of the same type
	template<typename T>
	struct ResourceHandle
	{
		uint16_t index = eastl::numeric_limits<uint16_t>::max();
		uint16_t version = eastl::numeric_limits<uint16_t>::max();

		inline bool operator==(const ResourceHandle<T>& other) const
		{
			return index == other.index && version == other.version;
		}

		inline bool operator!=(const ResourceHandle<T>& other) const
		{
			return index != other.index || version != other.version;
		}

		inline const bool IsNullHandle()
		{
			return index == eastl::numeric_limits<uint16_t>::max() && version == eastl::numeric_limits<uint16_t>::max();
		}
	};
}