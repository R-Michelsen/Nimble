#include "Test.h"

#include <DirectXMath.h>

#include "../../Nimble/Source/Entities/SystemDispatch.h"
#include "../../Nimble/Source/Graphics/EntitySystems/NoClipCameraSystem.h"

void Test::Init()
{
	camera = Entities->Create();
	Entities->RegisterComponent<Nimble::RenderCamera>();
	Entities->AddComponent<Nimble::RenderCamera>(camera, Nimble::RenderCamera({ 5.0f, 5.0f, 5.0f }, 16.0f / 9.0f, 1000.0f, -135.0f, -35.0f));

	RenderGraph->ActivateRenderPath<Nimble::ForwardRenderPath>(true, true);
}

void Test::Simulate(float deltaTime)
{
	Entities->systemDispatch->RunSystem<Nimble::NoClipCameraSystem>(deltaTime, Input);
}
