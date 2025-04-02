#include "based/pch.h"
#include "InteractableNote.h"

#include "InteractionTrigger.h"
#include "Tool.h"
#include "../GameSystems.h"
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
		mDocument->document->GetElementById("back-button")->AddEventListener("click", this);
	}
	else
		mDocument->document->Show();

	mDocument->document->GetElementById("note-body")->SetInnerRML(mCurrentNote->mNoteText);

	if (mPageTurnEvent) mPageTurnEvent->start();
}

void InteractableNoteSystem::Initialize()
{
	mPageTurnEvent = FMODSystem::CreateEventW("event:/PageTurning");
}

void InteractableNoteSystem::Update(float deltaTime)
{
	using namespace based;

	auto& registry = Engine::Instance().GetApp().GetCurrentScene()->GetRegistry();
	auto view = registry.view<scene::Enabled, InteractableNote, InteractionTrigger>();
	auto scene = Engine::Instance().GetApp().GetCurrentScene();

	for (auto& e : view)
	{
		auto& note = registry.get<InteractableNote>(e);
		auto& trigger = registry.get<InteractionTrigger>(e);

		// System callbacks don't know what Note is currently being looked at
		// so we store a pointer to it here
		// But once the loop is done, we want the current note to be the one that's
		// open (if there is one), so we store a pointer to whatever note was
		// previously the current note so we can set it back when we're done
		auto lastNote = mCurrentNote;
		mCurrentNote = &note;

		// Should only trigger this once, the first time the object is hovered
		if (!trigger.mHoverEntered)
		{
			trigger.mHoverEntered = true;
			OnInteractionHoverEnter(trigger.mTool);
		}

		if (input::Keyboard::KeyDown(BASED_INPUT_KEY_E))
		{
			// Show note if it's closed, otherwise hide it
			if (!note.mIsOpen)
			{
				OnInteract(trigger.mTool);
				note.mIsOpen = true;

				GameSystems::SetPlayerMouseLookEnabled(false);
				GameSystems::SetPlayerMovementEnabled(false);
				input::Mouse::SetCursorVisible(true);
				input::Mouse::SetCursorMode(input::CursorMode::Free);
			}
		}

		if (input::Keyboard::KeyDown(BASED_INPUT_KEY_ESCAPE)
			|| input::Keyboard::KeyDown(BASED_INPUT_KEY_TAB))
		{
			if (note.mIsOpen)
			{
				GameSystems::mToolSystem.CallOnInteract(true);
				note.mIsOpen = false;
				mDocument->document->Hide();

				GameSystems::SetPlayerMouseLookEnabled(true);
				GameSystems::SetPlayerMovementEnabled(true);
				input::Mouse::SetCursorVisible(false);
				input::Mouse::SetCursorMode(input::CursorMode::Confined);

				if (mPageTurnEvent) mPageTurnEvent->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
			}
		}

		// Again, only called once when the item stops being hovered
		if (trigger.mShouldExit)
		{
			OnInteractionHoverExit(trigger.mTool);

			// Removing the trigger means this note will no longer be
			// considered when evaluating notes
			registry.remove<InteractionTrigger>(e);
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
			ToolSystem::CallOnInteract(true);
			mCurrentNote->mIsOpen = false;
			mDocument->document->Hide();

			GameSystems::SetPlayerMouseLookEnabled(true);
			GameSystems::SetPlayerMovementEnabled(true);
			based::input::Mouse::SetCursorVisible(false);
			based::input::Mouse::SetCursorMode(based::input::CursorMode::Confined);
		}
	} 
}
