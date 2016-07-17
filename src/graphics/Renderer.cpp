#include "graphics/Renderer.hpp"
#include "graphics/RenderContext.hpp"
#include "graphics/GraphicsManager.hpp"
#include "cs488/GlErrorCheck.hpp"
#include "core/AssetManager.hpp"
#include "graphics/GraphicsManager.hpp"
#include "graphics/RenderStage.hpp"

#include "ecs/components/View.hpp"
#include "ecs/components/Transform.hpp"
#include "ecs/components/ModelInstance.hpp"
#include "ecs/components/PointLight.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include <algorithm>

namespace fw
{

Renderer::Renderer(GraphicsManager &graphics)
	: graphics(graphics), gBuffer(graphics.GetWindowWidth(), graphics.GetWindowHeight())
{
	gBufferDebugShader.generateProgramObject();
	gBufferDebugShader.attachVertexShader(
		GraphicsManager::GetShaderPath("ShadingPass.vert").c_str() );
	gBufferDebugShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("GBufferDebug.frag").c_str() );
	gBufferDebugShader.link();

	stencilShader.generateProgramObject();
	stencilShader.attachVertexShader(
	GraphicsManager::GetShaderPath("ShadingPass.vert").c_str() );
	stencilShader.attachFragmentShader(
	GraphicsManager::GetShaderPath("ScreenVolumetricLight.frag").c_str() );
	stencilShader.link();

	lightShader.generateProgramObject();
	lightShader.attachVertexShader(
		GraphicsManager::GetShaderPath("Light.vert").c_str() );
	lightShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("Light.frag").c_str() );
	lightShader.link();

	lightDebugShader.generateProgramObject();
	lightDebugShader.attachVertexShader(
		GraphicsManager::GetShaderPath("ShadingPass.vert").c_str() );
	lightDebugShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("LightDebug.frag").c_str() );
	lightDebugShader.link();

	postProcessShader.generateProgramObject();
	postProcessShader.attachVertexShader(
		GraphicsManager::GetShaderPath("ShadingPass.vert").c_str() );
	postProcessShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("PostProcess.frag").c_str() );
	postProcessShader.link();

	geometryPassShader.generateProgramObject();
	geometryPassShader.attachVertexShader(
		GraphicsManager::GetShaderPath("GeometryPass.vert").c_str() );
	geometryPassShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("GeometryPass.frag").c_str() );
	geometryPassShader.link();

	shadingPassShader.generateProgramObject();
	shadingPassShader.attachVertexShader(
		GraphicsManager::GetShaderPath("ShadingPass.vert").c_str() );
	shadingPassShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("ShadingPass.frag").c_str() );
	shadingPassShader.link();

	initPostProcessFBO();

	initQuad();
}

Renderer::~Renderer()
{}

