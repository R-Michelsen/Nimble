#include "NimblePCH.h"
#include "RenderPass.h"

#include "../DX11/DXGraphicsContext.h"
#include "../DX11/DXResourceManager.h"
#include "TransientResource.h"

namespace Nimble
{
	RenderPass::RenderPass(eastl::string name_, GraphicsContextType* graphicsContext, ResourceManagerType* resourceManager) :
		name(name_),
		commandBuffer(CommandBufferType(graphicsContext, resourceManager))
	{
	}
	
	void RenderPass::Register(const uint32_t renderPassIndex, RenderPassBuilder* builder,
		eastl::function<eastl::function<void(EntityManager* const, CommandBufferType* const)>(RenderPassBuilder* const)> setup)
	{
		builder->Attach(renderPassIndex);
		Executor = setup(builder);
	}
}
