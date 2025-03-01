#include "based/pch.h"
#include "DialogueSystem.h"

#include "../GameSystems.h"
#include "based/app.h"
#include "based/engine.h"
#include "based/input/mouse.h"
#include "based/scene/components.h"

DialogueSet::DialogueSet(const std::string& path)
{
	std::ifstream ifs(path);
	std::stringstream fileContent;

	fileContent << ifs.rdbuf();
	ifs.close();

	std::string line;
	while (std::getline(fileContent, line))
	{
		mLines.emplace(line);
	}
}

std::string DialogueSet::PopNextLine()
{
	if (mLines.empty()) return "";

	auto line = mLines.front();
	mLines.pop();

	return line;
}

/*DialogueSystem::DialogueSystem()
{
	auto& uiManager = based::Engine::Instance().GetUiManager();
	auto context = uiManager.GetContext("main");

	uiManager.SetPathPrefix("Assets/UI/");
	mDocument = uiManager.LoadWindow("DialogueBox", context);
	mDocument->document->Hide();
}*/

void DialogueSystem::Initialize()
{
	auto& uiManager = based::Engine::Instance().GetUiManager();
	auto context = uiManager.GetContext("main");

	uiManager.SetPathPrefix("Assets/UI/");
	mDocument = uiManager.LoadWindow("DialogueBox", context);
	mDocument->document->Hide();
}

void DialogueSystem::Update(float deltaTime)
{
	using namespace based;

	if (!mDocument) return;

	if (input::Mouse::ButtonDown(BASED_INPUT_MOUSE_LEFT))
	{
		ShowNextLine();
	}
}

void DialogueSystem::SetCurrentDialogue(const std::string& path)
{
	mCurrentDialogue = DialogueSet(path);
	ShowNextLine();

	GameSystems::SetPlayerMouseLookEnabled(false);
	GameSystems::SetPlayerMovementEnabled(false);
}

void DialogueSystem::CloseCurrentDialogue()
{
	mDocument->document->Hide();

	GameSystems::SetPlayerMouseLookEnabled(true);
	GameSystems::SetPlayerMovementEnabled(true);
}

void DialogueSystem::ShowNextLine()
{
	auto line = mCurrentDialogue.PopNextLine();

	if (line.empty())
	{
		CloseCurrentDialogue();
		return;
	}

	mDocument->document->GetElementById("dialogue-body")->SetInnerRML(line);

	if (!mDocument->document->IsVisible())
		mDocument->document->Show();
}
