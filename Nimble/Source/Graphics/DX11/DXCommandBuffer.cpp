#include "NimblePCH.h"
#include "DXCommandBuffer.h"

#include "DXResourceManager.h"
#include "DXGraphicsContext.h"
#include "DXGraphicsPipeline.h"
#include "DXDescriptorSet.h"
#include "../RenderGraph/RenderPass.h"
#include "../RenderGraph/TransientResource.h"

constexpr void* const NULLPTR_ARRAY[128] = {};
namespace Nimble
{
	DXCommandBuffer::DXCommandBuffer(GraphicsContextType* graphicsContext_, ResourceManagerType* const resourceManager_) :
		graphicsContext(graphicsContext_),
		resourceManager(resourceManager_)
	{
		defaultSamplerState = graphicsContext_->GetDefaultSamplerState();
		//graphicsContext->GetDevice()->CreateDeferredContext1(0, &deferredContext);
		deferredContext = graphicsContext->GetImmediateContext();
	}

	void DXCommandBuffer::UnbindResources(const uint32_t numRenderTargets, const uint32_t numReadResources)
	{
		deferredContext->OMSetRenderTargetsAndUnorderedAccessViews(
			numRenderTargets, (ID3D11RenderTargetView**)NULLPTR_ARRAY, nullptr,
			numRenderTargets, D3D11_1_UAV_SLOT_COUNT - numRenderTargets, (ID3D11UnorderedAccessView**)NULLPTR_ARRAY, nullptr);

		if (numReadResources > 0)
		{
			deferredContext->VSSetShaderResources(0, numReadResources, (ID3D11ShaderResourceView**)NULLPTR_ARRAY);
			deferredContext->PSSetShaderResources(0, numReadResources, (ID3D11ShaderResourceView**)NULLPTR_ARRAY);
			deferredContext->CSSetShaderResources(0, numReadResources, (ID3D11ShaderResourceView**)NULLPTR_ARRAY);

			deferredContext->GSSetShaderResources(0, numReadResources, (ID3D11ShaderResourceView**)NULLPTR_ARRAY);
			deferredContext->DSSetShaderResources(0, numReadResources, (ID3D11ShaderResourceView**)NULLPTR_ARRAY);
			deferredContext->HSSetShaderResources(0, numReadResources, (ID3D11ShaderResourceView**)NULLPTR_ARRAY);
		}

		rtvs.clear();
		for (uint32_t i = 0; i < numReadResources; i++)
		{
			uavs[i] = nullptr;
		}
	}

	void DXCommandBuffer::SetDefaultViewport()
	{
		D3D11_VIEWPORT viewport{ 0 };
		viewport.Width = static_cast<float>(graphicsContext->windowWidth);
		viewport.Height = static_cast<float>(graphicsContext->windowHeight);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		deferredContext->RSSetViewports(1, &viewport);
	}

	void DXCommandBuffer::SetViewport(const uint32_t width, const uint32_t height)
	{
		D3D11_VIEWPORT viewport{ 0 };
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		deferredContext->RSSetViewports(1, &viewport);
	}

