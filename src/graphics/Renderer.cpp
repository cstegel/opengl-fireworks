#include "graphics/Renderer.hpp"
#include "graphics/RenderContext.hpp"
#include "graphics/GraphicsManager.hpp"
#include "cs488/GlErrorCheck.hpp"
#include "core/AssetManager.hpp"
#include "graphics/GraphicsManager.hpp"

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
	// defaultShader.generateProgramObject();
	// defaultShader.attachVertexShader(
	// 	GraphicsManager::GetShaderPath("VertexShader.vert").c_str() );
	// defaultShader.attachFragmentShader(
	// 	GraphicsManager::GetShaderPath("FragmentShader.frag").c_str() );
	// defaultShader.link();

	normalOutputShader.generateProgramObject();
	normalOutputShader.attachVertexShader(
		GraphicsManager::GetShaderPath("LightPass.vert").c_str() );
	normalOutputShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("NormalOut.frag").c_str() );
	normalOutputShader.link();

	lightShader.generateProgramObject();
	lightShader.attachVertexShader(
		GraphicsManager::GetShaderPath("VertexShader.vert").c_str() );
	lightShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("Light.frag").c_str() );
	lightShader.link();

	geometryPassShader.generateProgramObject();
	geometryPassShader.attachVertexShader(
		GraphicsManager::GetShaderPath("GeometryPass.vert").c_str() );
	geometryPassShader.attachFragmentShader(
		GraphicsManager::GetShaderPath("GeometryPass.frag").c_str() );
	geometryPassShader.link();

	initQuad();
}

Renderer::~Renderer()
{}

void Renderer::Render(RenderContext & context)
{
	// update view/proj for this render cycle
	context.CacheProjection();
	glm::mat4 view = context.CacheView();
	glm::vec3 viewPos(view * glm::vec4(0, 0, 0, 1));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBO);
	geometryPassShader.enable();
	{
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		// glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderModels(context, geometryPassShader);
	}
	geometryPassShader.disable();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ShaderProgram *shader = &normalOutputShader;
	bool shouldBindLights = false;
	bool shouldBindViewPos = false;
	// switch (context.GetDisplayMode())
	// {
	// case DisplayMode::NORMALS:
	// 	shader = &normalOutputShader;
	// 	shouldBindLights = false;
	// 	shouldBindViewPos = false;
	// 	break;
	// default:
	// 	shader = &defaultShader;
	// 	shouldBindLights = true;
	// 	shouldBindViewPos = true;
	// 	break;
	// }

	shader->enable();
	{
		// glEnable(GL_CULL_FACE);
		// glEnable(GL_DEPTH_TEST);
		// glDepthMask(GL_TRUE);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gBuffer.BindTextures(*shader);

		if (shouldBindViewPos)
		{
			glUniform3f(
			shader->getUniformLocation("viewPos_World"),
			viewPos.x, viewPos.y, viewPos.z);
		}

		if (shouldBindLights)
		{
			bindLights(*shader, context);
		}

		renderQuad();

		gBuffer.UnbindTextures();
	}
	shader->disable();

	// lightShader.enable();
	// {
	// 	glUniformMatrix4fv(
	// 		lightShader.getUniformLocation("project"),
	// 		1, GL_FALSE, value_ptr(proj));
	//
	// 	glUniformMatrix4fv(
	// 		lightShader.getUniformLocation("view"),
	// 		1, GL_FALSE, value_ptr(view));
	//
	// 	for (ecs::Entity e : context.entityManager.EntitiesWith<PointLight, Transform>())
	// 	{
	// 		ecs::Handle<PointLight> pointLight = e.Get<PointLight>();
	//
	// 		// make sure we don't actually modify the transform (copy it)
	// 		Transform transform = *e.Get<Transform>();
	//
	// 		// light size depends asymptotically on its intensity
	// 		transform.Scale(1.0f - std::min(0.95f, 1.0f/(1.f + 0.05f*pointLight->intensity)));
	//
	// 		glUniform3fv(
	// 			lightShader.getUniformLocation("lightColour"),
	// 			1, &pointLight->colour[0]);
	//
	// 		Model & model = graphics.GetLightModel();
	//
	// 		model.Render(context, lightShader, transform.GetModelTransform(*e.GetManager()));
	// 	}
	// }
	// lightShader.disable();

	ImGui::Render();

	glfwSwapBuffers(context.GetWindow());
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
		model.Render(context, shader, transform->GetModelTransform(*e.GetManager()));
	}
}

void Renderer::bindLights(ShaderProgram & shader, RenderContext & context)
{
	uint i = 0;
	for (ecs::Entity e : context.entityManager.EntitiesWith<PointLight, Transform>())
	{
		ecs::Handle<PointLight> light = e.Get<PointLight>();
		ecs::Handle<Transform> transform = e.Get<Transform>();

		glm::vec4 pos = transform->GetModelTransform(*e.GetManager()) * glm::vec4(0, 0, 0, 1);
		glm::vec3 colour = light->colour;
		glm::vec2 atten = light->attenuation;

		string var = "pointLights[" + to_string(i) + "]";
		glUniform3f(shader.getUniformLocation(var + ".position"), pos.x, pos.y, pos.z);
		glUniform3f(shader.getUniformLocation(var + ".colour"), colour.r, colour.g, colour.b);
		glUniform1f(shader.getUniformLocation(var + ".intensity"), light->intensity);
		glUniform2f(shader.getUniformLocation(var + ".attenuation"), atten.x, atten.y);

		i++;
	}
}

}