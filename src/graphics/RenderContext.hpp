#pragma once

#include "Common.hpp"
#include "graphics/DisplayMode.hpp"
#include "graphics/RenderFeature.hpp"

#include <Ecs.hh>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace fw
{
	class InputManager;

	class RenderContext
	{
	public:
		RenderContext(ecs::EntityManager & entityManager);
		virtual ~RenderContext();

		void CreateWindow();
		bool ShouldClose() const;
		void SetTitle(string title);
		void BindInputCallbacks(InputManager & inputManager);

		/**
		 * Sets the entity to get the player's View from.
		 * Must have a View component.
		 */
		void SetPlayerView(ecs::Entity playerView);
		ecs::Entity GetPlayerView() const;

		// Compute and cache the view transform
		const glm::mat4 & CacheView();
		const glm::mat4 & GetCachedView() const;

		// Compute and cacche the projection transform
		const glm::mat4 & CacheProjection();
		const glm::mat4 & GetCachedProjection() const;

		GLFWwindow *GetWindow() const;

		int GetWindowWidth() const;
		int GetWindowHeight() const;

		// call this every frame in case window resizes
		void UpdateWindowDimensions();

		float AspectRatio() const;

		ecs::EntityManager & entityManager;

		DisplayMode GetDisplayMode() const;
		void SetDisplayMode(DisplayMode mode);

		void ToggleRenderFeature(RenderFeature feature, bool enabled);
		bool IsRenderFeatureEnabled(RenderFeature feature) const;

	private:
		void validateView(ecs::Entity) const;

		GLFWwindow *window = nullptr;
		int windowWidth = 1280;
		int windowHeight = 720;
		ecs::Entity playerView;

		glm::mat4 cachedView;
		glm::mat4 cachedProjection;

		DisplayMode displayMode;
		std::bitset<static_cast<uint>(RenderFeature::NUM_VALUES)> activeRenderFeatures;
	};
}