#include "graphics/Renderer.hpp"
#include "graphics/RenderContext.hpp"
#include "graphics/GraphicsManager.hpp"
#include "cs488/GlErrorCheck.hpp"
#include "core/AssetManager.hpp"
#include "graphics/GraphicsManager.hpp"
#include "graphics/RenderStage.hpp"
#include "core/Game.hpp"

#include "ecs/components/View.hpp"
#include "ecs/components/Transform.hpp"
#include "ecs/components/ModelInstance.hpp"
#include "ecs/components/PointLight.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <glm/ext.hpp>
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

	copyTextureShader.generateProgramObject();
	copyTextureShader.attachVertexShader(
		GraphicsManager::GetShaderPath("ShadingPass.vert").c_str() );
	copyTextureShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("CopyTexture.frag").c_str() );
	copyTextureShader.link();

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

	nullShader.generateProgramObject();
	nullShader.attachVertexShader(
		GraphicsManager::GetShaderPath("ShadingPass.vert").c_str() );
	nullShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("Null.frag").c_str() );
	nullShader.link();

	shadowMapShader.generateProgramObject();
	shadowMapShader.attachVertexShader(
		GraphicsManager::GetShaderPath("ShadowMap.vert").c_str() );
	shadowMapShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("ShadowMap.frag").c_str() );
	shadowMapShader.link();

	shadowMapDebugShader.generateProgramObject();
	shadowMapDebugShader.attachVertexShader(
		GraphicsManager::GetShaderPath("ShadingPass.vert").c_str() );
	shadowMapDebugShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("ShadowMapDebug.frag").c_str() );
	shadowMapDebugShader.link();

	initPostProcessFBO();
	initLightModelPassFBO();
	initTempMixFBO();
	initShadowMaps();

	initQuad();
}

Renderer::~Renderer()
{}

