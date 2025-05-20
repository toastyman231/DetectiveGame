#include "based/pch.h"
#include "InteractionDialogueSystem.h"

#include "InteractionTrigger.h"
#include "../GameSystems.h"
#include "based/app.h"
#include "based/engine.h"
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
	GameSystems::mDialogueSystem.SetCurrentDialogue(mCurrentDialogue->mPath);
	GameSystems::mDialogueSystem
		.mOnDialogueFinished
		.connect<&InteractionDialogueSystem::OnDialogueFinished>(this);

	auto view = based::Engine::Instance().GetApp().GetCurrentScene()->GetRegistry().view<based::input::InputComponent>();

	for (const auto& e : view)
	{
		auto [inputComp] = view.get(e);

		based::Engine::Instance().GetInputManager().AddInputMapping(inputComp, "IMC_Menu", 0);
	}
}

void InteractionDialogueSystem::Update(float deltaTime)
{
	using namespace based;

	auto& registry = Engine::Instance().GetApp().GetCurrentScene()->GetRegistry();
	auto view = registry.view<scene::Enabled, InteractionDialogueTrigger, Interactable>();
	auto scene = Engine::Instance().GetApp().GetCurrentScene();

	for (const auto& e : view)
	{
		auto [dialogue, interactable] = view.get(e);

		mCurrentDialogue = &dialogue;

		if (interactable.tags.HasTag(core::Tag("Interaction.Hover")))
		{
			OnInteractionHoverEnter(interactable.tool);
			interactable.tags.RemoveTag(core::Tag("Interaction.Hover"));
		}

		if (interactable.tags.HasTag(core::Tag("Interaction.Interact")))
		{
			interactable.tags.RemoveTag(core::Tag("Interaction.Interact"));
			mCurrentDialogueEntity = e;
			interactable.mCanInteract = false;
			OnInteract(interactable.tool);
		}

		if (interactable.tags.HasTag(core::Tag("Interaction.Unhover")))
		{
			OnInteractionHoverExit(interactable.tool);
			interactable.tags.RemoveTag(core::Tag("Interaction.Unhover"));
		}
	}

	mCurrentDialogue = nullptr;
}

void InteractionDialogueSystem::OnDialogueFinished()
{
	auto& registry = based::Engine::Instance().GetApp().GetCurrentScene()->GetRegistry();
	auto& interactable = registry.get<Interactable>(mCurrentDialogueEntity);

	ToolSystem::ShowInteractionUI(true);

	// Reset interaction when dialogue finishes
	interactable.mCanInteract = true;

	mCurrentDialogueEntity = entt::null;

	auto view = based::Engine::Instance().GetApp().GetCurrentScene()->GetRegistry().view<based::input::InputComponent>();

	for (const auto& e : view)
	{
		auto [inputComp] = view.get(e);

		based::Engine::Instance().GetInputManager().RemoveInputMapping(inputComp, "IMC_Menu");
	}
}
