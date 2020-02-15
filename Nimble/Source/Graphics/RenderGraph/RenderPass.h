#pragma once

#include <EASTL/functional.h>
#include <EASTL/vector.h>

#include "../BackendTypedefs.h"
#include "../Enums.h"
#include "../../Math/MathCommon.h"
#include "../DX11/DXCommandBuffer.h"
#include "RenderPassBuilder.h"
#include "../../Entities/EntityManager.h"

namespace Nimble
{
	struct TransientResource;

	struct TransientDescriptor
	{
		uint32_t index;
		uint32_t binding;
	};

	struct RenderPass
	{
		eastl::string name;

		CommandBufferType commandBuffer;
		eastl::function<void(EntityManager* const, CommandBufferType* const)> Executor;

		eastl::vector<TransientDescriptor> inputStorageBuffers;
		eastl::vector<TransientDescriptor> inputTextures;

		eastl::vector<TransientDescriptor> outputStorageBuffers;
		eastl::vector<TransientDescriptor> outputTextures;

		eastl::vector<uint32_t> targetTextures;

		RenderPass(eastl::string name_, GraphicsContextType* graphicsContext, ResourceManagerType* resourceManager);

		void Register(const uint32_t renderPassIndex, RenderPassBuilder* builder, 
			eastl::function<eastl::function<void(EntityManager* const, CommandBufferType* const)>(RenderPassBuilder* const)> setup);
	};
}