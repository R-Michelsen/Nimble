#pragma once

#include <EASTL/fixed_vector.h>

#include <utility>
#include <d3d11_3.h>

#include "Enums.h"

namespace Nimble
{
	using NimbleFlags = uint32_t;

	// Graphics backend compile-time flag, defaults to DX11
#ifdef NIMBLE_VULKAN
	class VkGraphicsContext;
	using GraphicsContextType = VkGraphicsContext;

	class VkResourceManager;
	using ResourceManagerType = VkResourceManager;

	class VkRenderPass;
	using RenderPassType = VkRenderPass;

	using GraphicsDeviceType = VkDevice;

	using TextureType = VkTexture2D;
	using BufferType = VkBuffer;
	using VertexShaderType = VkShaderModule;
	using PixelShaderType = VkShaderModule;
	using ComputeShaderType = VkShaderModule;

	constexpr size_t MAX_VERTEX_INPUT_ATTRIBUTES = 0;

#else
	class DXGraphicsContext;
	using GraphicsContextType = DXGraphicsContext;

	class DXResourceManager;
	using ResourceManagerType = DXResourceManager;

	class DXCommandBuffer;
	using CommandBufferType = DXCommandBuffer;

	struct DXGraphicsPipeline;
	using GraphicsPipelineType = DXGraphicsPipeline;

	struct DXDescriptorSet;
	using DescriptorSetType = DXDescriptorSet;

	struct DXDescriptor;
	using DescriptorType = DXDescriptor;

	struct DXComputePipeline;
	using ComputePipelineType = DXComputePipeline;

	struct DXTexture2D;
	using TextureType = DXTexture2D;

	struct DXBuffer;
	using BufferType = DXBuffer;

	using GraphicsDeviceType = ID3D11Device1*;

	constexpr size_t MAX_VERTEX_INPUT_ATTRIBUTES = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
#endif

	struct ResourceBinding
	{
		ResourceType type;
		uint32_t binding;
	};

	struct PipelineDescription
	{
		PipelineDepthState depthStencilState = PipelineDepthState::PIPELINE_DEPTH_ENABLE;
		PipelineRasterizationState rasterizationState = PipelineRasterizationState::PIPELINE_RASTERIZATION_DEFAULT;
		PipelineColourBlendingState colourBlendingState = PipelineColourBlendingState::COLOURBLENDING_DISABLE;
		PipelineDynamicState dynamicState = PipelineDynamicState::DYNAMICSTATE_NONE;

		eastl::fixed_vector<VertexInputAttribute, MAX_VERTEX_INPUT_ATTRIBUTES> vertexInputAttributes;
		eastl::vector<ResourceBinding> descriptorLayout;
	};
}