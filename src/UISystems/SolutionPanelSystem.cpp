#include "based/pch.h"
#include "SolutionPanelSystem.h"

#include <iostream>

#include "based/engine.h"
#include "based/input/keyboard.h"

#include "../GameSystems.h"
#include "based/input/mouse.h"

void ISolutionPanel::OnPanelSolved()
{
	BASED_TRACE("PANEL SOLVED");
	auto element = Rml::GetContext("main")->GetDocument("panels")->GetElementById(PanelID);
	if (!element) return;

	element->SetProperty("border-color", "#00FF00");
}

bool WordSolutionPanel::CheckSolution()
{
	// TODO
	return true;
}

void WordSolutionPanel::OnSolutionElementDropped(std::string slotID, void* element)
{
	if (!element) return;

	auto elementAsWord = *static_cast<std::string*>(element);

	BASED_TRACE("DROPPING {}", elementAsWord);
	WorkingSolution[slotID] = elementAsWord;
}

void WordSolutionPanel::SetupPanel(Rml::ElementDocument* document)
{
	for (auto& [id, word] : WorkingSolution)
	{
		auto element = document->GetElementById(id);
		if (!element) continue;

		element->SetClass("placeholder", false);
		element->SetInnerRML(word);
	}
}

bool WordSolutionPanel::CanDropElement(Rml::Element* target, Rml::Element* drag_element)
{
	if (IsLocked) return false;
	if (!target || !drag_element) return false;
	if (!target->GetOwnerDocument() || !drag_element->GetOwnerDocument()) return false;

	std::string dragClasses = drag_element->GetClassNames();
	std::string destClasses = target->GetClassNames();

	std::string wordType = SolutionPanelSystem::GetWordTypeFromClass(dragClasses);
	std::string destType = SolutionPanelSystem::GetWordTypeFromClass(destClasses);

	drag_element->SetInnerRML(drag_element->GetInnerRML()); // Required to prevent a crash for some reason
	return wordType == destType;
}

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

	AddSolutionPanel("word-panel", new WordSolutionPanel("word-panel"));
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
			mPanelGroup = based::Engine::Instance().GetUiManager().LoadWindow("SolutionScreen", context, "panels");

			Rml::ElementList placeholders;
			mPanelGroup->document->GetElementsByClassName(placeholders, "placeholder");
			uint32_t index = 0;
			for (auto element : placeholders)
			{
				element->SetId(std::to_string(index++));
			}

			for (auto& [id, panel] : mSolutionPanels)
			{
				if (!panel) continue;
				panel->SetupPanel(mPanelGroup->document);
			}

			RegisterDraggableContainer(mPanelGroup->document->GetElementById("word-panel"));

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
	if (std::find(mWords.begin(), mWords.end(), word) == mWords.end()) 
		mWords.emplace_back(word);
}

void SolutionPanelSystem::AddSolutionPanel(std::string panelID, ISolutionPanel* panel)
{
	mSolutionPanels[panelID] = panel;
}

void SolutionPanelSystem::ProcessEvent(Rml::Event& event)
{
	if (event == "dragstart")
	{
		based::Engine::Instance().GetWindow().SetCursor("pointer");
	}

	if (event == "dragdrop")
	{
		Rml::Element* dest = event.GetTargetElement();
		Rml::Element* drag_element = static_cast<Rml::Element*>(event.GetParameter<void*>("drag_element", nullptr));
		
		if (!dest || !drag_element) return;
		if (!dest->GetOwnerDocument() || !drag_element->GetOwnerDocument()) return;

		auto panel = mSolutionPanels[dest->GetParentNode()->GetId()];
		if (!panel) return;

		if (panel->CanDropElement(dest, drag_element))
		{
			std::string dragWord = drag_element->GetInnerRML();
			dest->SetClass("placeholder", false);
			dest->SetInnerRML(dragWord);

			panel->OnSolutionElementDropped(dest->GetId(), &dragWord);

			if (CheckSolution())
			{
				BASED_TRACE("LEVEL SOLVED");
			}
		}
	}

	if (event == "dragover")
	{
		Rml::Element* dest = event.GetTargetElement();
		Rml::Element* drag_element = static_cast<Rml::Element*>(event.GetParameter<void*>("drag_element", nullptr));

		if (!dest || !drag_element) return;
		if (!dest->GetOwnerDocument() || !drag_element->GetOwnerDocument()) return;

		auto panel = mSolutionPanels[dest->GetParentNode()->GetId()];
		if (!panel) return;

		if (!panel->CanDropElement(dest, drag_element))
		{
			based::Engine::Instance().GetWindow().SetCursor("unavailable");
		}
	}

	if (event == "dragout")
	{
		based::Engine::Instance().GetWindow().SetCursor("pointer");
	}

	if (event == "dragend")
	{
		based::Engine::Instance().GetWindow().SetCursor("arrow");
	}
}

void SolutionPanelSystem::RegisterDraggableContainer(Rml::Element* element)
{
	element->AddEventListener("dragdrop", this);
	element->AddEventListener("dragover", this);
	element->AddEventListener("dragout", this);
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
	case WordType::Thing:
		wordElement->SetClass("thing", true);
		break;
	case WordType::Verb:
		wordElement->SetClass("verb", true);
		break;
	case WordType::Number:
		wordElement->SetClass("number", true);
		break;
	}

	wordElement->SetInnerRML(word.Word);
	wordElement->AddEventListener("dragstart", this);
	wordElement->AddEventListener("dragend", this);
	content->AppendChild(std::move(wordElement));
}

std::string SolutionPanelSystem::GetWordTypeFromClass(const std::string& classNames)
{
	static const std::array<std::string, 5> word_types = { "person", "place", "thing", "verb", "number" };

	std::istringstream is(classNames);
	std::string word;

	while (is >> word)
	{
		if (std::find(word_types.begin(), word_types.end(), word) != word_types.end())
		{
			return word;
		}
	}

	return "NONE";
}

bool SolutionPanelSystem::CheckSolution()
{
	bool didAnyFail = false;
	for (auto& [id, panel] : mSolutionPanels)
	{
		if (!panel) continue;

		if (panel->CheckSolution())
		{
			if (!panel->IsLocked) panel->OnPanelSolved();
			panel->IsLocked = true;
		}
		else didAnyFail = true;
	}

	return !didAnyFail;
}
