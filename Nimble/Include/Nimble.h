#pragma once

#include <EASTL/unique_ptr.h>

#include "../Source/Application/InputHandler.h"
#include "../Source/Entities/EntityTypes.h"
#include "../Source/Entities/EntityManager.h"
#include "../Source/Graphics/RenderGraph/RenderGraph.h"
#include "../Source/Graphics/RenderPaths/ForwardRenderPath.h"
#include "../Source/Graphics/BackendTypedefs.h"
#include "../Source/Graphics/DX11/DXGraphicsContext.h"
#include "../Source/Graphics/DX11/DXResourceManager.h"
#include "../Source/Graphics/Enums.h"
#include "../Source/Graphics/RenderTypes.h"

namespace Nimble
{
	class Game
	{
	public:
		InputHandler* Input;
		GraphicsContextType* Graphics;
		ResourceManagerType* Resources;
		EntityManager* Entities;
		RenderGraph* RenderGraph;

		virtual ~Game() = default;
		virtual void Init() = 0;
		virtual void Simulate(float deltaTime) = 0;
	};

	extern eastl::unique_ptr<Game> CreateGame();
}

int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow);