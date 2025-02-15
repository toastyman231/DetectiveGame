#include "based/pch.h"
#include "based/log.h"
#include "based/main.h"
#include "based/graphics/mesh.h"
#include "based/scene/components.h"
#include "based/input/mouse.h"
#include "based/scene/entity.h"

using namespace based;

class BasedApp : public based::App
{
private:
	std::shared_ptr<scene::Entity> iconEntity;
public:
	core::WindowProperties GetWindowProperties() override
	{
		core::WindowProperties props;
		props.w = 1280;
		props.h = 720;
		props.imguiProps.IsDockingEnabled = true;

		return props;
	}

	based::GameSettings GetGameSettings() override
	{
		based::GameSettings settings;
		settings.gameMemory = 1;

		return settings;
	}

	void Initialize() override
	{
		App::Initialize();
		Engine::Instance().GetWindow().SetShouldRenderToScreen(true);
		input::Mouse::SetCursorVisible(!Engine::Instance().GetWindow().GetShouldRenderToScreen());
		input::Mouse::SetCursorMode(Engine::Instance().GetWindow().GetShouldRenderToScreen() ?
			input::CursorMode::Confined : input::CursorMode::Free);

		scene::Scene::LoadScene(ASSET_PATH("Scenes/Default3D.bscn"));
	}

	void Shutdown() override
	{
		App::Shutdown();
	}

	void Update(float deltaTime) override
	{
		App::Update(deltaTime);
	}

	void Render() override
	{
		App::Render();
	}

	void ImguiRender() override
	{
	}
};

based::App* CreateApp()
{
	return new BasedApp();
}