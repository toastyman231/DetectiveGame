#include "based/pch.h"
#include "InteractableNote.h"

#include "InteractionTrigger.h"
#include "Tool.h"
#include "../GameSystems.h"
#include "../Player/MouseLook.h"
#include "based/app.h"
#include "based/engine.h"
#include "based/input/keyboard.h"
#include "based/scene/entity.h"

void InteractableNoteSystem::OnInteractionHoverEnter(Tool* tool)
{
	BASED_TRACE("Note hover entered!");
}

void InteractableNoteSystem::OnInteractionHoverExit(Tool* tool)
{
	BASED_TRACE("Note hover exited!");
}

void InteractableNoteSystem::OnInteract(Tool* tool)
{
	using namespace based;

	auto uiManager = Engine::Instance().GetUiManager();
	auto context = uiManager.GetContext("main");

	if (Rml::DataModelConstructor constructor = context->CreateDataModel("Note"))
	{
		constructor.Bind("NoteText", &mCurrentNote->mNoteText);
	}

	uiManager.SetPathPrefix("Assets/UI/");

	mCurrentNote->mDocument = uiManager.LoadWindow("DefaultNote", context)->document;
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

		mCurrentNote = &note;

		if (!trigger.mHoverEntered)
		{
			trigger.mHoverEntered = true;
			OnInteractionHoverEnter(trigger.mTool);
		}

		if (input::Keyboard::KeyDown(BASED_INPUT_KEY_E))
		{
			if (!note.mIsOpen)
			{
				OnInteract(trigger.mTool);
				note.mIsOpen = true;

				GameSystems::SetPlayerMouseLookEnabled(false);
				GameSystems::SetPlayerMovementEnabled(false);
			} else
			{
				note.mIsOpen = false;
				note.mDocument->Close();

				GameSystems::SetPlayerMouseLookEnabled(true);
				GameSystems::SetPlayerMovementEnabled(true);
			}
		}

		if (trigger.mShouldExit)
		{
			OnInteractionHoverExit(trigger.mTool);

			trigger.mTool->SetCurrentTrigger(nullptr);
			registry.remove<InteractionTrigger>(e);
		}
	}
}
