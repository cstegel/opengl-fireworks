#pragma once

#include "cs488/ShaderProgram.hpp"
#include "graphics/DisplayMode.hpp"
#include "graphics/RenderFeature.hpp"
#include "graphics/GBuffer.hpp"
#include "graphics/RenderContext.hpp"

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

	struct ShadowMap
	{
		GLuint FBO;
		GLuint texDepth;
		glm::mat4 worldToLightMat;
	};

	static const uint SHADOW_WIDTH = 800;
	static const uint SHADOW_HEIGHT = 800;

	// binds a point light to a shader at the given index
	void bindWorldSpaceLight(
		ecs::Entity lightEnt,
		uint bindIndex,
		ShaderProgram & shader,
		RenderContext & context);

	void bindScreenSpaceLight(
		ecs::Entity lightEnt,
		uint bindIndex,
		ShaderProgram & shader,
		RenderContext & context,
		const glm::vec3 & viewPos);

	void renderModels(RenderContext & context, ShaderProgram & shader);
	void initQuad();
	void renderQuad();
	void initPostProcessFBO();
	void initLightModelPassFBO();
	void initTempMixFBO();
	void initShadowMaps();

	void generateShadowMap(ecs::Entity lightEnt, uint bindIndex, RenderContext & context, GLuint rebindFBO);

	/**
	 * terribly programmed function for adding together the gBuffer outline texture and
	 * the light models texture.  They will end up in "texTempMixColour"
	 */
	void mixOutlineWithLightModelsToTempTexture();

	bool isGBufferDebugDisplayMode(DisplayMode mode) const;

	GraphicsManager &graphics;
	ShaderProgram shadingPassShader;
	ShaderProgram gBufferDebugShader;
	ShaderProgram lightDebugShader;
	ShaderProgram lightShader;
	ShaderProgram stencilShader;
	ShaderProgram postProcessShader;
	ShaderProgram copyTextureShader;
	ShaderProgram nullShader;
	ShaderProgram shadowMapShader;
	ShaderProgram shadowMapDebugShader;

	ShaderProgram geometryPassShader;

	GBuffer gBuffer;
	GLuint quadVAO; // VAO for drawing the quad in the lighting pass

	// used for rendering various passes to different textures
	GLuint postProcessFBO = 0;
	Texture texPostProcessInput;

	GLuint lightModelPassFBO = 0;
	Texture texLightModelColour;

	GLuint tempMixFBO = 0;
	Texture texTempMixColour;

	array<ShadowMap, RenderContext::MAX_SHADER_POINT_LIGHTS> shadowMaps;
};

}