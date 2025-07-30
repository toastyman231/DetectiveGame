#pragma once

#include "MainScene.h"
#include "based/app.h"
#include "based/scene/scene.h"

class TitleScene : public based::scene::Scene, protected Rml::EventListener
{
public:
	TitleScene() : Scene("Title Scene") {}
	~TitleScene() override = default;

	void InitializeScene() override;
	void ProcessEvent(Rml::Event& event) override;
	void ShutdownScene() override;

private:
	based::managers::DocumentInfo* mDocument = nullptr;
};

inline void TitleScene::InitializeScene()
{
	Scene::InitializeScene();

	based::input::Mouse::SetCursorVisible(true);
	based::input::Mouse::SetCursorMode(based::input::CursorMode::Confined);

	auto context = based::Engine::Instance().GetUiManager().GetContext("main");
	based::Engine::Instance().GetUiManager().SetPathPrefix("Assets/UI/");
	mDocument = based::Engine::Instance().GetUiManager().LoadWindow("TitleMenu", context, "TitleMenu");

	mDocument->document->GetElementById("play")->AddEventListener(Rml::EventId::Mousedown, this);
	mDocument->document->GetElementById("quit")->AddEventListener(Rml::EventId::Mousedown, this);
}

inline void TitleScene::ProcessEvent(Rml::Event& event)
{
	if (event == Rml::EventId::Mousedown)
	{
		if (event.GetTargetElement()->GetId() == "play")
		{
			std::shared_ptr<Scene> mainScene = std::make_shared<MainScene>();
			based::Engine::Instance().GetApp().LoadScene(mainScene);
			ShutdownScene();
		}

		if (event.GetTargetElement()->GetId() == "quit")
		{
			based::Engine::Instance().Quit();
		}
	}
}

inline void TitleScene::ShutdownScene()
{
	Scene::ShutdownScene();

	if (!mDocument) return;
	based::Engine::Instance().GetUiManager().CloseWindow(*mDocument);
}
