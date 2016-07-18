#include "graphics/RenderContext.hpp"
#include "core/InputManager.hpp"
#include "core/Game.hpp"

#include "ecs/components/View.hpp"
#include "ecs/components/Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace fw
{

RenderContext::Timer::Timer(RenderStage stage)
	: stage(stage)
{
	startTime = glfwGetTime();
}

RenderContext::RenderContext(Game & game, ecs::EntityManager & entityManager)
	: game(game), entityManager(entityManager)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	// enable all render features
	activeRenderFeatures.set();
	displayMode = DisplayMode::REGULAR;
	currentRenderStage = RenderStage::NOT_RENDERING;
}

RenderContext::~RenderContext()
{
	if (window)
	{
		glfwDestroyWindow(window);
	}
}

DisplayMode RenderContext::GetDisplayMode() const
{
	return displayMode;
}

void RenderContext::SetDisplayMode(DisplayMode mode)
{
	displayMode = mode;
}

void RenderContext::ToggleRenderFeature(RenderFeature feature, bool enabled)
{
	activeRenderFeatures.set(static_cast<size_t>(feature), enabled);
}

bool RenderContext::IsRenderFeatureEnabled(RenderFeature feature) const
{
	return activeRenderFeatures.test(static_cast<size_t>(feature));
}

void RenderContext::CreateWindow()
{
	window = glfwCreateWindow(windowWidth, windowHeight, "Fireworks", nullptr, nullptr);
	if (!window)
	{
		throw std::runtime_error("glfw window creation failed");
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		throw std::runtime_error("glewInit failed");
	}

	// reposition window in center of screen
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwSetWindowPos(
		window,
		(mode->width - windowWidth) / 2,
		(mode->height - windowHeight) / 2);

	// clear any possible GL error status
	while(glGetError());
}

int RenderContext::GetWindowWidth() const
{
	if (!window)
	{
		throw std::runtime_error("no window is active");
	}
	return windowWidth;
}

int RenderContext::GetWindowHeight() const
{
	if (!window)
	{
		throw std::runtime_error("no window is active");
	}
	return windowHeight;
}

void RenderContext::UpdateWindowDimensions()
{
	if (!window)
	{
		throw std::runtime_error("no window is active");
	}
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
}

bool RenderContext::ShouldClose() const
{
	return !window || glfwWindowShouldClose(window);
}

void RenderContext::SetTitle(string title)
{
	glfwSetWindowTitle(window, title.c_str());
}

void RenderContext::BindInputCallbacks(InputManager &inputManager)
{
	inputManager.BindCallbacks(window);
}

GLFWwindow *RenderContext::GetWindow() const
{
	return window;
}

void RenderContext::SetPlayerView(ecs::Entity playerView)
{
	validateView(playerView);
	this->playerView = playerView;
}

ecs::Entity RenderContext::GetPlayerView() const
{
	validateView(playerView);
	return playerView;
}

const glm::mat4 & RenderContext::CacheView()
{
	ecs::Entity playerView = GetPlayerView();
	ecs::Handle<Transform> transformComp = playerView.Get<Transform>();

	cachedView = glm::inverse(transformComp->GetModelTransform());
	return cachedView;
}

const glm::mat4 & RenderContext::GetCachedView() const
{
	return cachedView;
}

const glm::mat4 & RenderContext::CacheProjection()
{
	ecs::Entity playerView = GetPlayerView();
	ecs::Handle<View> viewComp = playerView.Get<View>();

	cachedProjection = glm::perspective(
		viewComp->fovy, AspectRatio(), viewComp->near, viewComp->far);

	return cachedProjection;
}

const glm::mat4 & RenderContext::GetCachedProjection() const
{
	return cachedProjection;
}

const glm::vec3 RenderContext::GetViewForward() const
{
	ecs::Entity playerView = GetPlayerView();
	ecs::Handle<Transform> viewTransform = playerView.Get<Transform>();

	return viewTransform->GetForward(game.GetWorldForward());
}

RenderStage RenderContext::GetRenderStage() const
{
	return currentRenderStage;
}

RenderContext::Timer RenderContext::StartRenderStage(RenderStage stage)
{
	currentRenderStage = stage;
	return RenderContext::Timer(stage);
}

double RenderContext::GetRenderStageAvgTime(RenderStage stage) const
{
	if (renderTimes.count(stage) < 1) return 0.0;
	return renderTimes.at(stage);
}

void RenderContext::EndRenderStage(RenderContext::Timer & timer)
{
	double dt = glfwGetTime() - timer.startTime;

	if (renderTimes.count(timer.stage) < 1)
	{
		renderTimes[timer.stage] = dt;
	}
	else
	{
		// exponential moving average
		renderTimes[timer.stage] = renderTimes[timer.stage] * 0.9 + dt * 0.1;
	}
}

void RenderContext::validateView(ecs::Entity playerView) const
{
	if (!playerView.Has<View>())
	{
		throw std::runtime_error("entity must have a View component");
	}

	if (!playerView.Has<Transform>())
	{
		throw std::runtime_error("entity must have a Transform component");
	}
}

float RenderContext::AspectRatio() const
{
	return (float)windowWidth / (float)windowHeight;
}

}