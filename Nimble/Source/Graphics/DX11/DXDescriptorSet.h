#pragma once

#include <EASTL/vector.h>
#include "../Resources/ResourceTypes.h"

namespace Nimble
{
	struct DXDescriptor
	{
		void* resource;
		ResourceType type;
		uint32_t binding;
	};

	struct DXDescriptorSet
	{
		eastl::vector<eastl::pair<Texture2D, uint32_t>> textures;
		eastl::vector<eastl::pair<ConstantBuffer, uint32_t>> constantBuffers;
		eastl::vector<eastl::pair<StorageBuffer, uint32_t>> storageBuffers;
	};
}