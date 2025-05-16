#include "based/pch.h"
#include "MouseLook.h"

#include "based/app.h"
#include "based/input/joystick.h"
#include "based/input/mouse.h"
#include "based/scene/components.h"
#include "based/scene/entity.h"

void MouseLookSystem::Update(float deltaTime)
{
	using namespace based;

	auto scene = Engine::Instance().GetApp().GetCurrentScene();

	auto view = scene->GetRegistry().view<scene::Enabled, scene::EntityReference, MouseLook>();

	for (auto& e : view)
	{
		auto& mouseLookComp = view.get<MouseLook>(e);

		if (!mouseLookComp.mCanLook) continue;

		// Rotate camera view
		mouseLookComp.mPitch += static_cast<float>(input::Mouse::DX()) * mouseLookComp.mSensitivity * deltaTime;
		mouseLookComp.mYaw += static_cast<float>(input::Mouse::DY()) * mouseLookComp.mSensitivity * deltaTime;
		mouseLookComp.mPitch += input::Joystick::GetAxis(0, input::Joystick::Axis::RightStickHorizontal) * mouseLookComp.mSensitivity * deltaTime;
		mouseLookComp.mYaw += input::Joystick::GetAxis(0, input::Joystick::Axis::RightStickVertical) * mouseLookComp.mSensitivity * deltaTime;

		mouseLookComp.mYaw = math::Clamp(mouseLookComp.mYaw, -89.f, 89.f);

		auto camera = scene->GetEntityStorage().Get("Main Camera");
		
		camera->SetLocalRotation({ mouseLookComp.mYaw, mouseLookComp.mPitch, camera->GetTransform().LocalRotation().z});
	}
}
