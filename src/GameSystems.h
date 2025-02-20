#pragma once
#include "based/pch.h"
#include "based/engine.h"
#include "based/app.h"
#include "based/scene/entity.h"

#include "Interaction/InteractableNote.h"
#include "Interaction/Tool.h"
#include "Player/MouseLook.h"
#include "Player/PlayerControllerSystem.h"

namespace GameSystems
{
	static MouseLookSystem mMouseLookSystem;
	static PlayerControllerSystem mPlayerController;
	static ToolSystem mToolSystem;
	static InteractableNoteSystem mNoteSystem;

	static void SetPlayerMouseLookEnabled(bool enabled)
	{
		using namespace based;

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
}
