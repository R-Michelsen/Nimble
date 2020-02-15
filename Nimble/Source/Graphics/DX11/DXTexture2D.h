#pragma once

#include "../Enums.h"

namespace Nimble
{
	struct DXTexture2D
	{
		ShaderStageFlags shaderStage = ShaderStageFlags::UnknownStage;
		TextureUsageFlags usage = TextureUsageFlags::TextureReadOnly;

		ID3D11Texture2D* tex = nullptr;

		ID3D11ShaderResourceView* SRV = nullptr;
		ID3D11UnorderedAccessView* UAV = nullptr;
		ID3D11RenderTargetView* RTV = nullptr;
		ID3D11DepthStencilView* DSV = nullptr;

		ID3D11SamplerState* SS = nullptr;
	};
}
