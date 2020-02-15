#pragma once

#include <WinUser.h>

#include "../../Application/InputHandler.h"
#include "../../Math/MathCommon.h"
#include "../RenderTypes.h"
#include "../../Entities/EntityManager.h"

namespace Nimble
{
	struct NoClipCameraSystem : ComponentSystem
	{
		void Init()
		{
			DirectX::XMVECTOR front;

			entityManager->ForEach<RenderCamera>([&](Entity entity, RenderCamera& camera)
				{
					camera.front.x = cosf(DirectX::XMConvertToRadians(camera.yaw)) * cosf(DirectX::XMConvertToRadians(camera.pitch));
					camera.front.y = sinf(DirectX::XMConvertToRadians(camera.pitch));
					camera.front.z = sinf(DirectX::XMConvertToRadians(camera.yaw)) * cosf(DirectX::XMConvertToRadians(camera.pitch));
					front = DirectX::XMLoadFloat3A(&camera.front);
					front = DirectX::XMVector3Normalize(front);
					DirectX::XMStoreFloat3A(&camera.front, front);

					camera.view = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3A(&camera.position),
						DirectX::XMVectorAdd(DirectX::XMLoadFloat3A(&camera.position), front), DirectX::XMLoadFloat3A(&camera.up));
				});
		}

		void Update(float deltaTime, InputHandler* const inputHandler)
		{
			entityManager->ForEach<RenderCamera>([&](Entity entity, RenderCamera& camera)
				{
					if (inputHandler->mouseDrag.right.active && !camera.dragActive)
					{
						camera.dragActive = true;
						ShowCursor(false);
					}
					else if (!inputHandler->mouseDrag.right.active && camera.dragActive)
					{
						camera.dragActive = false;
						// Set the cursor back to the cached position
						SetCursorPos(inputHandler->mouseDrag.right.cachedPos.x, inputHandler->mouseDrag.right.cachedPos.y);
						ShowCursor(true);
					}

					if (inputHandler->mouseDrag.right.active || inputHandler->keyState[0x41] || inputHandler->keyState[0x44] ||
						inputHandler->keyState[0x53] || inputHandler->keyState[0x57])
					{

						const float cameraSpeed = 10.0f * deltaTime;

						// 'W'
						if (inputHandler->keyState[0x57])
						{
							DirectX::XMVECTOR v = DirectX::XMLoadFloat3A(&camera.position);
							v = DirectX::XMVectorAdd(v, DirectX::XMVectorScale(XMLoadFloat3A(&camera.front), cameraSpeed));
							DirectX::XMStoreFloat3A(&camera.position, v);
						}
						// 'S'
						if (inputHandler->keyState[0x53])
						{
							DirectX::XMVECTOR v = DirectX::XMLoadFloat3A(&camera.position);
							v = DirectX::XMVectorSubtract(v, DirectX::XMVectorScale(XMLoadFloat3A(&camera.front), cameraSpeed));
							DirectX::XMStoreFloat3A(&camera.position, v);
						}
						// 'D'
						if (inputHandler->keyState[0x44])
						{
							DirectX::XMVECTOR v = DirectX::XMLoadFloat3A(&camera.position);
							v = DirectX::XMVectorSubtract(v, DirectX::XMVectorScale(DirectX::XMVector3Normalize(
								DirectX::XMVector3Cross(DirectX::XMLoadFloat3A(&camera.front), { 0.0f, 1.0f, 0.0f })), cameraSpeed));
							DirectX::XMStoreFloat3A(&camera.position, v);
						}
						// 'A'
						if (inputHandler->keyState[0x41])
						{
							DirectX::XMVECTOR v = DirectX::XMLoadFloat3A(&camera.position);
							v = DirectX::XMVectorAdd(v, DirectX::XMVectorScale(DirectX::XMVector3Normalize(
								DirectX::XMVector3Cross(DirectX::XMLoadFloat3A(&camera.front), { 0.0f, 1.0f, 0.0f })), cameraSpeed));

							DirectX::XMStoreFloat3A(&camera.position, v);
						}

						if (inputHandler->mouseDrag.right.active)
						{
							const float dragSpeed = 200.0f * deltaTime;

							if (inputHandler->mouseDelta.x != 0)
							{
								camera.yaw -= static_cast<float>(inputHandler->mouseDelta.x) * dragSpeed;
							}
							if (inputHandler->mouseDelta.y != 0)
							{
								camera.pitch -= static_cast<float>(inputHandler->mouseDelta.y) * dragSpeed;
								// Put constraints on pitch.
								// 1.5533rad is approx 89deg
								if (camera.pitch > 89.0f)
								{
									camera.pitch = 89.0f;
								}
								if (camera.pitch < -89.0f)
								{
									camera.pitch = -89.0f;
								}
							}

							// Update camera front (direction)
							DirectX::XMVECTOR front;
							camera.front.x = cosf(DirectX::XMConvertToRadians(camera.yaw)) * cosf(DirectX::XMConvertToRadians(camera.pitch));
							camera.front.y = sinf(DirectX::XMConvertToRadians(camera.pitch));
							camera.front.z = sinf(DirectX::XMConvertToRadians(camera.yaw)) * cosf(DirectX::XMConvertToRadians(camera.pitch));
							front = DirectX::XMLoadFloat3A(&camera.front);
							front = DirectX::XMVector3Normalize(front);
							DirectX::XMStoreFloat3A(&camera.front, front);
						}
						// Update view matrix
						camera.view = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3A(&camera.position),
							DirectX::XMVectorAdd(DirectX::XMLoadFloat3A(&camera.position), DirectX::XMLoadFloat3A(&camera.front)), DirectX::XMLoadFloat3A(&camera.up));
					}
				});
		}
	};
}
