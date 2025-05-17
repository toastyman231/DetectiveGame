#include "based/pch.h"
#include "PlayerControllerSystem.h"

#include "../Systems/FMODSystem.h"
#include "based/app.h"
#include "based/core/basedtime.h"
#include "based/math/random.h"
#include "based/scene/components.h"
#include "based/scene/entity.h"

void PlayerControllerSystem::Initialize(float stepInterval)
{
	mStepInterval = stepInterval;
	mFootsteps0 = FMODSystem::CreateFMODEvent("event:/Footsteps/Footsteps");
	mFootsteps1 = FMODSystem::CreateFMODEvent("event:/Footsteps/Footsteps");
	mCreaks = FMODSystem::CreateFMODEvent("event:/Footsteps/Creaks");

	using namespace based;

	auto scene = Engine::Instance().GetApp().GetCurrentScene();

	auto view = scene->GetRegistry().view<input::InputComponent, scene::EntityReference, scene::CharacterController>();

	for (auto& e : view)
	{
		auto& inputComp = view.get<input::InputComponent>(e);
		inputComp.mTriggeredEvent.sink<input::InputAction>().connect<&PlayerControllerSystem::MoveCharacter>(this);
	}
}

void PlayerControllerSystem::Update(float deltaTime)
{
	using namespace based;

	// Set up references
	auto scene = Engine::Instance().GetApp().GetCurrentScene();
	auto& physSystem = Engine::Instance().GetPhysicsManager().GetPhysicsSystem();

	auto view = scene->GetRegistry().view<scene::Enabled, scene::EntityReference, scene::CharacterController>();

	for (auto& e : view)
	{
		scene::CharacterController& character = scene->GetRegistry().get<scene::CharacterController>(e);
		auto entity = scene->GetRegistry().get<scene::EntityReference>(e).entity.lock();

		if (!entity || !character.ControlEnabled)
		{
			return; // Don't update if the backing entity is invalid (should never happen)
		}

		// Align physics simulation with entity position (in case it was moved manually somewhere else)
		character.Character->SetPosition(
			convert(entity->GetTransform().Position()));

		// Allow horizontal movement control if the character is supported by solid ground
		// or if explicitly allowed by the user
		bool controlHorizontal = character.AllowAirControl || character.Character->IsSupported();

		// Align desired move direction with the camera's facing direction
		JPH::Vec3 camForward = convert(scene->GetActiveCamera()->GetForward());
		camForward.SetY(0);
		camForward = camForward.NormalizedOr(JPH::Vec3::sZero());
		JPH::Quat rot = JPH::Quat::sFromTo(JPH::Vec3::sAxisX(), camForward);
		mMoveDir = rot * mMoveDir;

		// If the player has control (not in the air), adjust their movement to account for inertia
		if (controlHorizontal)
			mDesiredDirection = 0.25f * mMoveDir * character.Speed + 0.75f * mDesiredDirection;

		// Only really necessary if the up direction is going to change.
		// Otherwise I think this can be set once on initialization
		JPH::Quat charUp = JPH::Quat::sEulerAngles(JPH::Vec3(0, 0, 0));
		character.Character->SetUp(charUp.RotateAxisY());
		character.Character->SetRotation(charUp);

		// Quick estimate of ground velocity used for determining if the player is falling
		character.Character->UpdateGroundVelocity();

		// If the player is grounded, use the ground velocity as their actual velocity
		// and give them the opportunity to jump.
		// If not, their velocity is limited to only the vertical component (keep falling)
		JPH::Vec3 current_vertical_velocity = character.Character->GetLinearVelocity().Dot(
			character.Character->GetUp()) * character.Character->GetUp();
		JPH::Vec3 ground_velocity = character.Character->GetGroundVelocity();
		JPH::Vec3 new_velocity;
		bool moving_towards_ground = (current_vertical_velocity.GetY() - ground_velocity.GetY()) < 0.1f;
		if (character.Character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround
			&& moving_towards_ground)
		{
			new_velocity = ground_velocity;
		}
		else
		{
			new_velocity = current_vertical_velocity;
		}

		auto groundVel = character.Character->GetLinearVelocity();
		if (groundVel.LengthSq() > 0.15f && math::Abs(current_vertical_velocity.GetY()) < 0.1f)
		{
			auto curTime = core::Time::GetTime();
			if (curTime - mLastStepTime >= mStepInterval)
			{
				mLastStepTime = curTime;
				if (mStepIndex > 0) mFootsteps0->start();
				else mFootsteps1->start();

				auto randValue = math::Random01();
				if (randValue <= mCreakChance)
				{
					mCreaks->start();
				}

				mStepIndex *= -1;
			}
		}

		// Add gravity
		new_velocity += charUp * physSystem.GetGravity() * core::Time::DeltaTime();

		// If they have horizontal control, adjust their current movement in the desired direction
		// Otherwise keep moving in the current direction, adjusted by their vertical movement
		if (controlHorizontal)
			new_velocity += charUp * mDesiredDirection;
		else
		{
			new_velocity += character.Character->GetLinearVelocity() - current_vertical_velocity;
		}

		// Update velocity
		character.Character->SetLinearVelocity(new_velocity);

		// Step forward physics sim for the character
		JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;
		character.Character->ExtendedUpdate(deltaTime,
			physSystem.GetGravity(),
			update_settings,
			physSystem.GetDefaultBroadPhaseLayerFilter(physics::Layers::MOVING),
			physSystem.GetDefaultLayerFilter(physics::Layers::MOVING),
			{ },
			{ },
			*Engine::Instance().GetPhysicsManager().GetTempAllocator());

		// Make sure backing entity has the same position as the controller
		auto position = character.Character->GetPosition();
		entity->SetPosition(convert(position));
	}

	mMoveDir = JPH::Vec3::sZero(); // Need to clear movement at the end of the frame since it's now been handled
}

void PlayerControllerSystem::MoveCharacter(const based::input::InputAction& action)
{
	if (action.name == "IA_Move")
	{
		mMoveDir = JPH::Vec3(action.GetValue().axis2DValue.x, 0.f, action.GetValue().axis2DValue.y);
		if (mMoveDir != JPH::Vec3::sZero()) mMoveDir = mMoveDir.Normalized();
	}
}
