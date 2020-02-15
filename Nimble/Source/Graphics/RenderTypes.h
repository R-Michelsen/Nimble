#pragma once

#include <DirectXMath.h>

#include "BackendTypedefs.h"
#include "Resources/ResourceTypes.h"
#include "DX11/DXDescriptorSet.h"

namespace Nimble
{

	struct alignas(16) RenderCamera
	{
		DirectX::XMFLOAT3A position;
		DirectX::XMFLOAT3A front;
		DirectX::XMFLOAT3A up;
		float aspectRatio;
		float viewDistance;

		float yaw;
		float pitch;

		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;

		bool dragActive;

		RenderCamera(DirectX::XMFLOAT3A position_, float aspectRatio_, float viewDistance_, float yaw_, float pitch_) :
			position(position_),
			front(DirectX::XMFLOAT3A()),
			up({ 0.0f, 1.0f, 0.0 }),
			aspectRatio(aspectRatio_),
			viewDistance(viewDistance_),
			yaw(yaw_),
			pitch(pitch_),
			view(DirectX::XMMATRIX()),
			projection(DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), aspectRatio, 0.1f, viewDistance)),
			dragActive(false)
		{
		}
	};

	struct alignas(16) MaterialConstants
	{
		DirectX::XMFLOAT4 baseColorFactor;
		DirectX::XMFLOAT4 emissiveFactor;

		float metallicFactor;
		float roughnessFactor;
		float alphaMask;
		float alphaMaskCutoff;

		int baseColorTextureIndex;
		int metallicRoughnessTextureIndex;
		int normalTextureIndex;
		int occlusionTextureIndex;

		int emissiveTextureIndex;

		int pad[3];
	};

	struct alignas(16) MeshConstants
	{
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX mvp;
	};

	struct Material
	{
		struct TextureIndices
		{
			int baseColorTextureIndex = -1;
			int metallicRoughnessTextureIndex = -1;
			int normalTextureIndex = -1;
			int occlusionTextureIndex = -1;
			int emissiveTextureIndex = -1;
		};

		Texture2D baseColorTexture{};
		Texture2D metallicRoughnessTexture{};
		Texture2D normalTexture{};
		Texture2D occlusionTexture{};
		Texture2D emissiveTexture{};

		TextureIndices textureIndices;

		DirectX::XMFLOAT4 baseColorFactor{};
		DirectX::XMFLOAT4 emissiveFactor{};
		float metallicFactor = 0.0f;
		float roughnessFactor = 0.0f;
		float alphaMask = 0.0f;
		float alphaCutoff = 0.0f;

		DescriptorSetType descriptorSet;
		ConstantBuffer constantBuffer;
	};

	struct MeshPrimitive
	{
		Material material;

		uint32_t vertexOffset;
		uint32_t indexOffset;
		uint32_t indexCount;
	};

	struct alignas(16) Mesh
	{
		DirectX::XMMATRIX localTransform;

		MeshConstants meshConstants;
		ConstantBuffer constantBuffer;
		DescriptorSetType descriptorSet;

		eastl::vector<MeshPrimitive> primitives;
	};

	struct PBRVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv0;
		DirectX::XMFLOAT2 uv1;
	};

	struct PBRSceneComponent
	{
		VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;

		eastl::vector<Mesh> meshes;
	};
}