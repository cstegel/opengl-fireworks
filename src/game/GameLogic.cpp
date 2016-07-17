#include "game/GameLogic.hpp"
#include "core/Game.hpp"
#include "graphics/RenderStage.hpp"

#include "ecs/components/Transform.hpp"
#include "ecs/components/ModelInstance.hpp"
#include "ecs/components/View.hpp"
#include "ecs/components/PointLight.hpp"

#include "audio/SoundId.hpp"

#include <cmath>
#include <iomanip>
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

	// auto playerLight = player.Assign<PointLight>();
	// playerLight->intensity = 1;

	game.graphics.SetPlayerView(player);
	game.audio.SetPlayerLocation(player);

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
	lightTransform->Translate(1, 25, 1);
	auto pointLight = light1.Assign<PointLight>();
	pointLight->intensity = 2;

	// ecs::Entity light2 = game.entityManager.NewEntity();
	// auto lightTransform2 = light2.Assign<Transform>();
	// lightTransform2->Translate(1, 120, 1);
	// auto pointLight2 = light2.Assign<PointLight>();
	// pointLight2->intensity = 10;

	game.audio.Play(SoundId::DRAGON_ROAR, alduin);
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

		if (ImGui::CollapsingHeader("Render Timing (ms)"))
		{
			double totalTime = 0;
			for (uint stage = (uint)RenderStage::GEOMETRY; stage < (uint)RenderStage::NUM_STAGES; stage++)
			{
				float renderTime = game.graphics.GetRenderStageAvgTime((RenderStage)stage);
				totalTime += renderTime;

				std::stringstream ss;
				ss << fw::to_string((RenderStage)stage)
				   << ": "
				   << std::setprecision(2)
				   << 1000*renderTime;

				ImGui::Text(ss.str().c_str());
			}

			std::stringstream ss;
			ss << "Total: " << ": " << std::setprecision(2) << 1000*totalTime;
			ImGui::Text(ss.str().c_str());
		}

		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Options"))
		{
			ImGui::Checkbox("Use Normal Maps (N)", &enableNormalMaps);
			ImGui::Checkbox("Volumetric Lighting (V)", &enableSSVolumetricLighting);
			ImGui::Checkbox("Gamma Correct (G)", &enableGammaCorrect);
			ImGui::Checkbox("HDR (H)", &enableHDR);
		}

		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Display Mode"))
		{
			ImGui::RadioButton("Regular (1)", (int*)&displayMode, (int)DisplayMode::REGULAR);
			ImGui::RadioButton("Normals (2)", (int*)&displayMode, (int)DisplayMode::NORMALS);
			ImGui::RadioButton("Albedo (3)", (int*)&displayMode, (int)DisplayMode::ALBEDO);
			ImGui::RadioButton("Specular (4)", (int*)&displayMode, (int)DisplayMode::SPECULAR);
			ImGui::RadioButton("Shininess (5)", (int*)&displayMode, (int)DisplayMode::SHININESS);
			ImGui::RadioButton("Position (6)", (int*)&displayMode, (int)DisplayMode::POSITION);
			ImGui::RadioButton("Stencil (7)", (int*)&displayMode, (int)DisplayMode::STENCIL);
			ImGui::RadioButton("Brightness (8)", (int*)&displayMode, (int)DisplayMode::BRIGHTNESS);
		}

		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			return false;
		}

	ImGui::End();

	// "~" or "`" key
	if (game.input.IsPressed(GLFW_KEY_GRAVE_ACCENT))
	{
		uiInputMode = !uiInputMode;
		game.input.LockFocus(uiInputMode);
		game.input.ToggleCursor(uiInputMode);
	}

	if	  (game.input.IsPressed(GLFW_KEY_1)) displayMode = DisplayMode::REGULAR;
	else if (game.input.IsPressed(GLFW_KEY_2)) displayMode = DisplayMode::NORMALS;
	else if (game.input.IsPressed(GLFW_KEY_3)) displayMode = DisplayMode::ALBEDO;
	else if (game.input.IsPressed(GLFW_KEY_4)) displayMode = DisplayMode::SPECULAR;
	else if (game.input.IsPressed(GLFW_KEY_5)) displayMode = DisplayMode::SHININESS;
	else if (game.input.IsPressed(GLFW_KEY_6)) displayMode = DisplayMode::POSITION;
	else if (game.input.IsPressed(GLFW_KEY_7)) displayMode = DisplayMode::STENCIL;
	else if (game.input.IsPressed(GLFW_KEY_8)) displayMode = DisplayMode::BRIGHTNESS;

	if (game.input.IsPressed(GLFW_KEY_N))
	{
		enableNormalMaps = !enableNormalMaps;
	}
	if (game.input.IsPressed(GLFW_KEY_V))
	{
		enableSSVolumetricLighting = !enableSSVolumetricLighting;
	}
	if (game.input.IsPressed(GLFW_KEY_G))
	{
		enableGammaCorrect = !enableGammaCorrect;
	}
	if (game.input.IsPressed(GLFW_KEY_H))
	{
		enableHDR = !enableHDR;
	}

	game.graphics.SetDisplayMode(displayMode);
	game.graphics.ToggleRenderFeature(RenderFeature::NORMAL_MAPS, enableNormalMaps);
	game.graphics.ToggleRenderFeature(RenderFeature::SCREEN_SPACE_VOLUMETRIC_LIGHTING,
		enableSSVolumetricLighting);
	game.graphics.ToggleRenderFeature(RenderFeature::HDR, enableHDR);
	game.graphics.ToggleRenderFeature(RenderFeature::GAMMA_CORRECT, enableGammaCorrect);

	return true;
}

}

