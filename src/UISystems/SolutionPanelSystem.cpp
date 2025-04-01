#include "based/pch.h"
#include "SolutionPanelSystem.h"

#include <iostream>

#include "based/engine.h"
#include "based/input/keyboard.h"

#include "../GameSystems.h"
#include "../Systems/FMODSystem.h"
#include "based/input/mouse.h"

void ISolutionPanel::OnPanelSolved()
{
	// Set panel border to green when solved
	auto element = Rml::GetContext("main")->GetDocument("panels")->GetElementById(PanelID);
	if (!element) return;

	element->SetProperty("border-color", "#00FF00");
}

void ISolutionPanel::OnPanelIncorrect()
{
	// Set panel border to red when incorrect
	auto element = Rml::GetContext("main")->GetDocument("panels")->GetElementById(PanelID);
	if (!element) return;

	element->SetProperty("border-color", "#FF0000");
}

WordSolutionPanel::WordSolutionPanel(const std::string& id, const std::string& solutionPath)
	: ISolutionPanel(id)
{
	// Load panel solution

	std::ifstream file(solutionPath);

	if (!file.is_open())
	{
		BASED_ERROR("Failed to open Word Solution file at {}", solutionPath);
		return;
	}

	std::string line;
	int lineNumber = 0;

	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::vector<std::string> words;
		std::string word;

		while (iss >> word)
		{
			words.push_back(word);
		}

		// Allow multiple possible words for a given slot
		if (words.size() == 1)
		{
			WorkingSolution[std::to_string(lineNumber)] = 
				WordSolution{
					"",
				{{{word}}}
				};
		}
		else if (words.size() > 1)
		{
			std::vector<std::string> solutionWords;
			for (const auto& w : words)
			{
				solutionWords.emplace_back(w);
			}
			WorkingSolution[std::to_string(lineNumber)] = 
				WordSolution{
					"",
				{solutionWords}
				};
		}

		lineNumber++;
	}

	file.close();
}

bool WordSolutionPanel::CheckSolution()
{
	// Check if current solution matches any of the valid words for each slot
	for (auto& [id, solution] : WorkingSolution)
	{
		if (solution.SolutionWords != solution.CurrentWord)
		{
			BASED_WARN("Word {} does not match {}!", solution.CurrentWord, solution.SolutionWords.PossibleWords[0]);

			if (!IsAnySlotUnfilled()) OnPanelIncorrect();

			return false;
		}
	}

	return true;
}

// Track solution in panel struct (for saving and solution verification)
void WordSolutionPanel::OnSolutionElementDropped(std::string slotID, void* element)
{
	if (!element) return;

	auto elementAsWord = *static_cast<std::string*>(element);

	WorkingSolution[slotID].CurrentWord = elementAsWord;
}

// Set up panel based on current WIP solution (useful for loading saved progress)
void WordSolutionPanel::SetupPanel(Rml::ElementDocument* document)
{
	for (auto& [id, word] : WorkingSolution)
	{
		auto element = document->GetElementById(id);
		if (!element) continue;

		if (!word.CurrentWord.empty()) 
			element->SetClass("placeholder", false);
		element->SetInnerRML(word.CurrentWord);
	}
}

// TODO: Optimize this
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

bool WordSolutionPanel::IsAnySlotUnfilled()
{
	for (auto& [id, solution] : WorkingSolution)
	{
		if (solution.CurrentWord.empty()) return true;
	}

	return false;
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

	AddSolutionPanel("word-panel", 
		new WordSolutionPanel("word-panel", "Assets/Solutions/MainPanel.txt"));
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
				// Set solution slot IDs for ALL panels
				// this way they're globally unique
				element->SetId(std::to_string(index++));
			}

			// Set up panel based on cached progress, and register event callbacks
			for (auto& [id, panel] : mSolutionPanels)
			{
				if (!panel) continue;
				panel->SetupPanel(mPanelGroup->document);
				RegisterDraggableContainer(mPanelGroup->document->GetElementById(id));
			}

			// Populate word list
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
	{
		FMODSystem::PlaySound("Assets/Sounds/TestSound.wav");

		mWords.emplace_back(word);

		auto context = Rml::GetContext("main");
		auto doc = context->GetDocument("PlayerHUD");
		auto body = doc->GetElementById("words-group");

		auto element = Rml::Factory::InstanceElement(body, "word", "word", Rml::XMLAttributes());
		element->SetClass("word", true);
		SetClassByType(element, word);
		element->SetInnerRML(word.Word);
		element->AddEventListener("animationend", this);

		auto p1 = Rml::Transform::MakeProperty({ Rml::Transforms::TranslateY{-1000.f } });
		auto p2 = Rml::Transform::MakeProperty({ Rml::Transforms::TranslateY{0.f} });
		element->Animate("transform", p2, 0.6f, 
			Rml::Tween(Rml::Tween::Cubic, Rml::Tween::InOut), 
			1, false, 0.0f, &p1);

		auto k1 = Rml::Property(0, Rml::Property::Unit::NUMBER);
		element->Animate("opacity", k1, 0.6f,
			Rml::Tween(Rml::Tween::Cubic, Rml::Tween::InOut),
			1, false, 1.f);

		body->AppendChild(std::move(element));
	}
}

void SolutionPanelSystem::AddSolutionPanel(std::string panelID, ISolutionPanel* panel)
{
	mSolutionPanels[panelID] = panel;
}

void SolutionPanelSystem::ProcessEvent(Rml::Event& event)
{
	if (event == Rml::EventId::Dragstart)
	{
		based::Engine::Instance().GetWindow().SetCursor("pointer");
	}

	if (event == Rml::EventId::Dragdrop)
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

			dest->ScrollIntoView();

			if (CheckSolution())
			{
				BASED_TRACE("LEVEL SOLVED");
			}
		}
	}

	if (event == Rml::EventId::Dragover)
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

	if (event == Rml::EventId::Dragout)
	{
		based::Engine::Instance().GetWindow().SetCursor("pointer");
	}

	if (event == Rml::EventId::Dragend)
	{
		based::Engine::Instance().GetWindow().SetCursor("arrow");
	}

	if (event == Rml::EventId::Animationend)
	{
		auto element = event.GetTargetElement();
		auto prop = event.GetParameter<std::string>("property", "");

		if (!element || prop.empty()) return;

		if (prop == "opacity")
		{
			element->GetParentNode()->RemoveChild(element);
		}
	}
}

void SolutionPanelSystem::RegisterDraggableContainer(Rml::Element* element)
{
	element->AddEventListener("dragdrop", this);
	element->AddEventListener("dragover", this);
	element->AddEventListener("dragout", this);
}

void SolutionPanelSystem::SetClassByType(Rml::ElementPtr& wordElement, SolutionWord word)
{
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
}

// Create draggable word element
void SolutionPanelSystem::AddWordInternal(SolutionWord word)
{
	if (!mPanelGroup || !mPanelGroup->document) return;

	Rml::Element* content = mPanelGroup->document->GetElementById("words-box");
	if (!content) return;

	Rml::ElementPtr wordElement = Rml::Factory::InstanceElement(content, 
		"word", "word", Rml::XMLAttributes());
	wordElement->SetClass("word", true);
	wordElement->SetClass("draggable", true);

	SetClassByType(wordElement, word);

	wordElement->SetInnerRML(word.Word);
	wordElement->AddEventListener("dragstart", this);
	wordElement->AddEventListener("dragend", this);
	content->AppendChild(std::move(wordElement));
}

// Read class names until one of the valid types is found (or not)
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

// Does not escape early, since OnSolved events are dispatched from here.
// So all panels need to be checked each time.
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
