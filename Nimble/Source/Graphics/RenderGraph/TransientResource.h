#pragma once

#include <EASTL/vector.h>
#include <EASTL/string.h>

#include "RenderPass.h"
#include "../../Math/MathCommon.h"
#include "../Enums.h"

namespace Nimble
{
	struct TransientResource
	{
		eastl::string identifier;
		ShaderStageFlags shaderStageFlags;
		eastl::string alias;

		eastl::vector<uint32_t> writers;
		eastl::vector<uint32_t> readers;

		TransientResource(eastl::string identifier_, NimbleFlags shaderStageFlags_, eastl::string alias_ = "") :
			identifier(identifier_),
			shaderStageFlags(static_cast<ShaderStageFlags>(shaderStageFlags_)),
			alias(alias_)
		{
		}
	};

	struct TransientTexture : public TransientResource
	{
		TextureFormat format;
		TextureSize sizeMode;
		DirectX::XMUINT2 size;
		uint32_t levels;

		Texture2D physicalTexture;

		TransientTexture(eastl::string identifier_, NimbleFlags shaderStageFlags_, TextureFormat format_, TextureSize sizeMode_, DirectX::XMUINT2 size_,
			uint32_t levels_, eastl::string alias_ = "") :
			TransientResource(identifier_, shaderStageFlags_, alias_),
			format(format_),
			sizeMode(sizeMode_),
			size(size_),
			levels(levels_) 
		{
		}
	};

	struct TransientStorageBuffer : public TransientResource
	{
		uint32_t stride;
		uint32_t count;

		StorageBuffer physicalBuffer;

		TransientStorageBuffer(eastl::string identifier_, NimbleFlags shaderStageFlags_, uint32_t stride_, uint32_t count_, eastl::string alias_ = "") :
			TransientResource(identifier_, shaderStageFlags_, alias_),
			stride(stride_),
			count(count_)
		{
		}
	};
}