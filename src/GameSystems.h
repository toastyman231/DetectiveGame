#pragma once
#include "based/pch.h"
#include "based/engine.h"
#include "based/app.h"
#include "based/scene/entity.h"
#include "Dialogue/DialogueSystem.h"

#include "Interaction/InteractableNote.h"
#include "Interaction/InteractionDialogueSystem.h"
#include "Interaction/Tool.h"
#include "Player/MouseLook.h"
#include "Player/PlayerControllerSystem.h"

class GameSystems
{
public:
	inline static MouseLookSystem mMouseLookSystem;
	inline static PlayerControllerSystem mPlayerController;
	inline static ToolSystem mToolSystem;
	inline static InteractableNoteSystem mNoteSystem;
	inline static DialogueSystem mDialogueSystem;
	inline static InteractionDialogueSystem mDialogueTrigger;

	static void SetPlayerMouseLookEnabled(bool enabled)
	{
		using namespace based;

		// Get the player entity, check if they have a MouseLook component,
		// and set whether mouse look is enabled if they do
		auto scene = Engine::Instance().GetApp().GetCurrentScene();
		if (auto player = scene->GetEntityStorage().Get("Player"))
		{
			if (player->HasComponent<MouseLook>())
			{
				scene->GetRegistry().patch<MouseLook>(player->GetEntityHandle(), [&](auto& mLook)
					{
						mLook.mCanLook = enabled;
					});
			}
		}
	}

	static void SetPlayerMovementEnabled(bool enabled)
	{
		using namespace based;

		// Get the player entity, check if they have a CharacterController component,
		// and set whether movement is enabled if they do
		auto scene = Engine::Instance().GetApp().GetCurrentScene();
		if (auto player = scene->GetEntityStorage().Get("Player"))
		{
			if (player->HasComponent<scene::CharacterController>())
			{
				scene->GetRegistry().patch<scene::CharacterController>(player->GetEntityHandle(), [&](auto& ch)
					{
						ch.ControlEnabled = enabled;
					});
			}
		}
	}
};
