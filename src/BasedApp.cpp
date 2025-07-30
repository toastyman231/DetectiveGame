#include "based/pch.h"
#include "based/log.h"
#include "based/main.h"
#include "based/graphics/mesh.h"
#include "based/scene/components.h"
#include "based/scene/entity.h"

#include "GameSystems.h"
#include "based/input/keyboard.h"
#include "Scenes/TitleScene.h"


using namespace based;

class BasedApp : public based::App
{
public:
	std::shared_ptr<scene::Scene> mStartupScene;

	based::core::WindowProperties GetWindowProperties() override
	{
		based::core::WindowProperties props;
		props.w = 1280;
		props.h = 720;
		props.imguiProps.IsDockingEnabled = true;
		props.title = "Detective Game";

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
		Engine::Instance().GetPhysicsManager().SetRenderDebug(false);
		Engine::Instance().GetWindow().SetFullscreen(false);

		// TODO: Set this back to TitleScreen when ready to ship
		mStartupScene = std::make_shared<TitleScene>();
		LoadScene(mStartupScene);

		// Uncomment to override render-to-screen and cursor settings
		/*Engine::Instance().GetWindow().SetShouldRenderToScreen(true);
		input::Mouse::SetCursorVisible(!Engine::Instance().GetWindow().GetShouldRenderToScreen());
		input::Mouse::SetCursorMode(Engine::Instance().GetWindow().GetShouldRenderToScreen() ?
			input::CursorMode::Confined : input::CursorMode::Free);*/

#ifdef BASED_CONFIG_DEBUG
		Rml::Debugger::Initialise(Engine::Instance().GetUiManager().GetContext("main"));
		Rml::Debugger::SetVisible(false);
#endif
	}

	void Shutdown() override
	{
		App::Shutdown();
	}

	void Update(float deltaTime) override
	{
		App::Update(deltaTime);

		/*auto position = input::Mouse::GetMousePosition();
		BASED_TRACE("MOUSE AT {} {}", position.x, position.y);*/

		if (input::Keyboard::KeyDown(BASED_INPUT_KEY_P))
		{
			GameSystems::SetPlayerMovementEnabled(false);
			GameSystems::SetPlayerMouseLookEnabled(false);
			GameSystems::mSolutionPanelSystem.SetLocked(true);

			auto doc = Engine::Instance().GetUiManager().LoadWindow("PauseMenu", 
				Engine::Instance().GetUiManager().GetContext("main"))->document;
			Rml::ElementList elements;
			doc->GetElementsByClassName(elements, "default-focus-target");
			if (!elements.empty()) elements[0]->Focus(true);
		}

		if (input::Keyboard::KeyDown(BASED_INPUT_KEY_KP_0))
		{
			Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
		}
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
