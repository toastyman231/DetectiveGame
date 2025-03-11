#include "based/pch.h"
#include "Tool.h"

#include "IInteractable.h"
#include "InteractionTrigger.h"
#include "../GameSystems.h"
#include "based/app.h"
#include "based/engine.h"
#include "based/graphics/linerenderer.h"
#include "based/input/joystick.h"
#include "based/input/mouse.h"
#include "Jolt/Physics/Collision/CastResult.h"

#include "Jolt/Physics/Collision/RayCast.h"

void Tool::SetCurrentTrigger(InteractionTrigger* trigger)
{
	using namespace based;

	auto context = Engine::Instance().GetUiManager().GetContext("main");
	auto element = context->GetRootElement()->GetElementById("interact-parent");

	if (trigger != mTrigger)
	{
		if (trigger)
		{
			element->SetProperty("display", "flex");
			element->SetProperty("visibility", "visible");
		}
		else
		{
			element->SetProperty("display", "none");
			element->SetProperty("visibility", "hidden");
		}
	}

	mTrigger = trigger;
}

void ToolSystem::Initialize()
{
	mOnInteract.connect<&ToolSystem::OnInteract>();
}

void ToolSystem::CallOnInteract(bool show)
{
	if (GameSystems::mToolSystem.mOnInteract) GameSystems::mToolSystem.mOnInteract(show);
}

void ToolSystem::OnInteract(bool show)
{
	auto context = based::Engine::Instance().GetUiManager().GetContext("main");
	auto element = context->GetRootElement()->GetElementById("interact-parent");

	if (show)
	{
		element->SetProperty("display", "flex");
		element->SetProperty("visibility", "visible");
	}
	else
	{
		element->SetProperty("display", "none");
		element->SetProperty("visibility", "hidden");
	}
}

void ToolSystem::Update(float deltaTime)
{
	using namespace based;

	auto scene = Engine::Instance().GetApp().GetCurrentScene();
	auto camera = scene->GetActiveCamera();
	auto& registry = scene->GetRegistry();
	auto view = registry.view<scene::Enabled, Tool>();
	auto& physSystem = Engine::Instance().GetPhysicsManager().GetPhysicsSystem();

	for (auto& e : view)
	{
		auto& tool = view.get<Tool>(e);

		// Set up a ray to cast from the camera position out where the player is looking
		JPH::RRayCast ray{
			convert(camera->GetTransform().Position()),
			convert(glm::normalize(camera->GetForward())) * tool.GetInteractionRange()
		};

		JPH::RayCastResult hit;

		if (physSystem.GetNarrowPhaseQuery().CastRay(ray, hit))
		{
			// Associated entity is stored on the body as User Data
			auto hitId = hit.mBodyID;
			entt::entity hitEnt = static_cast<entt::entity>(physSystem.GetBodyInterface().GetUserData(hitId));

			// Need to make sure it's not null, in case I forgot to register the User Data value
			if (hitEnt == entt::null)
			{
				BASED_WARN("Invalid entity hit!");
				if (auto trigger = tool.GetCurrentTrigger())
				{
					trigger->mShouldExit = true;
					tool.SetCurrentTrigger(nullptr);
				}
				continue;
			}

			// InteractionTrigger is used to let any Interactable systems know
			// that their entity is being looked at/interacted with.
			if (registry.all_of<InteractionTrigger>(hitEnt))
			{
				tool.SetCurrentTrigger(&registry.get<InteractionTrigger>(hitEnt));
				continue;
			}

			// This only runs if the hit entity does NOT have a trigger,
			// so if the tool has one set, we must have looked at another
			// interactable.
			if (auto trigger = tool.GetCurrentTrigger(); trigger != nullptr)
			{
				trigger->mShouldExit = true;
			}

			// Set up the trigger for the newly hovered object
			auto& newTrigger = registry.emplace<InteractionTrigger>(hitEnt);
			newTrigger.mTool = &tool;
			newTrigger.mId = (uint32_t) hitEnt;
			tool.SetCurrentTrigger(&newTrigger);

		} else
		{
			// Clear current trigger if we aren't looking at an interactable.
			// That means this gets called once per interactable, as expected.
			// The interactables' system is responsible for removing any
			// InteractionTriggers from the affected entity.
			if (auto trigger = tool.GetCurrentTrigger())
			{
				trigger->mShouldExit = true;
				tool.SetCurrentTrigger(nullptr);
			}
		}
	}
}
