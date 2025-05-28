#include "based/pch.h"
#include "InteractableNote.h"

#include "InteractionTrigger.h"
#include "Tool.h"
#include "../GameSystems.h"
#include "../KeyImageBindings.h"
#include "../Systems/FMODSystem.h"
#include "based/app.h"
#include "based/engine.h"
#include "based/input/keyboard.h"
#include "based/input/mouse.h"
#include "based/scene/entity.h"

// Listeners for interactable hover events
void InteractableNoteSystem::OnInteractionHoverEnter(Tool* tool)
{
	BASED_TRACE("Note hover entered!");
}

void InteractableNoteSystem::OnInteractionHoverExit(Tool* tool)
{
	BASED_TRACE("Note hover exited!");
}

void OnExitNote(InteractableNote& note, based::managers::DocumentInfo* doc, FMOD::Studio::EventInstance* event)
{
	ToolSystem::ShowInteractionUI(true);
	note.mIsOpen = false;
	doc->document->Hide();

	GameSystems::SetPlayerMouseLookEnabled(true);
	GameSystems::SetPlayerMovementEnabled(true);
	GameSystems::mSolutionPanelSystem.SetLocked(false);
	based::input::Mouse::SetCursorVisible(false);
	based::input::Mouse::SetCursorMode(based::input::CursorMode::Confined);

	if (event) event->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);

	GameSystems::mToolSystem.SetLocked(false);

	auto view = based::Engine::Instance().GetApp().GetCurrentScene()->GetRegistry().view<based::input::InputComponent>();

	for (const auto& e : view)
	{
		auto [inputComp] = view.get(e);

		based::Engine::Instance().GetInputManager().RemoveInputMapping(inputComp, "IMC_Menu");
	}
}

// Called when E is pressed in range of a note
void InteractableNoteSystem::OnInteract(Tool* tool)
{
	IInteractable::OnInteract(tool);

	using namespace based;

	auto& uiManager = Engine::Instance().GetUiManager();
	auto context = uiManager.GetContext("main");

	uiManager.SetPathPrefix("Assets/UI/");

	// Create UI document if it does not exist, otherwise just show it
	if (!mDocument)
	{
		mDocument = uiManager.LoadWindow("DefaultNote", context);
		auto backBtn = mDocument->document->GetElementById("back-button");
		backBtn->AddEventListener("click", this);
		auto backImage = mDocument->document->GetElementById("back-image");
		auto binding = ui::ElementBinding(backImage,
			[this](Rml::Element* elem)
			{
				auto input = managers::InputManager::GetInputComponentForPlayer(0);
				if (!input || !elem || !elem->IsVisible()) return;

				auto keyName = input->GetKeyImageForAction("IA_Back", mKeyMaps);

				elem->SetAttribute("sprite", keyName);
			});
		Engine::Instance().GetUiManager().AddBinding(binding);
	}
	else
		mDocument->document->Show();

	mDocument->document->GetElementById("note-body")->SetInnerRML(mCurrentNote->mNoteText);

	GameSystems::mToolSystem.SetLocked(true);

	if (mPageTurnEvent) mPageTurnEvent->start();

	mCurrentNote->mIsOpen = true;
	GameSystems::SetPlayerMouseLookEnabled(false);
	GameSystems::SetPlayerMovementEnabled(false);
	GameSystems::mSolutionPanelSystem.SetLocked(true);
	input::Mouse::SetCursorVisible(true);
	input::Mouse::SetCursorMode(input::CursorMode::Free);

	auto view = Engine::Instance().GetApp().GetCurrentScene()->GetRegistry().view<input::InputComponent>();

	for (const auto& e : view)
	{
		auto [inputComp] = view.get(e);

		Engine::Instance().GetInputManager().AddInputMapping(inputComp, "IMC_Menu", 0);
	}
}

void InteractableNoteSystem::Initialize()
{
	mPageTurnEvent = FMODSystem::CreateFMODEvent("event:/PageTurning");

	auto view = based::Engine::Instance().GetApp().GetCurrentScene()->GetRegistry().view<based::input::InputComponent>();

	for (const auto& e : view)
	{
		auto [inputComp] = view.get(e);

		inputComp.mCompletedEvent.sink<based::input::InputAction>().connect<&InteractableNoteSystem::OnInput>(this);
	}
}

void InteractableNoteSystem::Update(float deltaTime)
{
	using namespace based;

	auto& registry = Engine::Instance().GetApp().GetCurrentScene()->GetRegistry();
	auto view = registry.view<scene::Enabled, InteractableNote, Interactable>();
	auto scene = Engine::Instance().GetApp().GetCurrentScene();

	for (auto& e : view)
	{
		auto& note = registry.get<InteractableNote>(e);
		auto& interactable = registry.get<Interactable>(e);

		// System callbacks don't know what Note is currently being looked at
		// so we store a pointer to it here
		// But once the loop is done, we want the current note to be the one that's
		// open (if there is one), so we store a pointer to whatever note was
		// previously the current note so we can set it back when we're done
		auto lastNote = mCurrentNote;
		mCurrentNote = &note;

		// Should only trigger this once, the first time the object is hovered
		if (interactable.tags.HasTag(core::Tag("Interaction.Hover")))
		{
			OnInteractionHoverEnter(interactable.tool);
			interactable.tags.RemoveTag(core::Tag("Interaction.Hover"));
		}

		if (interactable.tags.HasTag(core::Tag("Interaction.Interact")))
		{
			if (!note.mIsOpen)
				OnInteract(interactable.tool);
			interactable.tags.RemoveTag(core::Tag("Interaction.Interact"));
		}

		if (interactable.tags.HasTag(core::Tag("Interaction.Unhover")))
		{
			OnInteractionHoverExit(interactable.tool);
			interactable.tags.RemoveTag(core::Tag("Interaction.Unhover"));
		}

		if (note.mIsOpen) mCurrentNote = &note;
		else mCurrentNote = lastNote;
	}
}

void InteractableNoteSystem::ProcessEvent(Rml::Event& event)
{
	if (event.GetType() == "click")
	{
		if (mCurrentNote->mIsOpen)
		{
			if (!mCurrentNote) return;
			OnExitNote(*mCurrentNote, mDocument, mPageTurnEvent);
		}
	} 
}

void InteractableNoteSystem::OnInput(const based::input::InputAction& action)
{
	if (action.name == "IA_Back" && mCurrentNote)
	{
		if (mCurrentNote->mIsOpen)
		{
			OnExitNote(*mCurrentNote, mDocument, mPageTurnEvent);
		}
	}
}
