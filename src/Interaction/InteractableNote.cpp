#include "based/pch.h"
#include "InteractableNote.h"

#include "InteractionTrigger.h"
#include "Tool.h"
#include "../GameSystems.h"
#include "based/app.h"
#include "based/engine.h"
#include "based/input/keyboard.h"
#include "based/scene/entity.h"

void InteractableNoteSystem::Initialize()
{
	
}

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
	using namespace based;

	auto& uiManager = Engine::Instance().GetUiManager();
	auto context = uiManager.GetContext("main");

	uiManager.SetPathPrefix("Assets/UI/");

	if (!mDocument)
		mDocument = uiManager.LoadWindow("DefaultNote", context);
	else
		mDocument->document->Show();

	mDocument->document->GetElementById("note-body")->SetInnerRML(mCurrentNote->mNoteText);

	/*if (!context->GetDataModel("Note"))
	{
		if (Rml::DataModelConstructor constructor = context->CreateDataModel("Note"))
		{
			mDataModel = constructor.GetModelHandle();
			constructor.Bind("NoteText", &mCurrentNote->mNoteText);
			mDataModel.DirtyVariable("NoteText");
		}
	}
	else
	{
		mDataModel.DirtyVariable("NoteText");
	}*/

	BASED_TRACE("Num docs at creation: {}", uiManager.GetDocuments().size());
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
				note.mIsOpen = false;
				//uiManager.CloseWindow(*note.mDocument);
				mDocument->document->Hide();
				//note.mDocument = nullptr;

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
