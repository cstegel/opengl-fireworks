#include "game/FireworkManager.hpp"
#include "core/Game.hpp"

#include "ecs/components/Firework.hpp"
#include "ecs/components/Transform.hpp"
#include "ecs/components/Physics.hpp"
#include "ecs/components/PointLight.hpp"

namespace fw
{

FireworkManager::FireworkManager(Game & game)
	: game(game)
{
	randEngine = std::default_random_engine(randDevice());
	randUnitFloat = std::uniform_real_distribution<float>(-1.f, 1.f);
}

FireworkManager::~FireworkManager()
{
}

bool FireworkManager::Frame(double dt)
{
	for (ecs::Entity e : game.entityManager.EntitiesWith<Firework, Transform, Physics, PointLight>())
	{
		ecs::Handle<Firework> firework = e.Get<Firework>();
		firework->explosionCountdown -= (float)dt;

		if (firework->explosionCountdown < 0)
		{
			if (firework->numExplosionsLeft > 0)
			{
				for (uint i = 0; i < firework->numSplitsOnExplosion; ++i)
				{
					createExplosionFirework(e);
				}
			}

			e.Destroy();
		}
	}

	return true;
}

ecs::Entity FireworkManager::createExplosionFirework(ecs::Entity firework)
{
	ecs::Entity nextFirework = game.entityManager.NewEntity();

	nextFirework.Assign<Transform>(*firework.Get<Transform>());

	ecs::Handle<Physics> newPhys = nextFirework.Assign<Physics>(*firework.Get<Physics>());
	newPhys->acceleration = glm::vec3(0, 0, 0);
	glm::vec3 explosionVelocity = 20.f*randomUnitVector();
	newPhys->velocity += explosionVelocity;

	ecs::Handle<Firework> newFirework = nextFirework.Assign<Firework>(*firework.Get<Firework>());
	newFirework->explosionCountdown = 2.0f;
	newFirework->numExplosionsLeft -= 1;

	nextFirework.Assign<PointLight>(*firework.Get<PointLight>());

	return nextFirework;
}

glm::vec3 FireworkManager::randomUnitVector()
{
	return glm::normalize(
		glm::vec3(
			randUnitFloat(randEngine),
			randUnitFloat(randEngine),
			randUnitFloat(randEngine)
		)
	);
}

}