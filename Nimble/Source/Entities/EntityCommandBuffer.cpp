#pragma once
#include "NimblePCH.h"
#include "EntityCommandBuffer.h"

#include "EntityManager.h"

namespace Nimble
{
	void EntityDestroyCommand::Play(EntityManager* const entityManager)
	{
		entityManager->Destroy(entity);
	}

	EntityCommandBuffer::EntityCommandBuffer(EntityManager* const entityManager_) : entityManager(entityManager_)
	{
	}

	void EntityCommandBuffer::DestroyEntity(Entity entity)
	{
		EntityDestroyCommand destroyCommand;
		destroyCommand.entity = entity;
		destroyBuffer.push_back(destroyCommand);
	}

	void EntityCommandBuffer::PlayCommands()
	{
		for (auto& cmd : destroyBuffer)
		{
			cmd.Play(entityManager);
		}
	}
}