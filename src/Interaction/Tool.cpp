#include "based/pch.h"
#include "Tool.h"

#include "IInteractable.h"
#include "InteractionTrigger.h"
#include "based/app.h"
#include "based/engine.h"
#include "based/graphics/linerenderer.h"
#include "based/input/joystick.h"
#include "based/input/mouse.h"
#include "Jolt/Physics/Collision/CastResult.h"

#include "Jolt/Physics/Collision/RayCast.h"

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
			convert(camera->GetTransform().Position),
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
				continue;
			}

			// Set up the trigger for the newly hovered object
			auto& newTrigger = registry.emplace<InteractionTrigger>(hitEnt);
			newTrigger.mTool = &tool;
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
