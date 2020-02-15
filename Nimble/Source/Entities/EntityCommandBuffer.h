#pragma once

#include "EntityTypes.h"

namespace Nimble
{
	class EntityManager;

	struct EntityDestroyCommand
	{
		Entity entity;

		void Play(EntityManager* const entityManager);
	};

	struct EntityCommandBuffer
	{
		EntityCommandBuffer(EntityManager* const entityManager);

		EntityManager* const entityManager;
		eastl::vector<EntityDestroyCommand> destroyBuffer;

		void DestroyEntity(Entity entity);
		void PlayCommands();
	};
}