#pragma once

#include <EASTL/type_traits.h>
#include <EASTL/vector.h>

#include <d3d11_3.h>

#include "../BackendTypedefs.h"
#include "../Resources/ResourceTypes.h"
#include "DXResourceManager.h"

namespace Nimble
{
	struct RenderPass;
	struct TransientTexture;
	struct TransientStorageBuffer;

	class DXCommandBuffer
	{
	public:
		DXCommandBuffer(GraphicsContextType* graphicsContext_, ResourceManagerType* const resourceManager_);

		void UnbindResources(const uint32_t numRenderTargets, const uint32_t numReadResources);

		void SetDefaultViewport();
		void SetViewport(const uint32_t width, const uint32_t height);

		void SetTopology();

		void ClearView(Texture2D view);
		void ClearDefaultViews();

		void BindOutputDescriptorsAndRenderTargets(RenderPass& renderPass, 
			const eastl::vector<TransientTexture>& transientTextures, const eastl::vector<TransientStorageBuffer>& transientBuffers);

		void BindVertexBuffer(const VertexBuffer buffer, const uint32_t stride, const uint32_t offset);
		void BindIndexBuffer(const IndexBuffer buffer, const uint32_t offset);

		void BindPipeline(const GraphicsPipeline pipeline);
		void BindDescriptorSet(const DescriptorSetType descriptorSet);

		template<typename T>
		inline void BindConstantBuffer(const uint32_t binding, const T buffer)
		{
			static_assert(eastl::is_same<T, ConstantBuffer>::value || eastl::is_same<T, ConstantMappedBuffer>::value);

			const auto& bufferResource = resourceManager->buffers[buffer.handle];

			if (bufferResource.shaderStageFlags & ShaderStageFlags::VertexStage)
			{
				deferredContext->VSSetConstantBuffers(binding, 1, &bufferResource.buf);
			}
			if (bufferResource.shaderStageFlags & ShaderStageFlags::PixelStage)
			{
				deferredContext->PSSetConstantBuffers(binding, 1, &bufferResource.buf);
			}
			if (bufferResource.shaderStageFlags & ShaderStageFlags::ComputeStage)
			{
				deferredContext->CSSetConstantBuffers(binding, 1, &bufferResource.buf);
			}
		}

		void BindStorageBuffer(const uint32_t binding, const StorageBuffer buffer);

		void BindTexture(const uint32_t binding, const Texture2D texture);
		void BindTextures(const uint32_t startBinding, const eastl::vector<Texture2D>& textures);

		void UpdateConstantBuffer(const ConstantBuffer buffer, const void* data);
		void UpdateConstantBuffer(const ConstantMappedBuffer buffer, const void* data, const uint32_t size);

		void Draw(const uint32_t vertexCount, const uint32_t offset);
		void DrawIndexed(const uint32_t indexCount, const uint32_t offset);

		void Flush();

	private:
		friend class DXGraphicsContext;
		GraphicsContextType* graphicsContext;
		ResourceManagerType* const resourceManager;

		ID3D11DeviceContext1* deferredContext = nullptr;
		ID3D11CommandList* commandList = nullptr;
		ID3D11InputLayout* inputLayout = nullptr;
		ID3D11SamplerState* defaultSamplerState = nullptr;

		eastl::fixed_vector<ID3D11RenderTargetView*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> rtvs;
		eastl::array<ID3D11UnorderedAccessView*, D3D11_1_UAV_SLOT_COUNT> uavs{};
	};
}
