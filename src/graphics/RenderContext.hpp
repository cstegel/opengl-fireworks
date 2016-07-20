#pragma once

#include "Common.hpp"
#include "graphics/DisplayMode.hpp"
#include "graphics/RenderFeature.hpp"
#include "graphics/RenderStage.hpp"

#include <Ecs.hh>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace fw
{
	class InputManager;
	class Game;

	class RenderContext
	{
	public:
		class Timer
		{
		private:
			friend class RenderContext;
			Timer(RenderStage stage);

			RenderStage stage;
			double startTime;
		};

		RenderContext(Game & game, ecs::EntityManager & entityManager);
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

		const glm::vec3 GetViewForward() const;

		GLFWwindow *GetWindow() const;

		int GetWindowWidth() const;
		int GetWindowHeight() const;

		// call this every frame in case window resizes
		void UpdateWindowDimensions();

		float AspectRatio() const;


		DisplayMode GetDisplayMode() const;
		void SetDisplayMode(DisplayMode mode);

		void ToggleRenderFeature(RenderFeature feature, bool enabled);
		bool IsRenderFeatureEnabled(RenderFeature feature) const;

		RenderStage GetRenderStage() const;

		/**
		 * Returns a timer that updates the time of this stage
		 * when it is finished (EndRenderStage() is called)
		 */
		Timer StartRenderStage(RenderStage stage);
		void EndRenderStage(Timer & timer);

		double GetRenderStageAvgTime(RenderStage stage) const;

		static const uint MAX_SHADER_POINT_LIGHTS = 1;
		Game & game;
		ecs::EntityManager & entityManager;
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

		RenderStage currentRenderStage;

		// exponential moving averages of times for render stages
		unordered_map<RenderStage, double, EnumHash> renderTimes;
	};
}