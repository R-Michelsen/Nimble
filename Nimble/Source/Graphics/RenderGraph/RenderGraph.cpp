#include "NimblePCH.h"
#include "RenderGraph.h"

#include "../DX11/DXGraphicsContext.h"
#include "../DX11/DXResourceManager.h"

#include "../Resources/ResourceHandle.h"

namespace Nimble
{
	void RenderGraph::AddRenderPass(eastl::string name, eastl::function<eastl::function<void(EntityManager* const, CommandBufferType* const)>(RenderPassBuilder* const)> setup)
	{
		NIMBLE_ASSERT(passIndices.find(name) == passIndices.end() && "Cannot add render pass, there already exists a render pass with that name!");

		auto index = static_cast<uint32_t>(passes.size());
		passes.push_back(RenderPass(name, graphicsContext, resourceManager));
		passIndices[name] = index;

		passes[index].Register(index, &renderPassBuilder, setup);
	}


	void RenderGraph::Recompile()
	{
		DestroyResources();
		Compile();
	}

	void RenderGraph::Execute()
	{
		NIMBLE_ASSERT(!passes.empty() && "Can't execute render pass, no passes to execute!");

		// Clear all non-persistent transient resources
		initialCommandBuffer.ClearDefaultViews();
		for (auto& texture : transientTextures)
		{
			if (!texture.physicalTexture.handle.IsNullHandle())
			{
				initialCommandBuffer.ClearView(texture.physicalTexture);
			}
		}

		for (const auto i : activeExecutionOrder)
		{
			auto& renderPass = passes[i];

			renderPass.commandBuffer.SetTopology();

			BindInputDescriptors(renderPass);
			renderPass.commandBuffer.BindOutputDescriptorsAndRenderTargets(renderPass, transientTextures, transientBuffers);
			renderPass.commandBuffer.SetViewport(graphicsContext->windowWidth, graphicsContext->windowHeight);

			renderPass.Executor(entityManager, &renderPass.commandBuffer);

			renderPass.commandBuffer.UnbindResources(static_cast<uint32_t>(renderPass.targetTextures.size()),
				static_cast<uint32_t>(renderPass.inputTextures.size() + renderPass.inputStorageBuffers.size()));
		}
	}

	// Sanity checks to ensure that render passes are well-formed
	void RenderGraph::Validate()
	{
		// Ensure that render passes aren't read or written 
		// in the passes where they are rendered to
		for (const auto& renderPass : passes)
		{
			NIMBLE_ASSERT(!renderPass.targetTextures.empty() && "Cannot validate render graph, all render passes must write to a target!");
			for (const auto target : renderPass.targetTextures)
			{
				if (transientTextures[target].alias != "")
				{
					for (const auto input : renderPass.inputTextures)
					{
						// A render target cannot be read from or written to in the same render pass
						NIMBLE_ASSERT(transientTextures[input.index].identifier != transientTextures[target].alias
							&& "Cannot validate render graph, a render pass is reading from or writing to one of its render targets!");
					}
					for (const auto input : renderPass.inputStorageBuffers)
					{
						// A render target cannot be read from or written to in the same render pass
						NIMBLE_ASSERT(transientTextures[input.index].identifier != transientTextures[target].alias
							&& "Cannot validate render graph, a render pass is reading from or writing to one of its render targets!");
					}
				}
			}

			eastl::vector<uint32_t> inputBindChecker;
			inputBindChecker.resize(D3D11_1_UAV_SLOT_COUNT);
			eastl::vector<uint32_t> outputBindChecker;
			outputBindChecker.resize(D3D11_1_UAV_SLOT_COUNT);

			for (const auto input : renderPass.inputTextures)
			{
				if (inputBindChecker.size() < input.binding)
				{
					inputBindChecker.resize(static_cast<uint64_t>(input.binding) * 2);
				}
				NIMBLE_ASSERT(inputBindChecker[input.binding] != 1 && "Cannot validate render graph, input resource bind conflict!");
				inputBindChecker[input.binding] = 1;
			}

			for (const auto input : renderPass.inputStorageBuffers)
			{
				if (inputBindChecker.size() < input.binding)
				{
					inputBindChecker.resize(static_cast<uint64_t>(input.binding) * 2);
				}
				NIMBLE_ASSERT(inputBindChecker[input.binding] != 1 && "Cannot validate render graph, input resource bind conflict!");
				inputBindChecker[input.binding] = 1;
			}

			for (const auto output : renderPass.outputTextures)
			{
				NIMBLE_ASSERT(outputBindChecker[output.binding] != 1 && "Cannot validate render graph, iutput resource bind conflict!");
				const auto& texture = transientTextures[output.index];

				if (texture.alias != "")
				{
					// Verify that the properties of the aliased resource match the properties of the current resource
					NIMBLE_ASSERT(textureIndices.find(texture.alias) != textureIndices.end());
					const auto aliasIndex = textureIndices[texture.alias];
					const auto& aliasTexture = transientTextures[aliasIndex];
					NIMBLE_ASSERT(texture.format == aliasTexture.format);
					NIMBLE_ASSERT(texture.levels == aliasTexture.levels);
					NIMBLE_ASSERT(texture.sizeMode == aliasTexture.sizeMode);
					NIMBLE_ASSERT(texture.size.x == aliasTexture.size.x && texture.size.y == aliasTexture.size.y);
				}

				outputBindChecker[output.binding] = 1;
			}

			for (const auto output : renderPass.outputStorageBuffers)
			{
				NIMBLE_ASSERT(outputBindChecker[output.binding] != 1 && "Cannot validate render graph, output resource bind conflict!");
				const auto& storageBuffer = transientBuffers[output.index];

				if (storageBuffer.alias != "")
				{
					// Verify that the properties of the aliased resource match the properties of the current resource
					NIMBLE_ASSERT(bufferIndices.find(storageBuffer.alias) != bufferIndices.end());
					const auto aliasIndex = bufferIndices[storageBuffer.alias];
					const auto& aliasBuffer = transientBuffers[aliasIndex];
					NIMBLE_ASSERT(storageBuffer.count == aliasBuffer.count);
					NIMBLE_ASSERT(storageBuffer.stride == aliasBuffer.stride);
				}

				outputBindChecker[output.binding] = 1;
			}
		}
	}

