#include "based/pch.h"
#include "based/log.h"
#include "based/main.h"
#include "based/graphics/mesh.h"
#include "based/input/keyboard.h"
#include "based/scene/components.h"
#include "based/input/mouse.h"
#include "based/scene/entity.h"

using namespace based;

class BasedApp : public based::App
{
private:
	std::shared_ptr<scene::Entity> iconEntity;
	JPH::Vec3 desiredDir;

public:
	core::WindowProperties GetWindowProperties() override
	{
		core::WindowProperties props;
		props.w = 1280;
		props.h = 720;
		props.imguiProps.IsDockingEnabled = true;

		return props;
	}

	based::GameSettings GetGameSettings() override
	{
		based::GameSettings settings;
		settings.gameMemory = 1;

		return settings;
	}

	void Initialize() override
	{
		App::Initialize();
		Engine::Instance().GetWindow().SetShouldRenderToScreen(true);
		input::Mouse::SetCursorVisible(!Engine::Instance().GetWindow().GetShouldRenderToScreen());
		input::Mouse::SetCursorMode(Engine::Instance().GetWindow().GetShouldRenderToScreen() ?
			input::CursorMode::Confined : input::CursorMode::Free);
		Engine::Instance().GetPhysicsManager().SetRenderDebug(true);

		scene::Scene::LoadScene(ASSET_PATH("Scenes/Default3D.bscn"));

		auto floor = scene::Entity::CreateEntity("Floor");
		floor->SetScale(glm::vec3{ 10, 0.3f, 10 });
		floor->AddComponent<scene::MeshRenderer>(graphics::Mesh::LoadMeshFromFile(ASSET_PATH("Meshes/cube.obj"),
			GetCurrentScene()->GetMeshStorage()));
		floor->AddComponent<scene::BoxShapeComponent>(floor->GetTransform().Scale);
		auto floorShape = floor->GetComponent<scene::BoxShapeComponent>();
		floor->AddComponent<scene::RigidbodyComponent>(floorShape, JPH::EMotionType::Static, physics::Layers::STATIC);

		GetCurrentScene()->GetEntityStorage().Load("Floor", floor);

		GetCurrentScene()->GetEntityStorage().Get("Cube")->SetPosition({ 5, 0.8f, 5 });

		auto cam = GetCurrentScene()->GetEntityStorage().Get("Main Camera");

		auto player = scene::Entity::CreateEntity("Player");
		player->AddComponent<scene::CapsuleShapeComponent>(1.35f * 0.5f, 0.3f);
		auto capsule = player->GetComponent<scene::CapsuleShapeComponent>();
		player->AddComponent<scene::CharacterController>(
			scene::Transform(glm::vec3(0)),
			capsule.shape);
		//player->SetPosition(glm::vec3{ 0, 30.f, 0 });
		cam->SetParent(player);
		cam->SetLocalTransform({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });

		GetCurrentScene()->GetEntityStorage().Load("Player", player);
	}

	void Shutdown() override
	{
		App::Shutdown();
	}

	void Update(float deltaTime) override
	{
		App::Update(deltaTime);

		auto view = GetCurrentScene()->GetRegistry().view<scene::Enabled, scene::CharacterController>();

		JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;

		for (auto& e : view)
		{
			scene::CharacterController& character = view.get<scene::CharacterController>(e);
			auto& physSystem = Engine::Instance().GetPhysicsManager().GetPhysicsSystem();

			/*character.Character->SetPosition(
				convert(GetCurrentScene()->GetEntityStorage().Get("Player")->GetTransform().Position));*/

			bool controlHorizontal = character.Character->IsSupported();

			JPH::Vec3 moveDir = JPH::Vec3::sZero();

			if (input::Keyboard::Key(BASED_INPUT_KEY_A)) moveDir.SetZ(-1);
			if (input::Keyboard::Key(BASED_INPUT_KEY_D)) moveDir.SetZ(1);
			if (input::Keyboard::Key(BASED_INPUT_KEY_W)) moveDir.SetX(1);
			if (input::Keyboard::Key(BASED_INPUT_KEY_S)) moveDir.SetX(-1);
			if (moveDir != JPH::Vec3::sZero()) moveDir = moveDir.Normalized();

			//BASED_TRACE("Move Dir: {} {} {}", moveDir.GetX(), moveDir.GetY(), moveDir.GetZ());

			JPH::Vec3 camForward = convert(GetCurrentScene()->GetActiveCamera()->GetForward());
			camForward.SetY(0);
			camForward = camForward.NormalizedOr(JPH::Vec3::sZero());
			JPH::Quat rot = JPH::Quat::sFromTo(JPH::Vec3::sAxisX(), camForward);
			moveDir = rot * moveDir;

			if (controlHorizontal)
				desiredDir = 0.25f * moveDir * 6.f + 0.75f * desiredDir;

			//BASED_TRACE("Desired Dir: {} {} {}", desiredDir.GetX(), desiredDir.GetY(), desiredDir.GetZ());

			//auto allowSliding = !moveDir.IsNearZero();

			JPH::Quat charUp = JPH::Quat::sEulerAngles(JPH::Vec3(0, 0, 0));
			character.Character->SetUp(charUp.RotateAxisY());
			character.Character->SetRotation(charUp);

			character.Character->UpdateGroundVelocity();

			JPH::Vec3 current_vertical_velocity = character.Character->GetLinearVelocity().Dot(
				character.Character->GetUp()) * character.Character->GetUp();
			JPH::Vec3 ground_velocity = character.Character->GetGroundVelocity();
			JPH::Vec3 new_velocity;
			bool moving_towards_ground = (current_vertical_velocity.GetY() - ground_velocity.GetY()) < 0.1f;
			if (character.Character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround
				&& moving_towards_ground)
			{
				// Assume velocity of ground when on ground
				new_velocity = ground_velocity;

				// Jump
				/*if (inJump && moving_towards_ground)
					new_velocity += sJumpSpeed * mCharacter->GetUp();*/
			}
			else
			{
				new_velocity = current_vertical_velocity;
			}

			// Gravity
			new_velocity += charUp * physSystem.GetGravity() * deltaTime;

			if (controlHorizontal)
				new_velocity += charUp * desiredDir;
			else
			{
				new_velocity = character.Character->GetLinearVelocity() - current_vertical_velocity;
			}

			BASED_TRACE("New Velocity: {} {} {}", new_velocity.GetX(), new_velocity.GetY(), new_velocity.GetZ());

			character.Character->SetLinearVelocity(new_velocity);

			character.Character->ExtendedUpdate(deltaTime,
				physSystem.GetGravity(),
				update_settings,
				physSystem.GetDefaultBroadPhaseLayerFilter(physics::Layers::MOVING),
				physSystem.GetDefaultLayerFilter(physics::Layers::MOVING),
				{ },
				{ },
				*Engine::Instance().GetPhysicsManager().GetTempAllocator());

			auto position = character.Character->GetPosition();
			GetCurrentScene()->GetEntityStorage().Get("Player")->SetPosition(
				convert(position));
			BASED_TRACE("Pos: {} {} {}", position.GetX(), position.GetY(), position.GetZ());
		}
	}

	void Render() override
	{
		App::Render();
	}

	void ImguiRender() override
	{
	}
};

based::App* CreateApp()
{
	return new BasedApp();
}