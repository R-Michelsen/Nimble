#pragma once

#include "../Enums.h"
#include <d3d11_3.h>

#ifdef _DEBUG
	#ifndef DXCHECK
	#define DXCHECK(x) \
	{ \
		if(FAILED(x)) { assert(false); }; \
	}
	#endif
#else
	#ifndef DXCHECK
	#define DXCHECK
	#endif
#endif

namespace Nimble
{
	inline constexpr DXGI_FORMAT GetTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGBA32:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case TextureFormat::RGB32:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case TextureFormat::RG32:
			return DXGI_FORMAT_R32G32_FLOAT;
		case TextureFormat::R32:
			return DXGI_FORMAT_R32_FLOAT;
		case TextureFormat::RGBA16:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case TextureFormat::RG16:
			return DXGI_FORMAT_R16G16_FLOAT;
		case TextureFormat::R16:
			return DXGI_FORMAT_R16_FLOAT;
		case TextureFormat::RGBA8:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case TextureFormat::RG8:
			return DXGI_FORMAT_R8G8_UNORM;
		case TextureFormat::R8:
			return DXGI_FORMAT_R8_UNORM;
		case TextureFormat::D32:
			return DXGI_FORMAT_D32_FLOAT;
		case TextureFormat::D24S8:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case TextureFormat::UNKNOWN:
			return DXGI_FORMAT_UNKNOWN;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	inline constexpr D3D11_INPUT_ELEMENT_DESC GetInputElementDescription(const VertexInputAttribute attribute,
		uint32_t& posCount, uint32_t& colorCount, uint32_t& normCount, uint32_t& uvCount)
	{
		NIMBLE_ASSERT(posCount < 10 && "Maximum amount of position semantics is set to 10! (Can be changed if necessary...)!");
		NIMBLE_ASSERT(colorCount < 10 && "Maximum amount of color semantics is set to 10! (Can be changed if necessary...)!");
		NIMBLE_ASSERT(normCount < 10 && "Maximum amount of normal semantics is set to 10! (Can be changed if necessary...)!");
		NIMBLE_ASSERT(uvCount < 10 && "Maximum amount of texcoord semantics is set to 10! (Can be changed if necessary...)!");
		switch (attribute)
		{
		case VertexInputAttribute::VERTEX_ATTRIBUTE_POSITION:
			return { "POSITION", posCount++, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		case VertexInputAttribute::VERTEX_ATTRIBUTE_NORMAL:
			return { "NORMAL", normCount++, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		case VertexInputAttribute::VERTEX_ATTRIBUTE_UV:
			return { "TEXCOORD", uvCount++, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		case VertexInputAttribute::VERTEX_ATTRIBUTE_COLOR:
			return { "COLOR", colorCount++, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		case VertexInputAttribute::VERTEX_ATTRIBUTE_FLOAT:
			return { "TEXCOORD", uvCount++, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		case VertexInputAttribute::VERTEX_ATTRIBUTE_FLOAT3:
			return { "TEXCOORD", uvCount++, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		case VertexInputAttribute::VERTEX_ATTRIBUTE_FLOAT4:
			return { "TEXCOORD", uvCount++, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		default:
			return { "UNKNOWN", 0, DXGI_FORMAT_UNKNOWN, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}
	}
}