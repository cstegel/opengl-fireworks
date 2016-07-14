#pragma once

#include "Common.hpp"
#include "game/HumanControlSystem.hpp"
#include "graphics/DisplayMode.hpp"

namespace fw
{
	class Game;

	class GameLogic
	{
	public:
		GameLogic(Game & game);
		~GameLogic();

		void Init();
		bool Frame(double dtSinceLastFrame);
	private:
		Game & game;
		HumanControlSystem humanControlSystem;

		ecs::Entity player;

		bool useNormalMaps = true;
		DisplayMode displayMode;
	};
}
