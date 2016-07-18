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

	static const float GRAVITY;
private:
	Game & game;
};

}