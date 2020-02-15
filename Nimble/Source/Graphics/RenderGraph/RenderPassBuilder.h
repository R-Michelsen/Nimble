#pragma once

#include <string>

#include "../Enums.h"

namespace Nimble
{
	class RenderGraph;
	struct RenderPass;
	struct TransientResource;
	struct TransientTexture;
	struct TransientStorageBuffer;

	class RenderPassBuilder
	{
		using RenderPassHandle = uint32_t;

	public:
		RenderPassBuilder(RenderGraph* renderGraph_);

		void Read(TransientTexture& resource, const uint32_t binding);
		void Read(TransientStorageBuffer& resource, const uint32_t binding);

		void Write(TransientTexture& resource, const uint32_t binding);
		void Write(TransientStorageBuffer& resource, const uint32_t binding);

		void WriteToRenderTargets(eastl::vector<TransientTexture> targetTextures);
		void WriteToBackBufferRenderTarget();

	private:
		friend struct RenderPass;
		void Attach(RenderPassHandle passIndex_);

		RenderGraph* renderGraph;
		RenderPassHandle passIndex;

		uint32_t RegisterTransientResource(TransientTexture& resource);
		uint32_t RegisterTransientResource(TransientStorageBuffer& resource);
	};
}
