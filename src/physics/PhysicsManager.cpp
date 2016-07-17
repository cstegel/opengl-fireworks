#include "physics/PhysicsManager.hpp"
#include "core/Game.hpp"

#include "ecs/components/Transform.hpp"
#include "ecs/components/Physics.hpp"

#include <Ecs.hh>

namespace fw
{

const float PhysicsManager::GRAVITY = 9.8f;

PhysicsManager::PhysicsManager(Game & game) : game(game)
{
}

PhysicsManager::~PhysicsManager()
{
}

bool PhysicsManager::Frame(double dtSinceLastFrame)
{
	glm::vec3 down = -game.GetWorldUp();
	glm::vec3 gravity = PhysicsManager::GRAVITY * down;

	for (ecs::Entity e : game.entityManager.EntitiesWith<Transform, Physics>())
	{
		ecs::Handle<Physics> physComp = e.Get<Physics>();
		ecs::Handle<Transform> transformComp = e.Get<Transform>();

		physComp->velocity +=
			(physComp->acceleration + gravity * physComp->gravityMult)
			* float(dtSinceLastFrame);

		glm::vec3 curPos = transformComp->GetPosition();
		glm::vec3 displacement = physComp->velocity * float(dtSinceLastFrame);

		if (displacement.y + curPos.y < 0)
		{
			displacement.y = -curPos.y;
			physComp->velocity *= -0.5;
		}

		transformComp->Translate(displacement);
	}

	return true;
}

}