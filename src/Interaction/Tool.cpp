#include "based/pch.h"
#include "Tool.h"

#include "InteractionTrigger.h"
#include "../GameSystems.h"
#include "based/app.h"
#include "based/engine.h"
#include "Jolt/Physics/Collision/CastResult.h"

#include "Jolt/Physics/Collision/RayCast.h"

void Tool::SetCurrentInteractable(Interactable* interactable)
{
	using namespace based;

	auto context = Engine::Instance().GetUiManager().GetContext("main");
	auto element = context->GetRootElement()->GetElementById("interact-parent");

	if (interactable != mCurrentInteractable)
	{
		if (mCurrentInteractable)
		{
			mCurrentInteractable->OnHoverExit();
			mCurrentInteractable->tool = nullptr;
		}

		if (interactable)
		{
			element->SetProperty("display", "flex");
			element->SetProperty("visibility", "visible");

			interactable->OnHoverEnter();
			interactable->tool = this;
		}
		else
		{
			element->SetProperty("display", "none");
			element->SetProperty("visibility", "hidden");
		}

		mCurrentInteractable = interactable;
	}
}

void ToolSystem::Initialize()
{
	auto view = based::Engine::Instance().GetApp().GetCurrentScene()->GetRegistry().view<based::input::InputComponent>();

	for (const auto& e : view)
	{
		auto [inputComp] = view.get(e);

		inputComp.mCompletedEvent.sink<based::input::InputAction>().connect<&ToolSystem::OnInteract>();
	}
}

void ToolSystem::OnInteract(const based::input::InputAction& action)
{
	if (action.name != "IA_Interact") return;

	auto view = based::Engine::Instance().GetApp().GetCurrentScene()->GetRegistry().view<Tool>();

	for (const auto& e : view)
	{
		auto [tool] = view.get(e);

		if (tool.GetCurrentInteractable() && tool.GetCurrentInteractable()->mCanInteract) 
			tool.GetCurrentInteractable()->OnInteract();
	}
}

void ToolSystem::ShowInteractionUI(bool show)
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

class TestObjectLayerFilter : public JPH::ObjectLayerFilter
{
public:
	bool ShouldCollide(JPH::ObjectLayer inLayer) const override
	{
		if (inLayer == based::physics::Layers::STATIC || inLayer == based::physics::Layers::UNUSED4) return true;
		else return false;
	}
};

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
		TestObjectLayerFilter filter;

		if (physSystem.GetNarrowPhaseQuery().CastRay(ray, hit, {}, filter))
		{
			// Associated entity is stored on the body as User Data
			auto hitId = hit.mBodyID;
			entt::entity hitEnt = static_cast<entt::entity>(physSystem.GetBodyInterface().GetUserData(hitId));

			// Need to make sure it's not null, in case I forgot to register the User Data value
			if (hitEnt == entt::null)
			{
				BASED_WARN("Invalid entity hit!");
				tool.SetCurrentInteractable(nullptr);
				continue;
			}

			// If the hit entity has an interactable component, and that interactable is not
			// the currently hovered one, set it as the currently hovered one.
			// This will trigger the old one to be unhovered, and the new one to be hovered (using tags).
			if (registry.all_of<Interactable>(hitEnt))
			{
				auto hitInteractable = &registry.get<Interactable>(hitEnt);

				if (hitInteractable->mCanInteract && hitInteractable != tool.GetCurrentInteractable())
				{
					tool.SetCurrentInteractable(hitInteractable);
				}
			} else
			{
				tool.SetCurrentInteractable(nullptr);
			}
		} else
		{
			// Clear current trigger if we aren't looking at an interactable.
			// That means this gets called once per interactable, as expected.
			// The interactables' system is responsible for removing any
			// InteractionTriggers from the affected entity.
			tool.SetCurrentInteractable(nullptr);
		}
	}
}
