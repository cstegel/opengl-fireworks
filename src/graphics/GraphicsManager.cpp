#include "graphics/GraphicsManager.hpp"
#include "graphics/RenderContext.hpp"
#include "core/Game.hpp"

#include <imgui/imgui.h>
#include <imgui/examples/opengl3_example/imgui_impl_glfw_gl3.h>

namespace fw
{

const string GraphicsManager::LIGHT_MODEL = "ico_sphere";

GraphicsManager::GraphicsManager(Game & game) : game(game)
{
	if (!glfwInit())
	{
		throw std::runtime_error("glfwInit() failed");
	}

	renderContext = make_shared<RenderContext>(game, game.entityManager);
	renderContext->CreateWindow();

	renderer = make_shared<Renderer>(*this);

	// Setup ImGui binding.  Tell the ImGui subsystem not to
	// bother setting up its callbacks -- ours will do just fine here.
	ImGui_ImplGlfwGL3_Init(renderContext->GetWindow(), false );
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDrawCursor = false;

	ImGui_ImplGlfwGL3_NewFrame();

	loadModels();
}

GraphicsManager::~GraphicsManager()
{
	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
}

double GraphicsManager::GetRenderStageAvgTime(RenderStage stage) const
{
	return renderContext->GetRenderStageAvgTime(stage);
}

uint GraphicsManager::GetWindowWidth() const
{
	return renderContext->GetWindowWidth();
}

uint GraphicsManager::GetWindowHeight() const
{
	return renderContext->GetWindowHeight();
}

DisplayMode GraphicsManager::GetDisplayMode() const
{
	return renderContext->GetDisplayMode();
}

void GraphicsManager::SetDisplayMode(DisplayMode mode)
{
	renderContext->SetDisplayMode(mode);
}

void GraphicsManager::ToggleRenderFeature(RenderFeature feature, bool enabled)
{
	return renderContext->ToggleRenderFeature(feature, enabled);
}

bool GraphicsManager::IsRenderFeatureEnabled(RenderFeature feature) const
{
	return renderContext->IsRenderFeatureEnabled(feature);
}

void GraphicsManager::loadModels()
{
	vector<string> names = {
		GraphicsManager::LIGHT_MODEL,
		"cyborg",
		"alduin",
		"ground_stone",
		"oldhouse2",
	};

	for (string name : names)
	{
		models[name] = Model(AssetManager::GetModelPath(name, "obj"));
	}
}

Model & GraphicsManager::GetModel(string name)
{
	if (models.count(name) < 1)
	{
		throw std::runtime_error("model \"" + name + "\" is not loaded");
	}
	return models[name];
}

Model & GraphicsManager::GetLightModel()
{
	return models[GraphicsManager::LIGHT_MODEL];
}

bool GraphicsManager::Frame()
{
	if (ShouldClose()) return false;


	renderContext->UpdateWindowDimensions();

	renderer->Render(*renderContext);

	double frameEnd = glfwGetTime();
	fpsTimer += frameEnd - lastFrameEnd;
	lastFrameEnd = frameEnd;
	frameCounter++;

	if (fpsTimer > 1.0)
	{
		renderContext->SetTitle("Fireworks (" + std::to_string(frameCounter) + " FPS)");
		frameCounter = 0;
		fpsTimer = 0;
	}


	glfwPollEvents();
	ImGui_ImplGlfwGL3_NewFrame();

	return true;
}

bool GraphicsManager::ShouldClose() const
{
	return !renderContext || renderContext->ShouldClose();
}

void GraphicsManager::SetPlayerView(ecs::Entity playerView)
{
	throwIfNotValidContext();
	renderContext->SetPlayerView(playerView);
}

void GraphicsManager::BindInputCallbacks(InputManager & inputManager)
{
	throwIfNotValidContext();
	inputManager.BindCallbacks(renderContext->GetWindow());
}

void GraphicsManager::throwIfNotValidContext() const
{
	if (!renderContext)
	{
		throw std::runtime_error("a render context must be created first");
	}
}

string GraphicsManager::GetShaderPath(const string & filename)
{
	return Game::RootDir() + "/src/graphics/shaders/" + filename;
}

}