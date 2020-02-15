#pragma once

#include <EASTL/hash_map.h>

#include "../BackendTypedefs.h"
#include "../Resources/ResourceTypes.h"
#include "../Enums.h"

namespace Nimble
{

	struct DXGraphicsPipeline
	{
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;

		ID3D11InputLayout* inputLayout = nullptr;

		ID3D11DepthStencilState* depthStencilState = nullptr;

		eastl::hash_map<uint32_t, ResourceType> descriptorLayout;
	};
}

