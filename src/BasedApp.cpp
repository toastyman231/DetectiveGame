#include "based/pch.h"
#include "based/log.h"
#include "based/main.h"
#include "based/graphics/mesh.h"
#include "based/scene/components.h"
#include "based/input/mouse.h"
#include "based/scene/entity.h"
#include "Player/PlayerControllerSystem.h"

using namespace based;

class BasedApp : public based::App
{
private:
	PlayerControllerSystem mPlayerController;

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
		Engine::Instance().GetPhysicsManager().SetRenderDebug(false);

		scene::Scene::LoadScene(ASSET_PATH("Scenes/Default3D.bscn"));

		auto floor = scene::Entity::CreateEntity("Floor");
		floor->SetScale(glm::vec3{ 10, 0.3f, 10 });
		floor->AddComponent<scene::MeshRenderer>(graphics::Mesh::LoadMeshFromFile(ASSET_PATH("Meshes/cube.obj"),
			GetCurrentScene()->GetMeshStorage()));
		floor->AddComponent<scene::BoxShapeComponent>(floor->GetTransform().Scale);
		auto floorShape = floor->GetComponent<scene::BoxShapeComponent>();
		floor->AddComponent<scene::RigidbodyComponent>(floorShape, JPH::EMotionType::Static, physics::Layers::STATIC);

		GetCurrentScene()->GetEntityStorage().Load("Floor", floor);

		auto cube = GetCurrentScene()->GetEntityStorage().Get("Cube");
		cube->SetPosition({ 5, 0.8f, 5 });
		cube->AddComponent<scene::BoxShapeComponent>(glm::vec3{ 1, 1, 1 }, 
			cube->GetTransform().Position, glm::vec3{0, 0, 0});
		auto shape = cube->GetComponent<scene::BoxShapeComponent>();
		cube->AddComponent<scene::RigidbodyComponent>(shape, JPH::EMotionType::Static, physics::Layers::STATIC);

		auto cam = GetCurrentScene()->GetEntityStorage().Get("Main Camera");

		auto player = scene::Entity::CreateEntity("Player");
		player->AddComponent<scene::CapsuleShapeComponent>(1.35f * 0.5f, 0.3f);
		auto capsule = player->GetComponent<scene::CapsuleShapeComponent>();
		player->AddComponent<scene::CharacterController>(
			scene::Transform(glm::vec3(0)),
			capsule.shape);
		cam->SetParent(player, false);
		cam->SetLocalPosition({ 0, 1.35f * 0.5f, 0 });

		GetCurrentScene()->GetEntityStorage().Load("Player", player);
	}

	void Shutdown() override
	{
		App::Shutdown();
	}

	void Update(float deltaTime) override
	{
		App::Update(deltaTime);

		mPlayerController.Update(deltaTime);
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