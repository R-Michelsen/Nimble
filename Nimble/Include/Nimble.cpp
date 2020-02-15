#include "NimblePCH.h"
#include "Nimble.h"

#include "../Source/Application/Window.h"

int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	auto window = eastl::make_unique<Nimble::Window>(hInstance, nCmdShow);

	auto graphics = eastl::make_unique<Nimble::GraphicsContextType>(hInstance, window->hwnd);
	auto resources = eastl::make_unique<Nimble::ResourceManagerType>(graphics->GetDevice());
	auto entities = eastl::make_unique<Nimble::EntityManager>();
	auto renderGraph = eastl::make_unique<Nimble::RenderGraph>(graphics.get(), resources.get(), entities.get());

	auto game = Nimble::CreateGame();
	game->Input = window->input.get();
	game->Graphics = graphics.get();
	game->Resources = resources.get();
	game->RenderGraph = renderGraph.get();
	game->Entities = entities.get();

	game->Init();
	while (window->ProcessMessages())
	{
		// If the windows is minimized, skip rendering
		if (IsIconic(window->hwnd)) continue;

		// Simulate and update the gamestate
		game->Simulate(window->time->delta);

		// Render the frame
		if (window->resized)
		{
			game->Graphics->Resize();
			game->RenderGraph->Recompile();
			window->resized = false;
		}
		game->RenderGraph->Execute();
		game->Graphics->Present();
	}

	// Order of destruction matters
	game.reset();
	renderGraph.reset();
	resources.reset();
	graphics.reset();
	entities.reset();
	window.reset();

	return 0;
}

// OPERATOR OVERLOADS FOR EASTL
void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	UNREFERENCED_PARAMETER(name);
	UNREFERENCED_PARAMETER(flags);
	UNREFERENCED_PARAMETER(debugFlags);
	UNREFERENCED_PARAMETER(file);
	UNREFERENCED_PARAMETER(line);

	return new uint8_t[size];
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	UNREFERENCED_PARAMETER(alignment);
	UNREFERENCED_PARAMETER(alignmentOffset);
	UNREFERENCED_PARAMETER(pName);
	UNREFERENCED_PARAMETER(flags);
	UNREFERENCED_PARAMETER(debugFlags);
	UNREFERENCED_PARAMETER(file);
	UNREFERENCED_PARAMETER(line);

	return new uint8_t[size];
}