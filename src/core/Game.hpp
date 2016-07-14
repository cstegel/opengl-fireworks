#pragma once

#include "graphics/GraphicsManager.hpp"
#include "core/InputManager.hpp"
#include "game/GameLogic.hpp"
#include "audio/AudioManager.hpp"
#include "Common.hpp"

#include <Ecs.hh>
#include <glm/glm.hpp>

namespace fw
{
	class Game
	{
	public:
		Game();
		~Game();

		int Start();
		glm::vec3 GetWorldUp() const;
		static string RootDir();

		GraphicsManager graphics;
		AudioManager audio;
		GameLogic logic;
		InputManager input;
		ecs::EntityManager entityManager;

	private:
		bool frame();
		bool shouldStop();

		double lastFrameTime;

		glm::vec3 worldUp;
	};
}

