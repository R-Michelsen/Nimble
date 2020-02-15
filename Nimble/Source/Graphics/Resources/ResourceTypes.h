#pragma once

#include "ResourceHandle.h"
#include "../BackendTypedefs.h"
#include "../Enums.h"

namespace Nimble
{
	template<typename T>
	struct Resource
	{
	public:
		Resource() = default;
		Resource(ResourceHandle<T> handle_) : handle(handle_) {}

		ResourceHandle<T> handle{};
	};

	struct Texture2D : public Resource<TextureType>
	{
		using Resource::Resource;
	};

	struct VertexBuffer : public Resource<BufferType>
	{
		using Resource::Resource;
	};

	struct IndexBuffer : public Resource<BufferType>
	{
		using Resource::Resource;
	};

	struct ConstantBuffer : public Resource<BufferType>
	{
		using Resource::Resource;
	};

	struct ConstantMappedBuffer : public Resource<BufferType>
	{
		using Resource::Resource;

	};

	struct StorageBuffer : public Resource<BufferType>
	{
		using Resource::Resource;
	};

	struct GraphicsPipeline : public Resource<GraphicsPipelineType>
	{
		using Resource::Resource;
	};
}