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
}

void InteractionDialogueSystem::OnInteractionHoverExit(Tool* tool)
{
}

void InteractionDialogueSystem::OnInteract(Tool* tool)
{
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

		if (input::Keyboard::KeyDown(BASED_INPUT_KEY_E))
		{
			GameSystems::mDialogueSystem.SetCurrentDialogue(dialogue.mPath);
			trigger.mShouldExit = true;
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
}
