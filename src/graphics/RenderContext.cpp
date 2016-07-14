#include "graphics/RenderContext.hpp"
#include "core/InputManager.hpp"

#include "ecs/components/View.hpp"
#include "ecs/components/Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace fw
{
	RenderContext::RenderContext(ecs::EntityManager & entityManager)
		: entityManager(entityManager)
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

		cachedView = glm::inverse(transformComp->GetModelTransform(*playerView.GetManager()));
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