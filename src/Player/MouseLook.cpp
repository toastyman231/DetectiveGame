#include "based/pch.h"
#include "MouseLook.h"

#include "based/app.h"
#include "based/core/basedtime.h"
#include "based/input/joystick.h"
#include "based/input/mouse.h"
#include "based/scene/components.h"
#include "based/scene/entity.h"

void MouseLookSystem::Initialize()
{
	using namespace based;

	auto scene = Engine::Instance().GetApp().GetCurrentScene();

	auto view = scene->GetRegistry().view<input::InputComponent>();

	for (auto& e : view)
	{
		auto& inputComp = view.get<input::InputComponent>(e);
		inputComp.mTriggeredEvent.sink<input::InputAction>().connect<&MouseLookSystem::HandleInput>(this);
	}
}

void MouseLookSystem::Update(float deltaTime)
{
	
}

void MouseLookSystem::HandleInput(const based::input::InputAction& action)
{
	if (action.name != "IA_Look") return;

	using namespace based;

	auto scene = Engine::Instance().GetApp().GetCurrentScene();

	auto view = scene->GetRegistry().view<scene::Enabled, scene::EntityReference, MouseLook>();

	for (auto& e : view)
	{
		auto& mouseLookComp = view.get<MouseLook>(e);

		if (!mouseLookComp.mCanLook) continue;


		// Rotate camera view
		auto sensitivity = mouseLookComp.mSensitivity;
		if (scene->GetRegistry().all_of<input::InputComponent>(e))
		{
			input::InputComponent& input = scene->GetRegistry().get<input::InputComponent>(e);
			if (input.mInputMethod == input::InputMethod::Controller)
			{
				BASED_TRACE("USING CONTROLLER SENSITIVITY");
				sensitivity = 250.f;
			}
		}
		mouseLookComp.mPitch += action.GetValue().axis2DValue.x * sensitivity * core::Time::DeltaTime();
		mouseLookComp.mYaw -= action.GetValue().axis2DValue.y * sensitivity * core::Time::DeltaTime();

		mouseLookComp.mYaw = math::Clamp(mouseLookComp.mYaw, -89.f, 89.f);

		auto camera = scene->GetEntityStorage().Get("Main Camera");

		camera->SetLocalRotation({ mouseLookComp.mYaw, mouseLookComp.mPitch, camera->GetTransform().LocalRotation().z });
	}
}
