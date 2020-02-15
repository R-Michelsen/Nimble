#include "NimblePCH.h"
#include "DXResourceManager.h"

#include "DXGraphicsPipeline.h"
#include "DXGraphicsContext.h"
#include "DXCommon.h"
#include "DXPipelinePresets.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"

namespace Nimble
{
	DXResourceManager::~DXResourceManager()
	{
		for (const auto& buffer : buffers.elements)
		{
			if (buffer.buf != nullptr) buffer.buf->Release();
			if (buffer.SRV != nullptr) buffer.SRV->Release();
			if (buffer.UAV != nullptr) buffer.UAV->Release();
		}

		for (const auto& texture : textures.elements)
		{
			if (texture.tex != nullptr) texture.tex->Release();
			if (texture.SRV != nullptr) texture.SRV->Release();
			if (texture.UAV != nullptr) texture.UAV->Release();
			if (texture.RTV != nullptr) texture.RTV->Release();
		}

		for (const auto& graphicsPipeline : graphicsPipelines.elements)
		{
			if (graphicsPipeline.vertexShader != nullptr) graphicsPipeline.vertexShader->Release();
			if (graphicsPipeline.pixelShader != nullptr) graphicsPipeline.pixelShader->Release();
			if (graphicsPipeline.inputLayout != nullptr) graphicsPipeline.inputLayout->Release();
			if (graphicsPipeline.depthStencilState != nullptr) graphicsPipeline.depthStencilState->Release();
		}
	}

	Nimble::Texture2D DXResourceManager::CreateTexture(const TextureFormat format, const NimbleFlags textureUsageFlags, NimbleFlags shaderStageFlags,
		const DirectX::XMUINT2 size, const uint32_t levels, const uint32_t samples)
	{
		auto handle = textures.GetNextHandle();
		textures[handle].shaderStage = static_cast<ShaderStageFlags>(shaderStageFlags);

		UINT bindFlags = 0;
		if (textureUsageFlags & TextureUsageFlags::TextureRenderTarget)
		{
			bindFlags |= D3D11_BIND_RENDER_TARGET;
		}
		if (textureUsageFlags & TextureUsageFlags::TextureReadWrite)
		{
			bindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		}
		if (textureUsageFlags & TextureUsageFlags::TextureReadOnly)
		{
			bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}

		D3D11_TEXTURE2D_DESC textureDescription{};
		textureDescription.Width = size.x;
		textureDescription.Height = size.y;
		textureDescription.MipLevels = levels;
		textureDescription.ArraySize = 1;
		textureDescription.Format = GetTextureFormat(format);
		textureDescription.SampleDesc.Count = samples;
		textureDescription.SampleDesc.Quality = 0;
		textureDescription.Usage = D3D11_USAGE_DEFAULT;
		textureDescription.BindFlags = bindFlags;
		textureDescription.CPUAccessFlags = 0;
		textureDescription.MiscFlags = 0;

		DXCHECK(graphicsDevice->CreateTexture2D(&textureDescription, nullptr, &textures[handle].tex));

		if (textureUsageFlags & TextureUsageFlags::TextureReadOnly)
		{
			DXCHECK(graphicsDevice->CreateShaderResourceView(textures[handle].tex, nullptr, &textures[handle].SRV));
		}
		if (textureUsageFlags & TextureUsageFlags::TextureReadWrite)
		{
			DXCHECK(graphicsDevice->CreateUnorderedAccessView(textures[handle].tex, nullptr, &textures[handle].UAV));
		}
		if (textureUsageFlags & TextureUsageFlags::TextureRenderTarget)
		{
			DXCHECK(graphicsDevice->CreateRenderTargetView(textures[handle].tex, nullptr, &textures[handle].RTV));
		}

		return Texture2D(handle);
	}

