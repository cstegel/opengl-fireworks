#pragma once

#include "Common.hpp"

namespace fw
{

class Game;

class PhysicsManager
{
public:
	PhysicsManager(Game & game);
	~PhysicsManager();

	bool Frame(double dtSinceLastFrame);

private:
	Game & game;
	static const float GRAVITY;
};

}