	void DXCommandBuffer::SetTopology()
	{
		deferredContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void DXCommandBuffer::ClearView(Texture2D view)
	{
		if (resourceManager->textures[view.handle].SRV != nullptr)
		{
			deferredContext->ClearView(resourceManager->textures[view.handle].SRV, DirectX::Colors::MidnightBlue, nullptr, 0);
		}
		if (resourceManager->textures[view.handle].UAV != nullptr)
		{
			deferredContext->ClearView(resourceManager->textures[view.handle].UAV, DirectX::Colors::MidnightBlue, nullptr, 0);
		}
		if (resourceManager->textures[view.handle].RTV != nullptr)
		{
			deferredContext->ClearRenderTargetView(resourceManager->textures[view.handle].RTV, DirectX::Colors::MidnightBlue);
		}
	}

	void DXCommandBuffer::ClearDefaultViews()
	{
		auto* rtv = graphicsContext->GetMainRTV();
		auto* dsv = graphicsContext->GetMainDSV();
		deferredContext->ClearRenderTargetView(rtv, DirectX::Colors::MidnightBlue);
		deferredContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void DXCommandBuffer::BindOutputDescriptorsAndRenderTargets(RenderPass& renderPass,
		const eastl::vector<TransientTexture>& transientTextures, const eastl::vector<TransientStorageBuffer>& transientBuffers)
	{
		NIMBLE_ASSERT(renderPass.targetTextures.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT && "Too many render targets in render pass!");
		for (const auto target : renderPass.targetTextures)
		{
			if (transientTextures[target].identifier == "NIMBLE_BACKBUFFER")
			{
				rtvs.push_back(graphicsContext->GetMainRTV());
				continue;
			}
			const auto& texture = transientTextures[target].physicalTexture;
			NIMBLE_ASSERT(resourceManager->textures[texture.handle].RTV != nullptr);
			rtvs.push_back(resourceManager->textures[texture.handle].RTV);
		}

		for (const auto output : renderPass.outputTextures)
		{
			NIMBLE_ASSERT(output.binding >= static_cast<uint32_t>(rtvs.size()) && "output resource binding conflicts with a render target binding!");
			NIMBLE_ASSERT(output.binding < D3D11_1_UAV_SLOT_COUNT && "output resource binding out of bounds");

			const auto& texture = transientTextures[output.index].physicalTexture;
			NIMBLE_ASSERT(resourceManager->textures[texture.handle].UAV != nullptr);
			uavs[output.binding] = resourceManager->textures[texture.handle].UAV;
		}

		for (const auto output : renderPass.outputStorageBuffers)
		{
			NIMBLE_ASSERT(output.binding >= static_cast<uint32_t>(rtvs.size()) && "output resource binding conflicts with a render target binding!");
			NIMBLE_ASSERT(output.binding < D3D11_1_UAV_SLOT_COUNT && "output resource binding out of bounds");

			const auto& buffer = transientBuffers[output.index].physicalBuffer;
			NIMBLE_ASSERT(resourceManager->buffers[buffer.handle].UAV != nullptr);
			uavs[output.binding] = resourceManager->buffers[buffer.handle].UAV;
		}

		ID3D11DepthStencilView* dsv = graphicsContext->GetMainDSV();

		uint32_t renderTargetCount = static_cast<uint32_t>(rtvs.size());
		deferredContext->OMSetRenderTargetsAndUnorderedAccessViews(renderTargetCount,
			rtvs.data(), dsv, renderTargetCount, D3D11_1_UAV_SLOT_COUNT - renderTargetCount, &uavs[renderTargetCount], nullptr);
	}

	void DXCommandBuffer::BindVertexBuffer(const VertexBuffer buffer, const uint32_t stride, const uint32_t offset)
	{
		deferredContext->IASetVertexBuffers(0, 1, &resourceManager->buffers[buffer.handle].buf, &stride, &offset);
	}

	void DXCommandBuffer::BindIndexBuffer(const IndexBuffer buffer, const uint32_t offset)
	{
		deferredContext->IASetIndexBuffer(resourceManager->buffers[buffer.handle].buf, DXGI_FORMAT_R32_UINT, offset);
	}

	void DXCommandBuffer::BindPipeline(const GraphicsPipeline pipeline)
	{
		const auto& pipelineResource = resourceManager->graphicsPipelines[pipeline.handle];

		// Set pipeline states
		deferredContext->OMSetDepthStencilState(pipelineResource.depthStencilState, 1);

		deferredContext->IASetInputLayout(pipelineResource.inputLayout);
		deferredContext->VSSetShader(pipelineResource.vertexShader, nullptr, 0);
		if (pipelineResource.pixelShader != nullptr) deferredContext->PSSetShader(pipelineResource.pixelShader, nullptr, 0);

	}

	void DXCommandBuffer::BindDescriptorSet(const DescriptorSetType descriptorSet)
	{
		for (const auto& [texture, binding] : descriptorSet.textures)
		{
			BindTexture(binding, texture);
		}

		for (const auto& [constantBuffer, binding] : descriptorSet.constantBuffers)
		{
			BindConstantBuffer(binding, constantBuffer);
		}

		for (const auto& [storageBuffer, binding] : descriptorSet.storageBuffers)
		{
			BindStorageBuffer(binding, storageBuffer);
		}
	}

	void DXCommandBuffer::BindStorageBuffer(const uint32_t binding, const StorageBuffer buffer)
	{
		const auto& bufferResource = resourceManager->buffers[buffer.handle];

		if (bufferResource.shaderStageFlags & ShaderStageFlags::VertexStage)
		{
			deferredContext->VSSetShaderResources(binding, 1, &bufferResource.SRV);
		}
		if (bufferResource.shaderStageFlags & ShaderStageFlags::PixelStage)
		{
			deferredContext->PSSetShaderResources(binding, 1, &bufferResource.SRV);
		}
		if (bufferResource.shaderStageFlags & ShaderStageFlags::ComputeStage)
		{
			deferredContext->CSSetShaderResources(binding, 1, &bufferResource.SRV);
		}
	}

	void DXCommandBuffer::BindTexture(const uint32_t binding, const Texture2D texture)
	{
		const auto& textureResource = resourceManager->textures[texture.handle];

		ID3D11SamplerState* samplerState = (textureResource.SS != nullptr) ? textureResource.SS : defaultSamplerState;

		if (textureResource.shaderStage & ShaderStageFlags::VertexStage)
		{
			deferredContext->VSSetShaderResources(binding, 1, &textureResource.SRV);
			deferredContext->VSSetSamplers(binding, 1, &samplerState);
		}
		if (textureResource.shaderStage & ShaderStageFlags::PixelStage)
		{
			deferredContext->PSSetShaderResources(binding, 1, &textureResource.SRV);
			deferredContext->PSSetSamplers(binding, 1, &samplerState);
		}
		if (textureResource.shaderStage & ShaderStageFlags::ComputeStage)
		{
			deferredContext->CSSetShaderResources(binding, 1, &textureResource.SRV);
			deferredContext->CSSetSamplers(binding, 1, &samplerState);
		}
	}

	void DXCommandBuffer::BindTextures(const uint32_t startBinding, const eastl::vector<Texture2D>& textures)
	{
		for (auto i = 0; i < textures.size(); i++)
		{
			BindTexture(startBinding + i, textures[i]);
		}
	}

	void DXCommandBuffer::UpdateConstantBuffer(const ConstantBuffer buffer, const void* data)
	{
		deferredContext->UpdateSubresource1(resourceManager->buffers[buffer.handle].buf, 0, nullptr, data, 0, 0, D3D11_COPY_DISCARD);
	}

	void DXCommandBuffer::UpdateConstantBuffer(const ConstantMappedBuffer buffer, const void* data, const uint32_t size)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource{};

		// Disable GPU access
		deferredContext->Map(resourceManager->buffers[buffer.handle].buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		// Copy new data
		memcpy(mappedResource.pData, data, size);

		// Reenable GPU access
		deferredContext->Unmap(resourceManager->buffers[buffer.handle].buf, 0);
	}

	void DXCommandBuffer::Draw(const uint32_t vertexCount, const uint32_t offset)
	{
		deferredContext->Draw(vertexCount, offset);
	}

	void DXCommandBuffer::DrawIndexed(const uint32_t indexCount, const uint32_t offset)
	{
		deferredContext->DrawIndexed(indexCount, offset, 0);
	}

	void DXCommandBuffer::Flush()
	{
		deferredContext->FinishCommandList(true, &commandList);
	}

}