void Renderer::initShadowMaps()
{
	for (Renderer::ShadowMap & shadowMap : shadowMaps)
	{
		glGenFramebuffers(1, &shadowMap.FBO);
		glGenTextures(1, &shadowMap.texDepth);
		glBindTexture(GL_TEXTURE_2D, shadowMap.texDepth);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			Renderer::SHADOW_WIDTH, Renderer::SHADOW_HEIGHT, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// bind texture as depth buff, explicitly say there's no colour attachments (necessary)
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap.texDepth, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

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

// should clean this up by splitting into a class
void Renderer::initLightModelPassFBO()
{
	glGenFramebuffers(1, &lightModelPassFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lightModelPassFBO);

	texLightModelColour = Texture(
		Texture::Type::UNKNOWN,
		graphics.GetWindowWidth(),
		graphics.GetWindowHeight(),
		GL_RGBA32F,
		GL_RGBA,
		GL_FLOAT,
		NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texLightModelColour.id, 0);

	GLuint attachments[1] = {
		GL_COLOR_ATTACHMENT0,
	};
	glDrawBuffers(1, attachments);

	GLuint lightModelFBODepthBuff = 0;
	glGenTextures(1, &lightModelFBODepthBuff);
	glBindTexture(GL_TEXTURE_2D, lightModelFBODepthBuff);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT,
		graphics.GetWindowWidth(),
		graphics.GetWindowHeight(),
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, lightModelFBODepthBuff, 0);

	CHECK_GL_ERRORS;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("G-Buffer is not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// should clean this up by splitting into a class
void Renderer::initTempMixFBO()
{
	glGenFramebuffers(1, &tempMixFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, tempMixFBO);

	texTempMixColour = Texture(
		Texture::Type::UNKNOWN,
		graphics.GetWindowWidth(),
		graphics.GetWindowHeight(),
		GL_RGBA32F,
		GL_RGBA,
		GL_FLOAT,
		NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texTempMixColour.id, 0);

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
	glm::vec3 skyColour(0, 0, 0);
	// glm::vec3 skyColour(30/255.f, 30/255.f, 30/255.f);

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

		// default the stencil texture to sky colour since it will be used for volumetric lighting
		glm::vec4 skyColourOpaque(skyColour, 1);
		glClearBufferfv(GL_COLOR, 3, &skyColourOpaque[0]);

		renderModels(context, geometryPassShader);
		context.EndRenderStage(timer);
	}
	geometryPassShader.disable();

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
	else if (displayMode == DisplayMode::VOLUMETRIC_LIGHTING)
	{
		// don't do any normal shading
		shader = &nullShader;
		shouldBindLights = false;
		shouldBindViewPos = false;
	}
	else if (displayMode == DisplayMode::SHADOW_MAP_DEPTH)
	{
		shader = &shadowMapDebugShader;
		shouldBindViewPos = false;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glClearColor(skyColour.r, skyColour.g, skyColour.b, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	shader->enable();
	{
		auto timer = context.StartRenderStage(RenderStage::SHADING);

		if (isGBufferDebugDisplayMode(displayMode))
		{
			glUniform1i(shader->getUniformLocation("displayMode"), (int)displayMode);
		}


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

		// may need multiple draw calls in order to calculate shading for all lights
		auto lightEntities = context.entityManager.EntitiesWith<PointLight, Transform>();
		auto lightIterator = lightEntities.begin();
		auto lastLightIterator = lightEntities.end();

		// first render pass, overwrite the sky
		glDisable(GL_BLEND);
		while (true)
		{
			if (shouldBindLights)
			{
				uint i = 0;
				for (;
				     i < RenderContext::MAX_SHADER_POINT_LIGHTS && lightIterator != lastLightIterator;
				     ++i, ++lightIterator)
				{
					generateShadowMap(*lightIterator, i, context, postProcessFBO);
					bindWorldSpaceLight(*lightIterator, i, *shader, context);
				}

				glUniform1i(shader->getUniformLocation("numLights"), i);
			}

			renderQuad();

			if (!shouldBindLights || lightIterator == lastLightIterator)
			{
				break;
			}

			// if there's more lights to do another pass, blend the result in
			glEnablei(GL_BLEND, 0);
			glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
			glBlendFuncSeparate(1.0, 1.0, 1.0, 1.0);
		}

		glDisable(GL_BLEND);

		gBuffer.UnbindTextures();

		context.EndRenderStage(timer);
	}
	shader->disable();


	// need to copy the depth buffer from the gBuffer FBO to the light model FBO
	// so that lights can be drawn behind stuff
	glBlitNamedFramebuffer(
		gBuffer.FBO, lightModelPassFBO,
		0, 0, context.GetWindowWidth(), context.GetWindowHeight(),
		0, 0, context.GetWindowWidth(), context.GetWindowHeight(),
		GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);

	glBindFramebuffer(GL_FRAMEBUFFER, lightModelPassFBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	lightShader.enable();
	{
		auto timer = context.StartRenderStage(RenderStage::LIGHT_MODELS);

		for (ecs::Entity e : context.entityManager.EntitiesWith<PointLight, Transform>())
		{
			ecs::Handle<PointLight> pointLight = e.Get<PointLight>();

			// make sure we don't actually modify the transform (copy it)
			Transform transform = *e.Get<Transform>();

			// light size depends asymptotically on its intensity
			transform.Scale(pointLight->GetRadius());

			glUniform3fv(
			lightShader.getUniformLocation("lightColour"),
			1, &pointLight->colour[0]);

			Model & model = graphics.GetLightModel();

			model.Render(context, lightShader, transform.GetModelTransform());
		}

		context.EndRenderStage(timer);
	}
	lightShader.disable();


	if (displayMode == DisplayMode::REGULAR || displayMode == DisplayMode::VOLUMETRIC_LIGHTING)
	{
		if (context.IsRenderFeatureEnabled(RenderFeature::SCREEN_SPACE_VOLUMETRIC_LIGHTING))
		{
			// must mix together the object outlines from the gbuffer with the light models
			mixOutlineWithLightModelsToTempTexture();

			// add the volumetric lighting colour to the post process FBO which
			// already has the shading output
			glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
			glEnablei(GL_BLEND, 0);
			glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
			glBlendFuncSeparate(1.0, 1.0, 1.0, 1.0);

			stencilShader.enable();
			{
				auto timer = context.StartRenderStage(RenderStage::VOLUMETRIC_LIGHTING);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texTempMixColour.id);
				glUniform1i(stencilShader.getUniformLocation("texColour"), 0);

				// may need multiple draw calls in order to calculate
				// volumetric lighting for all lights
				auto lightEntities = context.entityManager.EntitiesWith<PointLight, Transform>();
				auto lightIterator = lightEntities.begin();
				auto lastLightIterator = lightEntities.end();

				while (true)
				{
					uint i = 0;
					for (;
					i < RenderContext::MAX_SHADER_POINT_LIGHTS && lightIterator != lastLightIterator;
					++i, ++lightIterator)
					{
						bindScreenSpaceLight(*lightIterator, i, stencilShader, context, viewPos);
					}

					glUniform1i(stencilShader.getUniformLocation("numLights"), i);

					renderQuad();

					if (lightIterator == lightEntities.end())
					{
						break;
					}
				}

				context.EndRenderStage(timer);
			}

			glDisable(GL_BLEND);

			stencilShader.disable();
		}
		// copy the light models to the post process colour input since the
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
			glEnablei(GL_BLEND, 0);
			glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
			glBlendFuncSeparate(1.0, 1.0, 1.0, 1.0);

			copyTextureShader.enable();
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, gBuffer.texStencil.id);
				glUniform1i(copyTextureShader.getUniformLocation("texToCopy"), 0);

				renderQuad();

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texLightModelColour.id);
				glUniform1i(copyTextureShader.getUniformLocation("texToCopy"), 0);

				renderQuad();
			}
			copyTextureShader.disable();

			glDisable(GL_BLEND);
		}
	}

	// switch back to window framebuffer when we do post processing (last step)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		glClearColor(0,0,0,0);
		renderQuad();

		context.EndRenderStage(timer);
	}
	postProcessShader.disable();


	ImGui::Render();

	glfwSwapBuffers(context.GetWindow());
}

