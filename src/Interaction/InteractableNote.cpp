#include "based/pch.h"
#include "InteractableNote.h"

#include "InteractionTrigger.h"
#include "Tool.h"
#include "../GameSystems.h"
#include "based/app.h"
#include "based/engine.h"
#include "based/input/keyboard.h"
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
		mDocument = uiManager.LoadWindow("DefaultNote", context);
	else
		mDocument->document->Show();

	mDocument->document->GetElementById("note-body")->SetInnerRML(mCurrentNote->mNoteText);
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
			} else
			{
				GameSystems::mToolSystem.CallOnInteract(true);
				note.mIsOpen = false;
				mDocument->document->Hide();

				GameSystems::SetPlayerMouseLookEnabled(true);
				GameSystems::SetPlayerMovementEnabled(true);
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
	}

	// Clear the current note since we are no longer looking at any notes
	mCurrentNote = nullptr;
}