	Texture2D DXResourceManager::LoadTexture(const char* path, const NimbleFlags textureUsageFlags, NimbleFlags shaderStageFlags)
	{
		auto handle = textures.GetNextHandle();
		textures[handle].shaderStage = static_cast<ShaderStageFlags>(shaderStageFlags);

		int width, height, channels;
		stbi_uc* pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
		NIMBLE_ASSERT(pixels != nullptr);

		D3D11_TEXTURE2D_DESC textureDescription{};
		textureDescription.Width = width;
		textureDescription.Height = height;
		textureDescription.MipLevels = 1;
		textureDescription.ArraySize = 1;
		textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDescription.SampleDesc.Count = 1;
		textureDescription.SampleDesc.Quality = 0;
		textureDescription.Usage = (textureUsageFlags & TextureUsageFlags::TextureReadOnly) ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
		textureDescription.BindFlags = (textureUsageFlags & TextureUsageFlags::TextureReadOnly) ? D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_UNORDERED_ACCESS;
		textureDescription.CPUAccessFlags = 0;
		textureDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = pixels;
		data.SysMemPitch = width * sizeof(int);
		data.SysMemSlicePitch = 0;

		DXCHECK(graphicsDevice->CreateTexture2D(&textureDescription, &data, &textures[handle].tex));

		stbi_image_free(pixels);

		if (textureUsageFlags & TextureUsageFlags::TextureReadOnly)
		{
			DXCHECK(graphicsDevice->CreateShaderResourceView(textures[handle].tex, nullptr, &textures[handle].SRV));
		}
		if (textureUsageFlags & TextureUsageFlags::TextureReadWrite)
		{
			DXCHECK(graphicsDevice->CreateUnorderedAccessView(textures[handle].tex, nullptr, &textures[handle].UAV));
		}
		if (textureUsageFlags & TextureUsageFlags::TextureRenderTarget)
		{
			DXCHECK(graphicsDevice->CreateRenderTargetView(textures[handle].tex, nullptr, &textures[handle].RTV));
		}

		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		DXCHECK(graphicsDevice->CreateSamplerState(&samplerDesc, &textures[handle].SS));


		return Texture2D(handle);
	}

	// If data is nullptr the buffer will be created, but empty
	StorageBuffer DXResourceManager::CreateStorageBuffer(const NimbleFlags bufferUsageFlags, 
		const uint32_t stride, const uint32_t count, const void* data, NimbleFlags shaderStageFlags)
	{
		auto handle = buffers.GetNextHandle();
		buffers[handle].shaderStageFlags = static_cast<ShaderStageFlags>(shaderStageFlags);

		UINT bindFlags = 0;
		if (bufferUsageFlags & BufferUsageFlags::BufferReadOnly)
		{
			bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}
		if (bufferUsageFlags & BufferUsageFlags::BufferReadWrite)
		{
			bindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		}

		D3D11_BUFFER_DESC bufferDescription{};
		bufferDescription.ByteWidth = stride * count;
		bufferDescription.Usage = D3D11_USAGE_DEFAULT;
		bufferDescription.BindFlags = bindFlags;
		bufferDescription.CPUAccessFlags = 0;
		bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDescription.StructureByteStride = stride;

		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = data;
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		DXCHECK(graphicsDevice->CreateBuffer(&bufferDescription, (data != nullptr) ? &subresourceData : nullptr, &buffers[handle].buf));

		if (bufferUsageFlags & BufferUsageFlags::BufferReadOnly)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = count;

			DXCHECK(graphicsDevice->CreateShaderResourceView(buffers[handle].buf, &desc, &buffers[handle].SRV));
		}
		if (bufferUsageFlags & BufferUsageFlags::BufferReadWrite)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC desc{};
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = count;

