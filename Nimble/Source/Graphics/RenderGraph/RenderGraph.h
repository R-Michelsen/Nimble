#pragma once

#include <EASTL/hash_map.h>
#include <EASTL/string.h>

#include "../../Entities/EntityManager.h"
#include "RenderPass.h"
#include "RenderPassBuilder.h"
#include "TransientResource.h"
#include "../BackendTypedefs.h"
#include "../Enums.h"
#include "../DX11/DXBuffer.h"
#include "../DX11/DXTexture2D.h"
#include "../DX11/DXGraphicsContext.h"

namespace Nimble
{
	class RenderGraph
	{
	public:
		RenderGraph(GraphicsContextType* const graphicsContext_, ResourceManagerType* const resourceManager_, EntityManager* const entityManager_) :
			renderPassBuilder(RenderPassBuilder(this)),
			graphicsContext(graphicsContext_),
			resourceManager(resourceManager_),
			entityManager(entityManager_),
			initialCommandBuffer(CommandBufferType(graphicsContext, resourceManager)) {}

		template<typename RenderPath>
		RenderPath ActivateRenderPath(const bool enablePassCulling_, const bool enableMSAA_)
		{
			enablePassCulling = enablePassCulling_;
			enableMSAA = enableMSAA_;

			graphicsContext->CreateSwapchain(enableMSAA);

			Clear();
			RenderPath path(resourceManager, entityManager, this);
			Validate();
			Compile();
			return path;
		}

		void AddRenderPass(eastl::string name, eastl::function<eastl::function<void(EntityManager* const, CommandBufferType* const)>(RenderPassBuilder* const)> setup);
		void Recompile();
		void Execute();

	private:
		friend class RenderPassBuilder;
		RenderPassBuilder renderPassBuilder;

		GraphicsContextType* const graphicsContext;
		ResourceManagerType* const resourceManager;
		EntityManager* const entityManager;

		bool enableMSAA = true;
		bool enablePassCulling = true;

		eastl::hash_map<eastl::string, uint32_t> passIndices;
		eastl::vector<RenderPass> passes;

		eastl::hash_map<eastl::string, uint32_t> bufferIndices;
		eastl::hash_map<eastl::string, uint32_t> textureIndices;
		eastl::vector<TransientStorageBuffer> transientBuffers;
		eastl::vector<TransientTexture> transientTextures;

		CommandBufferType initialCommandBuffer;
		eastl::vector<uint32_t> activeExecutionOrder;


		void Validate();
		void Compile();
		void DestroyResources();
		void Clear();

		eastl::vector<uint32_t> FlattenAndCullPasses();
		void TraversePass(const uint32_t passIndex, eastl::vector<uint32_t>& executionOrder);
		eastl::vector<uint32_t> TopologicallySortPasses();

		void ActualizeTexture(const uint32_t transientIndex, const bool isRenderTarget);
		void ActualizeBuffer(const uint32_t transientIndex);

		void BindInputDescriptors(RenderPass& renderPass);
	};
}
