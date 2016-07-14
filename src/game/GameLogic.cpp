#include "game/GameLogic.hpp"
#include "core/Game.hpp"

#include "ecs/components/Transform.hpp"
#include "ecs/components/ModelInstance.hpp"
#include "ecs/components/View.hpp"
#include "ecs/components/PointLight.hpp"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <imgui/imgui.h>

namespace fw
{

GameLogic::GameLogic(Game & game)
	: game(game), humanControlSystem(game.entityManager, game.input)
{
	displayMode = DisplayMode::REGULAR;
}

void GameLogic::Init()
{
	// player
	player = game.entityManager.NewEntity();
	ecs::Handle<Transform> playerTransform = player.Assign<Transform>();
	playerTransform->Translate(glm::vec3(0, 1, 0));

	player.Assign<View>(glm::radians(75.0f), 0.01, 1000);
	humanControlSystem.AssignController(player);

	auto playerLight = player.Assign<PointLight>();
	playerLight->intensity = 1;

	game.graphics.SetPlayerView(player);

	// models

	ecs::Entity suit = game.entityManager.NewEntity();
	ecs::Handle<Transform> suitTransform = suit.Assign<Transform>();
	suitTransform->Translate(glm::vec3(4, 0, 0));
	suit.Assign<ModelInstance>("cyborg");

	ecs::Entity alduin = game.entityManager.NewEntity();
	ecs::Handle<Transform> alduinTransform = alduin.Assign<Transform>();
	alduinTransform->Translate(glm::vec3(-3, 0, -6));
	alduinTransform->Rotate(glm::radians(-45.f), glm::vec3(0, 1, 0));
	alduinTransform->Scale(0.01);
	alduin.Assign<ModelInstance>("alduin");

	ecs::Entity ground = game.entityManager.NewEntity();
	ecs::Handle<Transform> groundTransform = ground.Assign<Transform>();
	groundTransform->Translate(glm::vec3(0, 0, 0));
	ground.Assign<ModelInstance>("ground_stone");

	ecs::Entity oldhouse2 = game.entityManager.NewEntity();
	ecs::Handle<Transform> oldhouse2Transform = oldhouse2.Assign<Transform>();
	oldhouse2Transform->Translate(glm::vec3(0, 0, 18));
	oldhouse2Transform->Rotate(glm::radians(90.f), glm::vec3(0, 1, 0));
	oldhouse2Transform->Scale(0.05);
	oldhouse2.Assign<ModelInstance>("oldhouse2");

	// lights
	ecs::Entity light1 = game.entityManager.NewEntity();
	auto lightTransform = light1.Assign<Transform>();
	lightTransform->Translate(1, 2, 1);
	auto pointLight = light1.Assign<PointLight>();
	pointLight->intensity = 2;

	ecs::Entity light2 = game.entityManager.NewEntity();
	auto lightTransform2 = light2.Assign<Transform>();
	lightTransform2->Translate(1, 120, 1);
	auto pointLight2 = light2.Assign<PointLight>();
	pointLight2->intensity = 10;
}

GameLogic::~GameLogic()
{
}

bool GameLogic::Frame(double dtSinceLastFrame)
{
	if (!humanControlSystem.Frame(dtSinceLastFrame)) return false;

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

		ImGui::Text("Adjust Camera (SPACE + Mouse move)");
		if (ImGui::Checkbox("Use Normal Maps (N)", &useNormalMaps))
		{}

		ImGui::Text("Display Mode");
		if (ImGui::RadioButton("Regular (1)", (int*)&displayMode, (int)DisplayMode::REGULAR))
		{}
		if (ImGui::RadioButton("Normals (2)", (int*)&displayMode, (int)DisplayMode::NORMALS))
		{}


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			return false;
		}

	ImGui::End();

	if (game.input.IsPressed(GLFW_KEY_1))
	{
		displayMode = DisplayMode::REGULAR;
	}
	else if (game.input.IsPressed(GLFW_KEY_2))
	{
		displayMode = DisplayMode::NORMALS;
	}

	if (game.input.IsPressed(GLFW_KEY_N))
	{
		useNormalMaps = !useNormalMaps;
	}

	game.graphics.SetDisplayMode(displayMode);
	game.graphics.ToggleRenderFeature(RenderFeature::NORMAL_MAPS, useNormalMaps);

	return true;
}

}