			DXCHECK(graphicsDevice->CreateUnorderedAccessView(buffers[handle].buf, &desc, &buffers[handle].UAV));
		}
		
		return StorageBuffer(handle);
	}

	VertexBuffer DXResourceManager::CreateVertexBuffer(const void* vertices, const uint32_t count, const uint32_t stride)
	{
		auto handle = buffers.GetNextHandle();

		D3D11_BUFFER_DESC bufferDescription{};
		bufferDescription.ByteWidth = count * stride;
		bufferDescription.Usage = D3D11_USAGE_DEFAULT;
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.CPUAccessFlags = 0;
		bufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = vertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		DXCHECK(graphicsDevice->CreateBuffer(&bufferDescription, &data, &buffers[handle].buf));

		return VertexBuffer(handle);
	}

	IndexBuffer DXResourceManager::CreateIndexBuffer(const uint32_t* indices, const uint32_t count)
	{
		auto handle = buffers.GetNextHandle();

		D3D11_BUFFER_DESC bufferDescription{};
		bufferDescription.ByteWidth = count * sizeof(uint32_t);
		bufferDescription.Usage = D3D11_USAGE_DEFAULT;
		bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDescription.CPUAccessFlags = 0;
		bufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		DXCHECK(graphicsDevice->CreateBuffer(&bufferDescription, &data, &buffers[handle].buf));

		return IndexBuffer(handle);
	}

	ConstantBuffer DXResourceManager::CreateConstantBuffer(const uint32_t size, NimbleFlags shaderStageFlags, const void* data)
	{
		auto handle = buffers.GetNextHandle();
		buffers[handle].shaderStageFlags = static_cast<ShaderStageFlags>(shaderStageFlags);

		D3D11_BUFFER_DESC bufferDescription{};
		bufferDescription.ByteWidth = size;
		bufferDescription.Usage = D3D11_USAGE_DEFAULT;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = 0;
		bufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA resourceData{};
		if (data != nullptr)
		{
			resourceData.pSysMem = data;
		}

		DXCHECK(graphicsDevice->CreateBuffer(&bufferDescription, (data != nullptr) ? &resourceData : nullptr, &buffers[handle].buf));

		return ConstantBuffer(handle);
	}

	ConstantMappedBuffer DXResourceManager::CreateConstantMappedBuffer(const uint32_t size, NimbleFlags shaderStageFlags, const void* data)
	{
		auto handle = buffers.GetNextHandle();
		buffers[handle].shaderStageFlags = static_cast<ShaderStageFlags>(shaderStageFlags);

		D3D11_BUFFER_DESC bufferDescription{};
		bufferDescription.ByteWidth = size;
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.MiscFlags = 0;

		DXCHECK(graphicsDevice->CreateBuffer(&bufferDescription, nullptr, &buffers[handle].buf));

		return ConstantMappedBuffer(handle);
	}

	GraphicsPipeline DXResourceManager::CreateGraphicsPipeline(const char* vertexPath, const char* vertexEntry, const PipelineDescription& pipelineDescription)
	{
		return CreateGraphicsPipeline(vertexPath, vertexEntry, "", "", pipelineDescription);
	}

	GraphicsPipeline DXResourceManager::CreateGraphicsPipeline(const char* vertexPath, const char* vertexEntry, 
		const char* pixelPath, const char* pixelEntry, const PipelineDescription& pipelineDescription)
	{
		auto handle = graphicsPipelines.GetNextHandle();

		// Compile and create the vertex shader
		ID3DBlob* vertexBlob = CompileShader(vertexPath, vertexEntry, "vs_5_0");
		DXCHECK(graphicsDevice->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &graphicsPipelines[handle].vertexShader));

		// Create input element description for the vertex shader
		eastl::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescription;
		uint32_t posCount = 0;
		uint32_t colCount = 0;
		uint32_t normCount = 0;
		uint32_t uvCount = 0;
		for (const auto attrib : pipelineDescription.vertexInputAttributes)
		{
			inputElementDescription.push_back(GetInputElementDescription(attrib, posCount, colCount, normCount, uvCount));
		}
		DXCHECK(graphicsDevice->CreateInputLayout(inputElementDescription.data(), static_cast<uint32_t>(inputElementDescription.size()), 
			vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &graphicsPipelines[handle].inputLayout));
		vertexBlob->Release();

		// Compile and create the pixel shader if present
		if (strcmp(pixelPath, "") && strcmp(pixelEntry, ""))
		{
			ID3DBlob* pixelBlob = CompileShader(pixelPath, pixelEntry, "ps_5_0");
			DXCHECK(graphicsDevice->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), nullptr, &graphicsPipelines[handle].pixelShader));
			pixelBlob->Release();
		}

		// The pipeline keeps track of descriptor bindings to verify that 
		// future descriptor sets match the descriptor layout
		for (const auto& [type, binding] : pipelineDescription.descriptorLayout)
		{
			graphicsPipelines[handle].descriptorLayout[binding] = type;
		}

		// Create the depth stencil state
		if (pipelineDescription.depthStencilState == PipelineDepthState::PIPELINE_DEPTH_ENABLE)
		{
			DXCHECK(graphicsDevice->CreateDepthStencilState(&PIPELINE_DEPTHSTENCIL_STATE_ENABLE, &graphicsPipelines[handle].depthStencilState));
		}

		return GraphicsPipeline(handle);
	}

	PBRSceneComponent DXResourceManager::LoadModel(const char* path)
	{
		auto scene = PBRSceneComponent{};

		NIMBLE_ASSERT(strlen(path) < 256);
		char relativePath[256];
		_splitpath(path, nullptr, relativePath, nullptr, nullptr);

		cgltf_options options{};
		cgltf_data* data;
		auto result = cgltf_parse_file(&options, path, &data);
		NIMBLE_ASSERT(result == cgltf_result_success && "Failed parsing GLTF 2.0 model!");
		result = cgltf_load_buffers(&options, data, path);
		NIMBLE_ASSERT(result == cgltf_result_success && "Failed parsing GLTF 2.0 model!");

		eastl::vector<Material> materials;
		for (int i = 0; i < data->materials_count; i++)
		{
			const auto& material = data->materials[i];
			materials.push_back(ParseMaterial(material, relativePath));
		}

		eastl::vector<PBRVertex> vertices;
		eastl::vector<uint32_t> indices;

		for (int i = 0; i < data->nodes_count; i++)
		{
			const auto& node = data->nodes[i];
			ParseNode(*data, node, scene, vertices, indices, materials, DirectX::XMMatrixIdentity());
		}

		scene.vertexBuffer = CreateVertexBuffer(vertices.data(), static_cast<uint32_t>(vertices.size()), sizeof(PBRVertex));
		scene.indexBuffer = CreateIndexBuffer(indices.data(), static_cast<uint32_t>(indices.size()));

		cgltf_free(data);
		return scene;
	}

	DescriptorSetType DXResourceManager::CreateDescriptorSet(const eastl::vector<DescriptorType>& descriptors)
	{
		DescriptorSetType descriptorSet;
		for (const auto& descriptor : descriptors)
		{
			if (descriptor.type == ResourceType::Texture)
			{
				descriptorSet.textures.push_back({ *reinterpret_cast<Texture2D*>(descriptor.resource), descriptor.binding });
			}
			else if (descriptor.type == ResourceType::ConstantBuffer)
			{
				descriptorSet.constantBuffers.push_back({ *reinterpret_cast<ConstantBuffer*>(descriptor.resource), descriptor.binding });
			}
			else if (descriptor.type == ResourceType::StorageBuffer)
			{
				descriptorSet.storageBuffers.push_back({ *reinterpret_cast<StorageBuffer*>(descriptor.resource), descriptor.binding });
			}
		}
		return descriptorSet;
	}
	
	void DXResourceManager::DestroyTexture(ResourceHandle<TextureType> handle)
	{
		textures[handle].tex->Release();
		textures[handle] = TextureType();
		textures.DestroyResource(handle);
	}

	void DXResourceManager::DestroyBuffer(ResourceHandle<BufferType> handle)
	{
		buffers[handle].buf->Release();
		buffers[handle] = BufferType();
		buffers.DestroyResource(handle);
	}

	void DXResourceManager::DestroyGraphicsPipeline(ResourceHandle<GraphicsPipelineType> handle)
	{
		if (graphicsPipelines[handle].vertexShader != nullptr)
		{
			graphicsPipelines[handle].vertexShader->Release();
			graphicsPipelines[handle].vertexShader = nullptr;
		}
		if (graphicsPipelines[handle].pixelShader != nullptr)
		{
			graphicsPipelines[handle].pixelShader->Release();
			graphicsPipelines[handle].pixelShader = nullptr;
		}
		if (graphicsPipelines[handle].inputLayout != nullptr)
		{
			graphicsPipelines[handle].inputLayout->Release();
			graphicsPipelines[handle].inputLayout = nullptr;
		}
		if (graphicsPipelines[handle].depthStencilState != nullptr)
		{
			graphicsPipelines[handle].depthStencilState->Release();
			graphicsPipelines[handle].depthStencilState = nullptr;
		}
		graphicsPipelines.DestroyResource(handle);
	}
	
	ID3DBlob* DXResourceManager::CompileShader(const char* path, const char* entry, const char* profile)
	{
		uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		// Convert multi-byte string to unicode...
		size_t pathLength = strlen(path) + 1;
		eastl::vector<wchar_t> unicodePath;
		unicodePath.resize(pathLength);

		size_t outSize = 0;
		auto err = mbstowcs_s(&outSize, unicodePath.data(), pathLength, path, pathLength - 1);
		NIMBLE_ASSERT(err == 0 && "Error converting path to unicode!");

		D3DCompileFromFile(unicodePath.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, profile,
			flags, 0, &shaderBlob, &errorBlob);

		if (errorBlob != nullptr) errorBlob->Release();

		return shaderBlob;
	}

	Material DXResourceManager::ParseMaterial(const cgltf_material& material, const char* relativePath)
	{
		eastl::string absolutePath;

		Material mat{};
		NIMBLE_ASSERT(material.has_pbr_metallic_roughness && "At the moment Nimble only supports GLTF 2.0 with the PBR Metallic Workflow");
		if (material.pbr_metallic_roughness.base_color_texture.texture != nullptr)
		{
			const auto& texture = material.pbr_metallic_roughness.base_color_texture.texture;
			absolutePath = eastl::string(relativePath) + texture->image->uri;
			mat.baseColorTexture = LoadTexture(absolutePath.c_str(), TextureUsageFlags::TextureReadOnly, ShaderStageFlags::PixelStage);
			mat.textureIndices.baseColorTextureIndex = material.pbr_metallic_roughness.base_color_texture.texcoord;
		}
		if (material.pbr_metallic_roughness.metallic_roughness_texture.texture != nullptr)
		{
			const auto& texture = material.pbr_metallic_roughness.metallic_roughness_texture.texture;
			absolutePath = eastl::string(relativePath) + texture->image->uri;
			mat.metallicRoughnessTexture = LoadTexture(absolutePath.c_str(), TextureUsageFlags::TextureReadOnly, ShaderStageFlags::PixelStage);
			mat.textureIndices.metallicRoughnessTextureIndex = material.pbr_metallic_roughness.metallic_roughness_texture.texcoord;
		}
		if (material.normal_texture.texture != nullptr)
		{
			const auto& texture = material.normal_texture.texture;
			absolutePath = eastl::string(relativePath) + texture->image->uri;
			mat.normalTexture = LoadTexture(absolutePath.c_str(), TextureUsageFlags::TextureReadOnly, ShaderStageFlags::PixelStage);
			mat.textureIndices.normalTextureIndex = material.normal_texture.texcoord;
		}
		if (material.occlusion_texture.texture != nullptr)
		{
			const auto& texture = material.occlusion_texture.texture;
			absolutePath = eastl::string(relativePath) + texture->image->uri;
			mat.occlusionTexture = LoadTexture(absolutePath.c_str(), TextureUsageFlags::TextureReadOnly, ShaderStageFlags::PixelStage);
			mat.textureIndices.occlusionTextureIndex = material.occlusion_texture.texcoord;
		}
		if (material.emissive_texture.texture != nullptr)
		{
			const auto& texture = material.emissive_texture.texture;
			absolutePath = eastl::string(relativePath) + texture->image->uri;
			mat.emissiveTexture = LoadTexture(absolutePath.c_str(), TextureUsageFlags::TextureReadOnly, ShaderStageFlags::PixelStage);
			mat.textureIndices.emissiveTextureIndex = material.emissive_texture.texcoord;
		}
		mat.baseColorFactor = DirectX::XMFLOAT4(material.pbr_metallic_roughness.base_color_factor);
		mat.metallicFactor = material.pbr_metallic_roughness.metallic_factor;
		mat.roughnessFactor = material.pbr_metallic_roughness.roughness_factor;
		if (material.alpha_mode == cgltf_alpha_mode_mask)
		{
			mat.alphaMask = 1.0f;
			mat.alphaCutoff = material.alpha_cutoff;
		}

		MaterialConstants materialConstants{};
		materialConstants.alphaMask = mat.alphaMask;
		materialConstants.alphaMaskCutoff = mat.alphaCutoff;
		materialConstants.baseColorFactor = mat.baseColorFactor;
		materialConstants.emissiveFactor = mat.emissiveFactor;
		materialConstants.metallicFactor = mat.metallicFactor;
		materialConstants.roughnessFactor = mat.roughnessFactor;
		materialConstants.baseColorTextureIndex = mat.textureIndices.baseColorTextureIndex;
		materialConstants.emissiveTextureIndex = mat.textureIndices.emissiveTextureIndex;
		materialConstants.metallicRoughnessTextureIndex = mat.textureIndices.metallicRoughnessTextureIndex;
		materialConstants.normalTextureIndex = mat.textureIndices.normalTextureIndex;
		materialConstants.occlusionTextureIndex = mat.textureIndices.occlusionTextureIndex;
		
		mat.constantBuffer = CreateConstantBuffer(sizeof(MaterialConstants), ShaderStageFlags::PixelStage, &materialConstants);

		eastl::vector<DescriptorType> descriptors;
		descriptors.push_back({ &mat.constantBuffer,				ResourceType::ConstantBuffer, 1 });
		if (!mat.baseColorTexture.handle.IsNullHandle())
		{
			descriptors.push_back({ &mat.baseColorTexture,			ResourceType::Texture, 1 });
		}
		if (!mat.metallicRoughnessTexture.handle.IsNullHandle())
		{
			descriptors.push_back({ &mat.metallicRoughnessTexture,	ResourceType::Texture, 2 });
		}
		if (!mat.normalTexture.handle.IsNullHandle())
		{
			descriptors.push_back({ &mat.normalTexture,				ResourceType::Texture, 3 });
		}		
		if (!mat.occlusionTexture.handle.IsNullHandle())
		{
			descriptors.push_back({ &mat.occlusionTexture,			ResourceType::Texture, 4 });
		}
		if (!mat.emissiveTexture.handle.IsNullHandle())
		{
			descriptors.push_back({ &mat.emissiveTexture,			ResourceType::Texture, 5 });
		}

		mat.descriptorSet = CreateDescriptorSet(descriptors);
		return mat;
	}

	void DXResourceManager::ParseNode(const cgltf_data& data, const cgltf_node& node, PBRSceneComponent& scene, eastl::vector<PBRVertex>& vertices, 
		eastl::vector<uint32_t>& indices, const eastl::vector<Material>& materials, DirectX::XMMATRIX parentTransform)
	{
		const auto& ApplyNodeTransform = [](DirectX::XMMATRIX& currentTransform, const cgltf_node& targetNode)
		{
			if (targetNode.has_translation)
			{
				auto translation = DirectX::XMFLOAT3(targetNode.translation);
				currentTransform *= DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&translation));
			}
			if (targetNode.has_rotation)
			{
				auto rotation = DirectX::XMFLOAT4(targetNode.rotation);
				currentTransform *= DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation));
			}
			if (targetNode.has_scale)
			{
				auto scale = DirectX::XMFLOAT3(targetNode.scale);
				currentTransform *= DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale));
			}
			if (targetNode.has_matrix)
			{
				auto nodeTransform = DirectX::XMFLOAT4X4(targetNode.matrix);
				currentTransform *= DirectX::XMLoadFloat4x4(&nodeTransform);
			}
		};

		// Construct transform matrix from node data and parent data
		auto transform = DirectX::XMMatrixIdentity();
		ApplyNodeTransform(transform, node);

		cgltf_node* currentNode = node.parent;
		while(currentNode != nullptr)
		{
			ApplyNodeTransform(transform, *currentNode);
			currentNode = currentNode->parent;
		}

		if (node.mesh != nullptr)
		{
			Mesh mesh = ParseMesh(data, *node.mesh, vertices, indices, materials);
			mesh.localTransform = transform;
			mesh.constantBuffer = CreateConstantBuffer(sizeof(MeshConstants), ShaderStageFlags::VertexStage);

			mesh.descriptorSet = CreateDescriptorSet({ { &mesh.constantBuffer, ResourceType::ConstantBuffer, 0} });
			scene.meshes.push_back(mesh);
		}
	}

	Mesh DXResourceManager::ParseMesh(const cgltf_data& data, const cgltf_mesh& mesh, eastl::vector<PBRVertex>& vertices, 
		eastl::vector<uint32_t>& indices, const eastl::vector<Material>& materials)
	{
		Mesh parsedMesh{};

		for (int i = 0; i < mesh.primitives_count; i++)
		{
			const cgltf_primitive& primitive = mesh.primitives[i];

			size_t vertexCount = 0;

			const uint32_t vertexOffset = static_cast<uint32_t>(vertices.size());
			const uint32_t indexOffset = static_cast<uint32_t>(indices.size());

			size_t positionStride = 0;
			size_t normalStride = 0;
			size_t uv0Stride = 0;
			size_t uv1Stride = 0;
			const float* positions = nullptr;
			const float* normals = nullptr;
			const float* uvs0 = nullptr;
			const float* uvs1 = nullptr;

			for (int j = 0; j < primitive.attributes_count; j++)
			{
				const cgltf_attribute& attribute = primitive.attributes[j];
				
				if (attribute.type == cgltf_attribute_type_position)
				{
					const char* buffer = reinterpret_cast<const char*>(attribute.data->buffer_view->buffer->data);
					positions = reinterpret_cast<const float*>(&buffer[attribute.data->offset + attribute.data->buffer_view->offset]);
					positionStride = attribute.data->stride / sizeof(float);
					NIMBLE_ASSERT(positions != nullptr && positionStride > 0);
					vertexCount = attribute.data->count;
				}
				else if (attribute.type == cgltf_attribute_type_normal)
				{
					const char* buffer = reinterpret_cast<const char*>(attribute.data->buffer_view->buffer->data);
					normals = reinterpret_cast<const float*>(&buffer[attribute.data->offset + attribute.data->buffer_view->offset]);
					normalStride = attribute.data->stride / sizeof(float);
					NIMBLE_ASSERT(normals != nullptr && normalStride > 0);
				}
				else if (attribute.type == cgltf_attribute_type_texcoord)
				{
					if (!strcmp(attribute.name, "TEXCOORD_0"))
					{
						const char* buffer = reinterpret_cast<const char*>(attribute.data->buffer_view->buffer->data);
						uvs0 = reinterpret_cast<const float*>(&buffer[attribute.data->offset + attribute.data->buffer_view->offset]);
						uv0Stride = attribute.data->stride / sizeof(float);
						NIMBLE_ASSERT(uvs0 != nullptr && uv0Stride > 0);
					}
					else if (!strcmp(attribute.name, "TEXCOORD_1"))
					{
						const char* buffer = reinterpret_cast<const char*>(attribute.data->buffer_view->buffer->data);
						uvs1 = reinterpret_cast<const float*>(&buffer[attribute.data->offset + attribute.data->buffer_view->offset]);
						uv1Stride = attribute.data->stride / sizeof(float);
						NIMBLE_ASSERT(uvs1 != nullptr && uv1Stride > 0);
					}
				}
			}

			for (int j = 0; j < vertexCount; j++)
			{
				PBRVertex vertex{};
				vertex.position = DirectX::XMFLOAT3(&positions[j * positionStride]);
				if (normals != nullptr)
				{
					DirectX::XMFLOAT3 rawN = DirectX::XMFLOAT3(&normals[j * normalStride]);
					auto normalizedN = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&rawN));
					DirectX::XMStoreFloat3(&vertex.normal, normalizedN);
				}
				if (uvs0 != nullptr)
				{
					vertex.uv0 = DirectX::XMFLOAT2(&uvs0[j * uv0Stride]);
				}
				if (uvs1 != nullptr)
				{
					vertex.uv1 = DirectX::XMFLOAT2(&uvs1[j * uv1Stride]);
				}
				vertices.push_back(vertex);
			}

			// TODO: Fallback to draw without indexes if not available
			NIMBLE_ASSERT(primitive.indices->count > 0);
			NIMBLE_ASSERT(primitive.indices->component_type == cgltf_component_type_r_8u ||
				primitive.indices->component_type == cgltf_component_type_r_16u ||
				primitive.indices->component_type == cgltf_component_type_r_32u && 
				"Indexbuffer type not supported, supported types are uint8, uint16, uint32");

			const char* buffer = reinterpret_cast<const char*>(primitive.indices->buffer_view->buffer->data);
			const void* indexBuffer = &buffer[primitive.indices->offset + primitive.indices->buffer_view->offset];

			if (primitive.indices->component_type == cgltf_component_type_r_8u)
			{
				const uint8_t* buf = static_cast<const uint8_t*>(indexBuffer);
				for (auto j = 0; j < primitive.indices->count; j++)
				{
					indices.push_back(buf[j]);
				}
			}
			else if (primitive.indices->component_type == cgltf_component_type_r_16u)
			{
				const uint16_t* buf = static_cast<const uint16_t*>(indexBuffer);
				for (auto j = 0; j < primitive.indices->count; j++)
				{
					indices.push_back(buf[j]);
				}
			}
			else if (primitive.indices->component_type == cgltf_component_type_r_32u)
			{
				const uint32_t* buf = static_cast<const uint32_t*>(indexBuffer);
				for (auto j = 0; j < primitive.indices->count; j++)
				{
					indices.push_back(buf[j]);
				}
			}

			MeshPrimitive meshPrimitive;
			meshPrimitive.vertexOffset = vertexOffset;
			meshPrimitive.indexOffset = indexOffset;
			meshPrimitive.indexCount = static_cast<uint32_t>(indices.size()) - indexOffset;
			for (int j = 0; j < data.materials_count; j++)
			{
				if (&data.materials[j] == primitive.material)
				{
					meshPrimitive.material = materials[j];
					break;
				}
			}
			parsedMesh.primitives.push_back(meshPrimitive);
		}

		return parsedMesh;
	}

}
