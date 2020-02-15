#include "NimblePCH.h"
#include "ForwardRenderPath.h"

#include "../DX11/DXResourceManager.h"
#include "../RenderGraph/RenderGraph.h"
#include "../RenderTypes.h"

using namespace DirectX;

namespace Nimble
{
	ForwardRenderPath::ForwardRenderPath(ResourceManagerType* const resourceManager_, EntityManager* const entityManager_, RenderGraph* const renderGraph_) :
		resourceManager(resourceManager_),
		entityManager(entityManager_),
		renderGraph(renderGraph_)
	{
		entityManager->RegisterComponent<PBRSceneComponent>();
		auto testEntity = entityManager->Create();
		entityManager->AddComponent<PBRSceneComponent>(testEntity, resourceManager->LoadModel("Nimble/Resources/Models/Sponza/Sponza.gltf"));

		PipelineDescription pipelineDescription{};
		pipelineDescription.vertexInputAttributes = {
				VertexInputAttribute::VERTEX_ATTRIBUTE_POSITION,
				VertexInputAttribute::VERTEX_ATTRIBUTE_NORMAL,
				VertexInputAttribute::VERTEX_ATTRIBUTE_UV,
				VertexInputAttribute::VERTEX_ATTRIBUTE_UV
		};

		pipelineDescription.descriptorLayout = {
				{ ResourceType::ConstantBuffer, 0 },
				{ ResourceType::ConstantBuffer, 1 },
				{ ResourceType::Texture, 0 },
				{ ResourceType::Texture, 1 },
				{ ResourceType::Texture, 2 },
				{ ResourceType::Texture, 3 },
				{ ResourceType::Texture, 4 }
		};

		auto pipeline = resourceManager->CreateGraphicsPipeline("Nimble/Resources/Shaders/DefaultVS.hlsl", "main",
			"Nimble/Resources/Shaders/DefaultPS.hlsl", "main", pipelineDescription);

		renderGraph->AddRenderPass("PBR Pass", [&](RenderPassBuilder* builder)
			{
				builder->WriteToBackBufferRenderTarget();

				return [=](EntityManager* entities, CommandBufferType* commandBuffer)
				{
					DirectX::XMMATRIX VP;

					entities->ForEach<RenderCamera>([&](Entity entity, RenderCamera& camera) noexcept
						{
							VP = camera.view * camera.projection;
						});

					commandBuffer->BindPipeline(pipeline);

					entities->ForEach<PBRSceneComponent>([&](Entity entity, PBRSceneComponent& scene) noexcept
						{
							for (auto& mesh : scene.meshes)
							{
								mesh.meshConstants.model = mesh.localTransform;
								mesh.meshConstants.mvp = mesh.localTransform * VP;
								commandBuffer->UpdateConstantBuffer(mesh.constantBuffer, &mesh.meshConstants);

								for (const auto& primitive : mesh.primitives)
								{
									commandBuffer->BindVertexBuffer(scene.vertexBuffer, sizeof(PBRVertex), primitive.vertexOffset * sizeof(PBRVertex));
									commandBuffer->BindIndexBuffer(scene.indexBuffer, primitive.indexOffset * sizeof(uint32_t));
									commandBuffer->BindDescriptorSet(mesh.descriptorSet);
									commandBuffer->BindDescriptorSet(primitive.material.descriptorSet);

									commandBuffer->DrawIndexed(primitive.indexCount, 0);
								}
							}
						});
				};
			});
	}
}