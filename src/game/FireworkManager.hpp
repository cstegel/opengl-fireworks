#pragma once

#include "Common.hpp"

#include "ecs/components/Firework.hpp"

#include <Ecs.hh>
#include <glm/glm.hpp>
#include <random>

namespace fw
{

class Game;

class FireworkManager
{
public:
	FireworkManager(Game & game);
	~FireworkManager();

	bool Frame(double dt);

private:
	ecs::Entity createExplosionFirework(ecs::Entity firework);
	glm::vec3 randomUnitVector();
	glm::vec3 randomColour();

	Game & game;
	std::random_device randDevice;
	std::default_random_engine randEngine;
	std::uniform_real_distribution<float> randUnitFloat;
};

}