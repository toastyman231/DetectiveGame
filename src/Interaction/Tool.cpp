#include "based/pch.h"
#include "Tool.h"

#include "IInteractable.h"
#include "InteractionTrigger.h"
#include "based/app.h"
#include "based/engine.h"
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

		JPH::RRayCast ray{
			convert(camera->GetTransform().Position),
			convert(glm::normalize(camera->GetForward())) * tool.GetInteractionRange()
		};

		JPH::RayCastResult hit;

		if (physSystem.GetNarrowPhaseQuery().CastRay(ray, hit))
		{
			auto hitId = hit.mBodyID;
			entt::entity hitEnt = static_cast<entt::entity>(physSystem.GetBodyInterface().GetUserData(hitId));

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

			//BASED_TRACE("Entity hit: {}", static_cast<uint32_t>(hitEnt));

			if (registry.all_of<InteractionTrigger>(hitEnt))
			{
				continue;
			}

			auto& newTrigger = registry.emplace<InteractionTrigger>(hitEnt);
			newTrigger.mTool = &tool;
			tool.SetCurrentTrigger(&newTrigger);

		} else
		{
			if (auto trigger = tool.GetCurrentTrigger())
			{
				trigger->mShouldExit = true;
				tool.SetCurrentTrigger(nullptr);
			}
			//BASED_TRACE("Did not hit anything!");
		}
	}
}