	void RenderGraph::Compile()
	{
		NIMBLE_ASSERT(textureIndices.find("NIMBLE_BACKBUFFER") != textureIndices.end() && "Cannot compile render graph, back buffer not referenced in any pass!");
		NIMBLE_ASSERT(transientTextures[textureIndices["NIMBLE_BACKBUFFER"]].writers.size() == 1
			&& "Cannot compile render graph, only a single render pass is allowed to write to the back buffer!");

		// We flatten the render graph into an execution order
		// in the case of culling we can disregard passes that the
		// back buffer is not dependent on. Otherwise a simple
		// topological sorting works to sort the graph
		if (enablePassCulling)
		{
			activeExecutionOrder = FlattenAndCullPasses();
		}
		else
		{
			activeExecutionOrder = TopologicallySortPasses();
		}

		// Assemble execution contexts for the active render passes
		for (const auto i : activeExecutionOrder)
		{
			const auto& renderPass = passes[i];

			for (const auto output : renderPass.outputTextures)
			{
				ActualizeTexture(output.index, false);
			}

			for (const auto target : renderPass.targetTextures)
			{
				// Skip the creation of the back buffer as it is created by the swapchain
				if (transientTextures[target].identifier == "NIMBLE_BACKBUFFER") continue;
				ActualizeTexture(target, true);
			}

			for (const auto output : renderPass.outputStorageBuffers)
			{
				ActualizeBuffer(output.index);
			}
		}
	}

	void RenderGraph::DestroyResources()
	{
		for (auto& texture : transientTextures)
		{
			if (!texture.physicalTexture.handle.IsNullHandle())
			{
				resourceManager->DestroyTexture(texture.physicalTexture.handle);
			}
		}

		for (auto& buffer : transientBuffers)
		{
			if (!buffer.physicalBuffer.handle.IsNullHandle())
			{
				resourceManager->DestroyBuffer(buffer.physicalBuffer.handle);
			}
		}
	}

	void RenderGraph::Clear()
	{
		DestroyResources();
		passIndices.clear();
		passes.clear();
		bufferIndices.clear();
		textureIndices.clear();
		transientBuffers.clear();
		transientTextures.clear();
		activeExecutionOrder.clear();
	}

	eastl::vector<uint32_t> RenderGraph::FlattenAndCullPasses()
	{
		eastl::vector<uint32_t> executionOrder;
		auto backBufferIndex = textureIndices["NIMBLE_BACKBUFFER"];
		auto passIndex = transientTextures[backBufferIndex].writers[0];

		executionOrder.push_back(passIndex);
		TraversePass(passIndex, executionOrder);

		eastl::reverse(executionOrder.begin(), executionOrder.end());
		return executionOrder;
	}

	void RenderGraph::TraversePass(const uint32_t passIndex, eastl::vector<uint32_t>& executionOrder)
	{
		// Traverse the writers of the input resources of the current pass
		const auto& renderPass = passes[passIndex];

		uint32_t startIndex = static_cast<uint32_t>(executionOrder.size());

		for (const auto input : renderPass.inputTextures)
		{
			const auto& texture = transientTextures[input.index];
			for (const auto writer : texture.writers)
			{
				// Add the writer to the execution order if it isn't already
				if (eastl::find(executionOrder.cbegin(), executionOrder.cend(), writer) == executionOrder.end())
				{
					executionOrder.push_back(writer);
				}
			}
		}

		for (const auto input : renderPass.inputStorageBuffers)
		{
			const auto& buffer = transientBuffers[input.index];
			for (const auto writer : buffer.writers)
			{
				// Add the writer to the execution order if it isn't already
				if (eastl::find(executionOrder.cbegin(), executionOrder.cend(), writer) == executionOrder.end())
				{
					executionOrder.push_back(writer);
				}
			}
		}

		// Traverse the passes that were added
		uint32_t size = static_cast<uint32_t>(executionOrder.size());
		for (uint32_t i = startIndex; i < size; i++)
		{
			TraversePass(executionOrder[i], executionOrder);
		}
	}

