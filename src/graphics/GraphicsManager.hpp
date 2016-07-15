#pragma once

// NO INCLUDES BEFORE THIS SO WE GUARANTEE glew is included before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <Ecs.hh>

#include "Common.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Model.hpp"
#include "graphics/DisplayMode.hpp"
#include "graphics/RenderFeature.hpp"
#include "core/InputManager.hpp"
#include "core/AssetManager.hpp"


namespace fw
{
	class Game;
	class RenderContext;

	class GraphicsManager
	{
	public:
		GraphicsManager(Game &game);
		~GraphicsManager();

		bool Frame();
		bool ShouldClose() const;

		uint GetWindowWidth() const;
		uint GetWindowHeight() const;

		/**
		 * Sets the entity to be used when rendering the player's view.
		 * This entity must have a "View" component.
		 */
		void SetPlayerView(ecs::Entity playerView);
		void BindInputCallbacks(InputManager & inputManager);
		static string GetShaderPath(const string & filename);

		Model & GetModel(string name);
		Model & GetLightModel();

		DisplayMode GetDisplayMode() const;
		void SetDisplayMode(DisplayMode mode);

		// returns if the feature was toggled to "enabled"
		void ToggleRenderFeature(RenderFeature feature, bool enabled);
		bool IsRenderFeatureEnabled(RenderFeature feature) const;

	private:
		static const string LIGHT_MODEL;

		void throwIfNotValidContext() const;
		void loadModels();

		Game & game;
		shared_ptr<RenderContext> renderContext;
		shared_ptr<Renderer> renderer;
		unordered_map<string, Model> models;

		double lastFrameEnd;
		double fpsTimer;
		uint32_t frameCounter = 0;
	};
}