void Renderer::mixOutlineWithLightModelsToTempTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, tempMixFBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// light models were already rendered with depth info so we can just add the two
	// colour textures together (object outlines + light models)
	glEnablei(GL_BLEND, 0);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
	glBlendFuncSeparate(1.0, 1.0, 1.0, 1.0);

	copyTextureShader.enable();
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBuffer.texStencil.id);
		glUniform1i(copyTextureShader.getUniformLocation("texToCopy"), 0);

		renderQuad();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texLightModelColour.id);
		glUniform1i(copyTextureShader.getUniformLocation("texToCopy"), 0);

		renderQuad();
	}
	copyTextureShader.disable();

	glDisable(GL_BLEND);
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

void Renderer::generateShadowMap(ecs::Entity lightEnt, uint bindIndex, RenderContext & context, GLuint rebindFBO)
{
	ShadowMap & shadowMap = shadowMaps.at(bindIndex);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.FBO);
	// float infDepth = 1.0f;
	// glClearNamedFramebufferfv(shadowMap.FBO, GL_DEPTH, 0, &infDepth);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, Renderer::SHADOW_WIDTH, Renderer::SHADOW_HEIGHT);
	shadowMapShader.enable();
	{
		glm::vec3 worldUp = graphics.game.GetWorldUp();
		glm::mat4 worldToLight = glm::inverse(lightEnt.Get<Transform>()->GetModelTransform());
		glm::mat4 lookDown = glm::lookAt(glm::vec3(0, 0, 0), -worldUp, worldUp);
		glm::mat4 lightPerspective = glm::perspective(
			glm::radians(45.0f), 1.0f, 0.1f, 1000.f
		);

		shadowMap.worldToLightMat = lightPerspective * lookDown * worldToLight;

		glUniformMatrix4fv(
		shadowMapShader.getUniformLocation("worldToLightMat"), 1, GL_FALSE,
			glm::value_ptr(shadowMap.worldToLightMat));

		renderModels(context, shadowMapShader);
	}
	shadowMapShader.disable();

	// reset glViewport
	glViewport(0, 0, context.GetWindowWidth(), context.GetWindowHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, rebindFBO);
}

void Renderer::bindWorldSpaceLight(
	ecs::Entity lightEnt,
	uint lightIndex,
	ShaderProgram & shader,
	RenderContext & context)
{
	ecs::Handle<PointLight> light = lightEnt.Get<PointLight>();
	ecs::Handle<Transform> transform = lightEnt.Get<Transform>();

	glm::vec3 pos = transform->GetPosition();
	glm::vec3 colour = light->colour;
	glm::vec2 atten = light->attenuation;

	ShadowMap & shadowMap = shadowMaps.at(lightIndex);


	string var = "pointLights[" + std::to_string(lightIndex) + "]";
	glUniform3f(shader.getUniformLocation(var + ".position"), pos.x, pos.y, pos.z);
	glUniform3f(shader.getUniformLocation(var + ".colour"), colour.r, colour.g, colour.b);
	glUniform1f(shader.getUniformLocation(var + ".intensity"), light->intensity);
	glUniform2f(shader.getUniformLocation(var + ".attenuation"), atten.x, atten.y);
	glUniformMatrix4fv(shader.getUniformLocation(var + ".worldToLightMat", true),
		1, GL_FALSE, glm::value_ptr(shadowMap.worldToLightMat));

	// start at 3 because other gBuffer textures bind 0 - 2
	glActiveTexture(GL_TEXTURE3 + lightIndex);
	glBindTexture(GL_TEXTURE_2D, shadowMap.texDepth);
	glUniform1i(
		shader.getUniformLocation(var + ".texShadowDepth", true),
		3 + lightIndex
	);
}

void Renderer::bindScreenSpaceLight(
	ecs::Entity lightEnt,
	uint bindIndex,
	ShaderProgram & shader,
	RenderContext & context,
	const glm::vec3 & viewPos)
{
	glm::mat4 projView = context.GetCachedProjection() * context.GetCachedView();
	glm::vec3 viewForward = context.GetViewForward();

	ecs::Handle<PointLight> light = lightEnt.Get<PointLight>();
	ecs::Handle<Transform> transform = lightEnt.Get<Transform>();

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


	string var = "pointLights[" + std::to_string(bindIndex) + "]";
	glUniform1f(shader.getUniformLocation(var + ".viewAngleReduction"), viewAngleReduction);
	glUniform2f(shader.getUniformLocation(var + ".position_Screen"), uvScreenSpacePos.x, uvScreenSpacePos.y);
	glUniform1i(shader.getUniformLocation(var + ".behindViewer"), behindViewer);
	// glUniform3f(shader.getUniformLocation(var + ".colour"), colour.r, colour.g, colour.b);
	// glUniform1f(shader.getUniformLocation(var + ".intensity"), light->intensity);
	// glUniform2f(shader.getUniformLocation(var + ".attenuation"), atten.x, atten.y);

}

}