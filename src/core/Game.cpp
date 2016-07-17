#include "core/Game.hpp"

#include "ecs/components/Controller.hpp"
#include "ecs/components/Transform.hpp"
#include "ecs/components/View.hpp"
#include "ecs/components/ModelInstance.hpp"
#include "ecs/components/PointLight.hpp"
#include "ecs/components/Physics.hpp"


namespace fw
{
	Game::Game() : graphics(*this), audio(*this), logic(*this), physics(*this)
	{
		entityManager.RegisterComponentType<HumanController>();
		entityManager.RegisterComponentType<ModelInstance>();
		entityManager.RegisterComponentType<Physics>();
		entityManager.RegisterComponentType<PointLight>();
		entityManager.RegisterComponentType<Transform>();
		entityManager.RegisterComponentType<View>();

		worldUp = glm::vec3(0, 1, 0);
		worldForward = glm::vec3(0, 0, -1);
	}

	Game::~Game()
	{
	}

	glm::vec3 Game::GetWorldUp() const
	{
		return worldUp;
	}

	glm::vec3 Game::GetWorldForward() const
	{
		return worldForward;
	}

	int Game::Start()
	{
		graphics.BindInputCallbacks(input);
		logic.Init();
		lastFrameTime = glfwGetTime();

		while (true)
		{
			if (shouldStop()) break;
			if (!frame()) break;
		}

		return 0;
	}

	bool Game::shouldStop()
	{
		return graphics.ShouldClose();
	}

	bool Game::frame()
	{
		double frameTime = glfwGetTime();
		double dt = frameTime - lastFrameTime;

		input.Checkpoint();
		if (input.IsDown(GLFW_KEY_ESCAPE))
		{
			return false;
		}

		if (!logic.Frame(dt)) return false;
		if (!physics.Frame(dt)) return false;
		if (!graphics.Frame()) return false;
		if (!audio.Frame()) return false;

		lastFrameTime = frameTime;
		return true;
	}

	string Game::RootDir()
	{
		// compile time define
		return PROJECT_ROOT_DIR;
	}
}