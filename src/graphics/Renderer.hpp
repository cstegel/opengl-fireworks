#pragma once

#include "cs488/ShaderProgram.hpp"
#include "graphics/DisplayMode.hpp"
#include "graphics/RenderFeature.hpp"

#include <Ecs.hh>

namespace fw
{
	class RenderContext;
	class GraphicsManager;

	class Renderer
	{
	public:
		Renderer(GraphicsManager & graphicsManager);
		~Renderer();

		void Render(RenderContext & context);

	private:
		void bindLights(ShaderProgram & shader, RenderContext & context);

		GraphicsManager &graphics;
		ShaderProgram defaultShader;
		ShaderProgram normalOutputShader;
		ShaderProgram lightShader;
};
}