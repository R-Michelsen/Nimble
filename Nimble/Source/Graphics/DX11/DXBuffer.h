#pragma once

#include <d3d11_3.h>
#include "../Enums.h"

namespace Nimble
{
	struct DXBuffer
	{
		ShaderStageFlags shaderStageFlags = ShaderStageFlags::UnknownStage;
		BufferUsageFlags usage = BufferUsageFlags::BufferReadOnly;

		ID3D11Buffer* buf = nullptr;

		ID3D11ShaderResourceView* SRV = nullptr;
		ID3D11UnorderedAccessView* UAV = nullptr;
	};
}