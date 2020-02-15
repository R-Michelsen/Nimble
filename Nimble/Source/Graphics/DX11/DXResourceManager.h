#pragma once

#include <tuple>

#include "../BackendTypedefs.h"
#include "../Enums.h"
#include "../Resources/ResourceHandle.h"
#include "../Resources/ResourceArray.h"
#include "../Resources/ResourceTypes.h"
#include "../RenderTypes.h"
#include "../../Math/MathCommon.h"
#include "DXBuffer.h"
#include "DXTexture2D.h"
#include "DXGraphicsPipeline.h"
#include "DXDescriptorSet.h"

struct cgltf_data;
struct cgltf_node;
struct cgltf_material;
struct cgltf_mesh;

namespace Nimble
{
	constexpr size_t MAX_TEXTURE_COUNT = 4096;
	constexpr size_t MAX_BUFFER_COUNT = 4096;
	constexpr size_t MAX_GRAPHICS_PIPELINE_COUNT = 128;

	class DXResourceManager
	{
	public:
		ResourceArray<TextureType, MAX_TEXTURE_COUNT> textures;
		ResourceArray<BufferType, MAX_BUFFER_COUNT> buffers;

		ResourceArray<GraphicsPipelineType, MAX_GRAPHICS_PIPELINE_COUNT> graphicsPipelines;

		DXResourceManager(GraphicsDeviceType graphicsDevice) : graphicsDevice(graphicsDevice) {}
		~DXResourceManager();

		[[nodiscard]] Texture2D CreateTexture(const TextureFormat format, const NimbleFlags textureUsageFlags, NimbleFlags shaderStageFlags,
			const DirectX::XMUINT2 size, const uint32_t levels, const uint32_t samples);
		[[nodiscard]] Texture2D LoadTexture(const char* path, const NimbleFlags textureUsageFlags, NimbleFlags shaderStageFlags);

		[[nodiscard]] StorageBuffer CreateStorageBuffer(const NimbleFlags bufferUsageFlags, const uint32_t stride, 
			const uint32_t count, const void* data, NimbleFlags shaderStageFlags);

		[[nodiscard]] VertexBuffer CreateVertexBuffer(const void* vertices, const uint32_t count, const uint32_t stride);
		[[nodiscard]] IndexBuffer CreateIndexBuffer(const uint32_t* indices, const uint32_t count);
		[[nodiscard]] ConstantBuffer CreateConstantBuffer(const uint32_t size, NimbleFlags shaderStageFlags, const void* data = nullptr);
		[[nodiscard]] ConstantMappedBuffer CreateConstantMappedBuffer(const uint32_t size, NimbleFlags shaderStageFlags, const void* data = nullptr);

		[[nodiscard]] GraphicsPipeline CreateGraphicsPipeline(const char* vertexPath, const char* vertexEntry, const PipelineDescription& pipelineDescription);
		[[nodiscard]] GraphicsPipeline CreateGraphicsPipeline(const char* vertexPath, const char* vertexEntry, const char* pixelPath, const char* pixelEntry,
			const PipelineDescription& pipelineDescription);

		[[nodiscard]] PBRSceneComponent LoadModel(const char* path);

		DescriptorSetType CreateDescriptorSet(const eastl::vector<DescriptorType>& descriptors);

		void DestroyTexture(ResourceHandle<TextureType> handle);
		void DestroyBuffer(ResourceHandle<BufferType> handle);
		void DestroyGraphicsPipeline(ResourceHandle<GraphicsPipelineType> handle);

	private:
		GraphicsDeviceType graphicsDevice;

		ID3DBlob* CompileShader(const char* path, const char* entry, const char* profile);

		Material ParseMaterial(const cgltf_material& material, const char* relativePath);
		void ParseNode(const cgltf_data& data, const cgltf_node& node, PBRSceneComponent& scene, eastl::vector<PBRVertex>& vertices, 
			eastl::vector<uint32_t>& indices, const eastl::vector<Material>& materials, DirectX::XMMATRIX parentTransform);
		Mesh ParseMesh(const cgltf_data& data, const cgltf_mesh& mesh, eastl::vector<PBRVertex>& vertices,
			eastl::vector<uint32_t>& indices, const eastl::vector<Material>& materials);
	};
}

