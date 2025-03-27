#include "based/pch.h"
#include "SolutionPanelSystem.h"

#include "based/engine.h"
#include "based/input/keyboard.h"

#include "../GameSystems.h"
#include "based/input/mouse.h"

void SolutionPanelSystem::Initialize()
{
	AddWord({ "Shadow", WordType::Person });
	AddWord({ "Assistant", WordType::Person });
	AddWord({ "Scientist", WordType::Person });
	AddWord({ "Entered", WordType::Verb });
	AddWord({ "Exited", WordType::Verb });
	AddWord({ "Picked", WordType::Verb });
	AddWord({ "Broken", WordType::Verb });
	AddWord({ "Moved", WordType::Verb });
	AddWord({ "Swapped", WordType::Verb });
	AddWord({ "Caught", WordType::Verb });
	AddWord({ "Fought", WordType::Verb });
	AddWord({ "Door", WordType::Place });
	AddWord({ "Window", WordType::Place });
	AddWord({ "Lock", WordType::Thing });
	AddWord({ "Glass", WordType::Thing });
	AddWord({ "Teleswapper", WordType::Thing });
	AddWord({ "Knife", WordType::Thing });
	AddWord({ "Tools", WordType::Thing });
	AddWord({ "2", WordType::Number });
}

void SolutionPanelSystem::Update(float deltaTime)
{
	if (based::input::Keyboard::KeyDown(BASED_INPUT_KEY_TAB))
	{
		if (mIsPanelGroupShown)
		{
			based::Engine::Instance().GetUiManager().CloseWindow(*mPanelGroup);

			GameSystems::SetPlayerMouseLookEnabled(true);
			GameSystems::SetPlayerMovementEnabled(true);
			based::input::Mouse::SetCursorVisible(false);
			based::input::Mouse::SetCursorMode(based::input::CursorMode::Confined);

			mIsPanelGroupShown = false;
		} else
		{
			auto context = based::Engine::Instance().GetUiManager().GetContext("main");
			mPanelGroup = based::Engine::Instance().GetUiManager().LoadWindow("SolutionScreen", context);

			for (auto& word : mWords)
			{
				AddWordInternal(word);
			}

			GameSystems::SetPlayerMouseLookEnabled(false);
			GameSystems::SetPlayerMovementEnabled(false);
			based::input::Mouse::SetCursorVisible(true);
			based::input::Mouse::SetCursorMode(based::input::CursorMode::Free);

			mIsPanelGroupShown = true;
		}
	}
}

void SolutionPanelSystem::AddWord(SolutionWord word)
{
	 //if (std::find(mWords.begin(), mWords.end(), word) == mWords.end()) 
	mWords.emplace_back(word);
}

void SolutionPanelSystem::AddWordInternal(SolutionWord word)
{
	if (!mPanelGroup || !mPanelGroup->document) return;

	Rml::Element* content = mPanelGroup->document->GetElementById("words-box");
	if (!content) return;

	Rml::ElementPtr wordElement = Rml::Factory::InstanceElement(content, 
		"word", "word", Rml::XMLAttributes());
	wordElement->SetClass("word", true);
	wordElement->SetClass("draggable", true);

	switch (word.Type)
	{
	case WordType::Person:
		wordElement->SetClass("person", true);
		break;
	case WordType::Place:
		wordElement->SetClass("place", true);
		break;
	case WordType::Number:
		wordElement->SetClass("number", true);
		break;
	default:
		break;
	}

	wordElement->SetInnerRML(word.Word);
	content->AppendChild(std::move(wordElement));
}
