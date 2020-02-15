#pragma once

#include "../BackendTypedefs.h"

namespace Nimble
{
	class DXResourceManager;
	class EntityManager;
	class RenderGraph;

	class ForwardRenderPath
	{
	public:
		ForwardRenderPath(ResourceManagerType* const resourceManager_, EntityManager* const entityManager_, RenderGraph* const renderGraph_);

	private:
		friend class RenderGraph;

		ResourceManagerType* const resourceManager;
		EntityManager* const entityManager;
		RenderGraph* const renderGraph;
	};
}


