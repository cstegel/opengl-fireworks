#pragma once

#include "cs488/ShaderProgram.hpp"
#include "graphics/DisplayMode.hpp"
#include "graphics/RenderFeature.hpp"
#include "graphics/GBuffer.hpp"

#include <Ecs.hh>
#include <glm/glm.hpp>

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
		void bindWorldSpaceLights(ShaderProgram & shader, RenderContext & context);
		void bindScreenSpaceLights(
			ShaderProgram & shader,
			RenderContext & context,
			const glm::vec3 & viewPos);

		void renderModels(RenderContext & context, ShaderProgram & shader);
		void initQuad();
		void renderQuad();
		void initPostProcessFBO();

		bool isGBufferDebugDisplayMode(DisplayMode mode) const;

		GraphicsManager &graphics;
		ShaderProgram shadingPassShader;
		ShaderProgram gBufferDebugShader;
		ShaderProgram lightDebugShader;
		ShaderProgram lightShader;
		ShaderProgram stencilShader;
		ShaderProgram postProcessShader;

		ShaderProgram geometryPassShader;

		GBuffer gBuffer;
		GLuint quadVAO; // VAO for drawing the quad in the lighting pass

		GLuint postProcessFBO = 0;
		Texture texPostProcessInput;
};
}