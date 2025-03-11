#include "based/pch.h"
#include "InteractionDialogueSystem.h"

#include "InteractionTrigger.h"
#include "../GameSystems.h"
#include "based/app.h"
#include "based/engine.h"
#include "based/input/keyboard.h"
#include "based/scene/components.h"

void InteractionDialogueSystem::OnInteractionHoverEnter(Tool* tool)
{
	BASED_TRACE("Hovered dialogue");
}

void InteractionDialogueSystem::OnInteractionHoverExit(Tool* tool)
{
	BASED_TRACE("Unhovered dialogue");
}

void InteractionDialogueSystem::OnInteract(Tool* tool)
{
	IInteractable::OnInteract(tool);
}

void InteractionDialogueSystem::Update(float deltaTime)
{
	using namespace based;

	auto& registry = Engine::Instance().GetApp().GetCurrentScene()->GetRegistry();
	auto view = registry.view<scene::Enabled, InteractionDialogueTrigger, InteractionTrigger>();
	auto scene = Engine::Instance().GetApp().GetCurrentScene();

	for (const auto& e : view)
	{
		auto [dialogue, trigger] = view.get(e);

		if (!trigger.mHoverEntered)
		{
			trigger.mHoverEntered = true;
			OnInteractionHoverEnter(trigger.mTool);
		}

		if (input::Keyboard::KeyDown(BASED_INPUT_KEY_E) && dialogue.mCanInteract)
		{
			OnInteract(trigger.mTool);
			GameSystems::mDialogueSystem.SetCurrentDialogue(dialogue.mPath);
			dialogue.mCanInteract = false;
			mCurrentDialogueEntity = e;
			GameSystems::mDialogueSystem
				.mOnDialogueFinished
				.connect<&InteractionDialogueSystem::OnDialogueFinished>(this);
		}

		// Again, only called once when the item stops being hovered
		if (trigger.mShouldExit)
		{
			OnInteractionHoverExit(trigger.mTool);

			// Removing the trigger means this interactable will no longer be
			// considered when evaluating interactables
			registry.remove<InteractionTrigger>(e);
		}
	}
}

void InteractionDialogueSystem::OnDialogueFinished()
{
	auto& registry = based::Engine::Instance().GetApp().GetCurrentScene()->GetRegistry();
	auto& trigger = registry.get<InteractionTrigger>(mCurrentDialogueEntity);
	auto& dialogue = registry.get<InteractionDialogueTrigger>(mCurrentDialogueEntity);

	GameSystems::mToolSystem.CallOnInteract(true);
	OnInteractionHoverExit(trigger.mTool);

	// Reset interaction when dialogue finishes
	registry.remove<InteractionTrigger>(mCurrentDialogueEntity);
	dialogue.mCanInteract = true;

	mCurrentDialogueEntity = entt::null;
}