// should clean this up by splitting into a class
void Renderer::initPostProcessFBO()
{
	glGenFramebuffers(1, &postProcessFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);

	texPostProcessInput = Texture(
		Texture::Type::POST_PROCESS_INPUT,
		graphics.GetWindowWidth(),
		graphics.GetWindowHeight(),
		GL_RGBA32F,
		GL_RGBA,
		GL_FLOAT,
		NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texPostProcessInput.id, 0);

	GLuint attachments[1] = {
		GL_COLOR_ATTACHMENT0,
	};
	glDrawBuffers(1, attachments);

	CHECK_GL_ERRORS;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("G-Buffer is not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::Render(RenderContext & context)
{
	// update view/proj for this render cycle
	context.CacheProjection();
	glm::mat4 view = context.CacheView();
	glm::vec3 viewPos(glm::inverse(view) * glm::vec4(0, 0, 0, 1));
	glm::vec3 skyColour(153/255.f, 204/255.f, 255/255.f);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBO);
	geometryPassShader.enable();
	{
		auto timer = context.StartRenderStage(RenderStage::GEOMETRY);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		glDepthMask(GL_TRUE);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// colour attachment 2 is albedo spec, clear it to a sky value
		glm::vec4 skyColourNoSpec(skyColour, 0);
		glClearBufferfv(GL_COLOR, 2, &skyColourNoSpec[0]);

		renderModels(context, geometryPassShader);
		context.EndRenderStage(timer);
	}
	geometryPassShader.disable();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ShaderProgram *shader = &shadingPassShader;
	bool shouldBindLights = true;
	bool shouldBindViewPos = true;
	DisplayMode displayMode = context.GetDisplayMode();

	if (isGBufferDebugDisplayMode(displayMode))
	{
		shader = &gBufferDebugShader;
		shouldBindLights = false;
		shouldBindViewPos = false;
	}
	else if (displayMode == DisplayMode::BRIGHTNESS)
	{
		shader = &lightDebugShader;
	}

	// everything after geometry pass should render to the post process FBO texture
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	shader->enable();
	{
		auto timer = context.StartRenderStage(RenderStage::SHADING);

		if (isGBufferDebugDisplayMode(displayMode))
		{
			glUniform1i(shader->getUniformLocation("displayMode"), (int)displayMode);
		}

		// default the post process input to sky colour
		glm::vec4 skyColourOpaque(skyColour, 1);
		glClearBufferfv(GL_COLOR, 0, &skyColourOpaque[0]);

		gBuffer.BindCoreTextures(*shader);
		if (displayMode == DisplayMode::STENCIL)
		{
			gBuffer.BindStencilTexture(*shader);
		}

		if (shouldBindViewPos)
		{
			glUniform3f(
			shader->getUniformLocation("viewPos_World"),
			viewPos.x, viewPos.y, viewPos.z);
		}

		if (shouldBindLights)
		{
			bindWorldSpaceLights(*shader, context);
		}

		renderQuad();
		gBuffer.UnbindTextures();

		context.EndRenderStage(timer);
	}
	shader->disable();

	if (context.IsRenderFeatureEnabled(RenderFeature::SCREEN_SPACE_VOLUMETRIC_LIGHTING)
		&& displayMode == DisplayMode::REGULAR)
	{
		stencilShader.enable();
		{
			auto timer = context.StartRenderStage(RenderStage::VOLUMETRIC_LIGHTING);

			glEnablei(GL_BLEND, 0); // blend on the colour buffer
			glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
			glBlendFuncSeparate(1.0, 1.0, 1.0, 1.0);

			// volumetric lighting needs to sample sky colour so bind albedo texture
			gBuffer.BindCoreTextures(stencilShader);
			gBuffer.BindStencilTexture(stencilShader);
			bindScreenSpaceLights(stencilShader, context, viewPos);

			renderQuad();
			glDisable(GL_BLEND);

			context.EndRenderStage(timer);
		}
		stencilShader.disable();
	}

	lightShader.enable();
	{
		auto timer = context.StartRenderStage(RenderStage::LIGHT_MODELS);

		for (ecs::Entity e : context.entityManager.EntitiesWith<PointLight, Transform>())
		{
			ecs::Handle<PointLight> pointLight = e.Get<PointLight>();

			// make sure we don't actually modify the transform (copy it)
			Transform transform = *e.Get<Transform>();

			// light size depends asymptotically on its intensity
			transform.Scale(1.0f - std::min(0.95f, 1.0f/(1.f + 0.05f*pointLight->intensity)));

			glUniform3fv(
			lightShader.getUniformLocation("lightColour"),
			1, &pointLight->colour[0]);

			Model & model = graphics.GetLightModel();

			model.Render(context, lightShader, transform.GetModelTransform());
		}

		context.EndRenderStage(timer);
	}
	lightShader.disable();

	// switch back to window framebuffer when we do post processing (last step)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(153/255.f, 204/255.f, 255/255.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	postProcessShader.enable();
	{
		auto timer = context.StartRenderStage(RenderStage::POST_PROCESS);

		bool gammaCorrect =
			context.IsRenderFeatureEnabled(RenderFeature::GAMMA_CORRECT)
			&& displayMode == DisplayMode::REGULAR;

		bool hdr =
			context.IsRenderFeatureEnabled(RenderFeature::HDR)
			&& displayMode == DisplayMode::REGULAR;

		glUniform1i(postProcessShader.getUniformLocation("doGammaCorrect"), gammaCorrect);
		glUniform1i(postProcessShader.getUniformLocation("doHDR"), hdr);

		texPostProcessInput.Bind(postProcessShader, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderQuad();

		context.EndRenderStage(timer);
	}
	postProcessShader.disable();


	ImGui::Render();

	glfwSwapBuffers(context.GetWindow());
}

bool Renderer::isGBufferDebugDisplayMode(DisplayMode mode) const
{
	if (
		mode == DisplayMode::NORMALS ||
		mode == DisplayMode::ALBEDO ||
		mode == DisplayMode::SPECULAR ||
		mode == DisplayMode::SHININESS ||
		mode == DisplayMode::POSITION ||
		mode == DisplayMode::STENCIL)
	{
		return true;
	}

	return false;
}

void Renderer::initQuad()
{
	GLfloat quadVertices[] = {
		// Positions		// Texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top left
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
		1.0f,   1.0f, 0.0f, 1.0f, 1.0f, // top right

		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
		1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		1.0f,   1.0f, 0.0f, 1.0f, 1.0f, // top right
	};

	GLuint quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);
	{
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
			(GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(0);
}

void Renderer::renderQuad()
{
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Renderer::renderModels(RenderContext & context, ShaderProgram & shader)
{
	for (ecs::Entity e : context.entityManager.EntitiesWith<ModelInstance, Transform>())
	{
		ecs::Handle<ModelInstance> modelInstance = e.Get<ModelInstance>();
		ecs::Handle<Transform> transform = e.Get<Transform>();

		Model & model = graphics.GetModel(modelInstance->modelId);
		model.Render(context, shader, transform->GetModelTransform());
	}
}

void Renderer::bindWorldSpaceLights(ShaderProgram & shader, RenderContext & context)
{
	uint i = 0;
	for (ecs::Entity e : context.entityManager.EntitiesWith<PointLight, Transform>())
	{
		ecs::Handle<PointLight> light = e.Get<PointLight>();
		ecs::Handle<Transform> transform = e.Get<Transform>();

		glm::vec3 pos = transform->GetPosition();
		glm::vec3 colour = light->colour;
		glm::vec2 atten = light->attenuation;

		string var = "pointLights[" + std::to_string(i) + "]";
		glUniform3f(shader.getUniformLocation(var + ".position"), pos.x, pos.y, pos.z);
		glUniform3f(shader.getUniformLocation(var + ".colour"), colour.r, colour.g, colour.b);
		glUniform1f(shader.getUniformLocation(var + ".intensity"), light->intensity);
		glUniform2f(shader.getUniformLocation(var + ".attenuation"), atten.x, atten.y);

		i++;
	}

	glUniform1i(shader.getUniformLocation("numLights"), i);
}

void Renderer::bindScreenSpaceLights(
	ShaderProgram & shader,
	RenderContext & context,
	const glm::vec3 & viewPos)
{
	glm::mat4 projView = context.GetCachedProjection() * context.GetCachedView();
	glm::vec3 viewForward = context.GetViewForward();

	uint i = 0;
	for (ecs::Entity e : context.entityManager.EntitiesWith<PointLight, Transform>())
	{
		ecs::Handle<PointLight> light = e.Get<PointLight>();
		ecs::Handle<Transform> transform = e.Get<Transform>();

		glm::vec4 worldPos = glm::vec4(transform->GetPosition(), 1);
		glm::vec4 clipSpacePos = projView * worldPos;
		glm::vec2 ndcSpacePos = glm::vec2(clipSpacePos) / clipSpacePos.w;
		glm::vec2 uvScreenSpacePos = (ndcSpacePos + glm::vec2(1, 1)) / 2.0f;
		// glm::vec3 colour = light->colour;
		// glm::vec2 atten = light->attenuation;

		// reduce the volumetric light effect as the screen position goes to infinity
		float viewAngleReduction = 1.0f / (1.0f + fabs(glm::length(ndcSpacePos)));

		glm::vec3 viewToLight = glm::vec3(worldPos) - viewPos;
		bool behindViewer = glm::dot(viewToLight, viewForward) < 0;

		string var = "pointLights[" + std::to_string(i) + "]";
		glUniform1f(shader.getUniformLocation(var + ".viewAngleReduction"), viewAngleReduction);
		glUniform2f(shader.getUniformLocation(var + ".position_Screen"), uvScreenSpacePos.x, uvScreenSpacePos.y);
		glUniform1i(shader.getUniformLocation(var + ".behindViewer"), behindViewer);
		// glUniform3f(shader.getUniformLocation(var + ".colour"), colour.r, colour.g, colour.b);
		// glUniform1f(shader.getUniformLocation(var + ".intensity"), light->intensity);
		// glUniform2f(shader.getUniformLocation(var + ".attenuation"), atten.x, atten.y);

		i++;
	}

	glUniform1i(shader.getUniformLocation("numLights"), i);
}

}