	eastl::vector<uint32_t> RenderGraph::TopologicallySortPasses()
	{
		eastl::vector<uint32_t> executionOrder;

		// Gather indegrees and add render passes with indegree 0 to a queue
		eastl::vector<uint32_t> indegrees;
		eastl::vector<uint32_t> S;
		indegrees.resize(passIndices.size());
		for (const auto& [_, index] : passIndices)
		{
			auto inputCount = static_cast<uint32_t>(passes[index].inputTextures.size() + passes[index].inputStorageBuffers.size());
			indegrees[index] = inputCount;
			if (inputCount == 0)
			{
				S.push_back(index);
			}
		}

		auto visitedCount = 0;
		while (!S.empty())
		{
			const auto index = S.back();
			S.pop_back();
			visitedCount++;

			executionOrder.push_back(index);

			for (const auto output : passes[index].outputTextures)
			{
				for (const auto reader : transientTextures[output.index].readers)
				{
					indegrees[reader]--;
					if (indegrees[reader] == 0)
					{
						S.push_back(reader);
					}
				}
			}

			for (const auto output : passes[index].outputStorageBuffers)
			{
				for (const auto reader : transientBuffers[output.index].readers)
				{
					indegrees[reader]--;
					if (indegrees[reader] == 0)
					{
						S.push_back(reader);
					}
				}
			}

			for (const auto output : passes[index].targetTextures)
			{
				for (const auto reader : transientTextures[output].readers)
				{
					indegrees[reader]--;
					if (indegrees[reader] == 0)
					{
						S.push_back(reader);
					}
				}
			}
		}

		NIMBLE_ASSERT(visitedCount == passes.size() && "Cannot compile render graph, graph contains a dependency cycle!");
		return executionOrder;
	}

	void RenderGraph::ActualizeTexture(const uint32_t transientIndex, const bool isRenderTarget)
	{
		auto& texture = transientTextures[transientIndex];

		// If the transient resource has an alias, since input resources are actualized before their output
		// counterparts, the current resource can simply use the physical resource of the aliased resource
		if (texture.alias != "")
		{
			auto& alias = transientTextures[textureIndices[texture.alias]];
			NIMBLE_ASSERT(!alias.physicalTexture.handle.IsNullHandle() && "Alias resource has not be created!");
			texture.physicalTexture.handle = alias.physicalTexture.handle;
			return;
		}

		NIMBLE_ASSERT(!texture.readers.empty() || !texture.writers.empty() && "Cannot realize resource, its neither read from or written to!");


		NimbleFlags usage = 0;
		if (!texture.writers.empty())
		{
			usage |= TextureUsageFlags::TextureReadWrite;
		}
		if (!texture.readers.empty())
		{
			usage |= TextureUsageFlags::TextureReadOnly;
		}
		if (isRenderTarget)
		{
			usage |= TextureUsageFlags::TextureRenderTarget;
		}

		DirectX::XMUINT2 size = texture.size;
		if (texture.sizeMode == TextureSize::ScreenRelative)
		{
			size = { texture.size.x * graphicsContext->windowWidth, texture.size.y * graphicsContext->windowHeight };
		}

		texture.physicalTexture = resourceManager->CreateTexture(texture.format, usage, texture.shaderStageFlags, size, texture.levels, enableMSAA ? MSAA_SAMPLE_COUNT : 1);
	}

	void RenderGraph::ActualizeBuffer(const uint32_t transientIndex)
	{
		auto& buffer = transientBuffers[transientIndex];

		// If the transient resource has an alias, since input resources are actualized before their output
		// counterparts, the current resource can simply use the physical resource of the aliased resource
		if (buffer.alias != "")
		{
			auto& alias = transientBuffers[bufferIndices[buffer.alias]];
			NIMBLE_ASSERT(!alias.physicalBuffer.handle.IsNullHandle() && "Alias resource has not be created!");
			buffer.physicalBuffer.handle = alias.physicalBuffer.handle;
			return;
		}

		NIMBLE_ASSERT(!buffer.readers.empty() || !buffer.writers.empty() && "Cannot realize resource, its neither read from or written to!");
		
		NimbleFlags usage = 0;
		if (!buffer.writers.empty())
		{
			usage |= BufferUsageFlags::BufferReadWrite;
		}
		if (!buffer.readers.empty())
		{
			usage |= BufferUsageFlags::BufferReadOnly;
		}

		buffer.physicalBuffer = resourceManager->CreateStorageBuffer(usage, buffer.stride, buffer.count, nullptr, buffer.shaderStageFlags);
	}

	void RenderGraph::BindInputDescriptors(RenderPass& renderPass)
	{
		for (const auto input : renderPass.inputTextures)
		{
			const auto& texture = transientTextures[input.index];
			renderPass.commandBuffer.BindTexture(input.binding, texture.physicalTexture);
		}

		for (const auto input : renderPass.inputStorageBuffers)
		{
			const auto& buffer = transientBuffers[input.index];
			renderPass.commandBuffer.BindStorageBuffer(input.binding, buffer.physicalBuffer);
		}
	}

}
