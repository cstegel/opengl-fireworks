#pragma once

#include "cs488/ShaderProgram.hpp"
#include "graphics/DisplayMode.hpp"
#include "graphics/RenderFeature.hpp"
#include "graphics/GBuffer.hpp"

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
		void renderModels(RenderContext & context, ShaderProgram & shader);
		void initQuad();
		void renderQuad();

		bool isGBufferDebugDisplayMode(DisplayMode mode) const;

		GraphicsManager &graphics;
		ShaderProgram shadingPassShader;
		ShaderProgram gBufferDebugShader;
		ShaderProgram lightDebugShader;
		ShaderProgram lightShader;

		ShaderProgram geometryPassShader;

		GBuffer gBuffer;
		GLuint quadVAO; // VAO for drawing the quad in the lighting pass
};
}