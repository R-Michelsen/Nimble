#pragma once

#include <d3d11_1.h>

namespace Nimble
{
	constexpr D3D11_DEPTH_STENCIL_DESC PIPELINE_DEPTHSTENCIL_STATE_ENABLE{
		true,							// DEPTH ENABLE
		D3D11_DEPTH_WRITE_MASK_ALL,		// DEPTH WRITE MASK
		D3D11_COMPARISON_LESS,			// DEPTH COMPARISON
		false,							// STENCIL ENABLE
		0,								// STENCIL READ MASK
		0,								// STENCIL WRITE MASK
		{},								// STENCIL OP FRONTFACE
		{}								// STENCIL OP BACKFACE
	};
}