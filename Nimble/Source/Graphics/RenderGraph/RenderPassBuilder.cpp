#include "NimblePCH.h"
#include "RenderPassBuilder.h"

#include "RenderGraph.h"
#include "RenderPass.h"
#include "TransientResource.h"

namespace Nimble
{
	RenderPassBuilder::RenderPassBuilder(RenderGraph* renderGraph_) :
		passIndex(eastl::numeric_limits<uint32_t>::max()),
		renderGraph(renderGraph_)
	{
	}

	void RenderPassBuilder::Attach(RenderPassHandle passIndex_)
	{
		passIndex = passIndex_;
	}

	void RenderPassBuilder::Read(TransientTexture& resource, const uint32_t binding)
	{
		auto& renderPass = renderGraph->passes[passIndex];

		// Register the transient resource in the render graph if it isn't already
		const uint32_t index = RegisterTransientResource(resource);

		// Register the render pass as a reader of the transient resource
		auto& texture = renderGraph->transientTextures[index];
		texture.readers.push_back(passIndex);

		// Register the resource in the inputs of the render pass
		renderPass.inputTextures.push_back({ index, binding });
	}

	void RenderPassBuilder::Read(TransientStorageBuffer& resource, const uint32_t binding)
	{
		auto& renderPass = renderGraph->passes[passIndex];

		// Register the transient resource in the render graph if it isn't already
		const uint32_t index = RegisterTransientResource(resource);

		// Register the render pass as a reader of the transient resource
		auto& buffer = renderGraph->transientBuffers[index];
		buffer.readers.push_back(passIndex);

		// Register the resource in the inputs of the render pass
		renderPass.inputStorageBuffers.push_back({ index, binding });
	}

	void RenderPassBuilder::Write(TransientTexture& resource, const uint32_t binding)
	{
		NIMBLE_ASSERT(binding <= D3D11_1_UAV_SLOT_COUNT && "Binding is outside the bounds of the maximum number of allowed output resources!");
		auto& renderPass = renderGraph->passes[passIndex];

		// Register the transient resource in the render graph if it isn't already
		const uint32_t index = RegisterTransientResource(resource);

		// Register the render pass as a reader of the transient resource
		auto& texture = renderGraph->transientTextures[index];
		texture.writers.push_back(passIndex);

		// Register the resource in the inputs of the render pass
		renderPass.outputTextures.push_back({ index, binding });
	}

	void RenderPassBuilder::Write(TransientStorageBuffer& resource, const uint32_t binding)
	{
		NIMBLE_ASSERT(binding <= D3D11_1_UAV_SLOT_COUNT && "Binding is outside the bounds of the maximum number of allowed output resources!");
		auto& renderPass = renderGraph->passes[passIndex];

		// Register the transient resource in the render graph if it isn't already
		const uint32_t index = RegisterTransientResource(resource);

		// Register the render pass as a reader of the transient resource
		auto& buffer = renderGraph->transientBuffers[index];
		buffer.writers.push_back(passIndex);

		// Register the resource in the inputs of the render pass
		renderPass.outputStorageBuffers.push_back({ index, binding });
	}

	void RenderPassBuilder::WriteToRenderTargets(eastl::vector<TransientTexture> targets)
	{
		auto& renderPass = renderGraph->passes[passIndex];
		NIMBLE_ASSERT(renderPass.targetTextures.empty() && "Render targets must only be specified once!");
		NIMBLE_ASSERT(targets.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

		for (auto& target : targets)
		{
			const uint32_t index = RegisterTransientResource(target);

			// Register the render pass as a reader of the transient resource
			auto& resource = renderGraph->transientTextures[index];
			resource.writers.push_back(passIndex);

			// Register the resource in the render targets of the render pass
			renderPass.targetTextures.push_back(index);
		}
	}

	void RenderPassBuilder::WriteToBackBufferRenderTarget()
	{
		auto texture = TransientTexture("NIMBLE_BACKBUFFER", ShaderStageFlags::UnknownStage, TextureFormat::UNKNOWN, TextureSize::Absolute, { 0, 0 }, 0);
		WriteToRenderTargets({ texture });
	}

	uint32_t RenderPassBuilder::RegisterTransientResource(TransientTexture& resource)
	{
		if (renderGraph->textureIndices.find(resource.identifier) == renderGraph->textureIndices.end())
		{
			const uint32_t index = static_cast<uint32_t>(renderGraph->transientTextures.size());
			renderGraph->transientTextures.push_back(resource);
			renderGraph->textureIndices[resource.identifier] = index;
			return index;
		}
		else
		{
			return renderGraph->textureIndices[resource.identifier];
		}
	}

	uint32_t RenderPassBuilder::RegisterTransientResource(TransientStorageBuffer& resource)
	{
		if (renderGraph->bufferIndices.find(resource.identifier) == renderGraph->bufferIndices.end())
		{
			const uint32_t index = static_cast<uint32_t>(renderGraph->transientBuffers.size());
			renderGraph->transientBuffers.push_back(resource);
			renderGraph->bufferIndices[resource.identifier] = index;
			return index;
		}
		else
		{
			return renderGraph->bufferIndices[resource.identifier];
		}
	}
}
