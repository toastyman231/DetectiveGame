#include "based/pch.h"
#include "based/log.h"
#include "based/main.h"
#include "based/graphics/mesh.h"
#include "based/scene/components.h"
#include "based/input/mouse.h"
#include "based/scene/entity.h"

#include "GameSystems.h"
#include "based/graphics/model.h"
#include "based/input/keyboard.h"
#include "Systems/FMODSystem.h"

using namespace based;

class BasedApp : public based::App
{
public:
	based::core::WindowProperties GetWindowProperties() override
	{
		based::core::WindowProperties props;
		props.w = 1280;
		props.h = 720;
		props.imguiProps.IsDockingEnabled = true;
		props.title = "Detective Game";

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

		//scene::Scene::LoadScene("Assets/Scenes/ApartmentLevel.bscn");
		scene::Scene::LoadScene(ASSET_PATH("Scenes/Default3D.bscn"));

		auto floor = scene::Entity::CreateEntity("F");
		auto carpet = graphics::Material::LoadMaterialFromFile("Assets/Materials/Carpet.bmat",
			GetCurrentScene()->GetMaterialStorage());
		floor->SetScale(glm::vec3{ 100, 0.3f, 100 });
		floor->AddComponent<scene::MeshRenderer>(graphics::Mesh::LoadMeshFromFile(ASSET_PATH("Meshes/cube.obj"),
			GetCurrentScene()->GetMeshStorage()),
			carpet);
		floor->AddComponent<scene::BoxShapeComponent>(floor->GetTransform().Scale());
		auto floorShape = floor->GetComponent<scene::BoxShapeComponent>();
		floor->AddComponent<scene::RigidbodyComponent>(floorShape, JPH::EMotionType::Static,
			physics::Layers::STATIC);
		auto floorBody = floor->GetComponent<scene::RigidbodyComponent>();
		floorBody.RegisterBody(floor->GetEntityHandle());

		std::ifstream ifs("Assets/Notes/TestNote.txt");
		std::stringstream sstream;
		sstream << ifs.rdbuf();
		ifs.close();

		floor->AddComponent<InteractableNote>(sstream.str());

		GetCurrentScene()->GetEntityStorage().Load("F", floor);

		/*auto cube = GetCurrentScene()->GetEntityStorage().Get("Cube");
		cube->SetPosition({ 5, 0.f, 5 });
		cube->AddComponent<scene::BoxShapeComponent>(glm::vec3{ 1, 1, 1 },
			cube->GetTransform().Position(), glm::vec3{0, 0, 0});
		auto shape = cube->GetComponent<scene::BoxShapeComponent>();
		cube->AddComponent<scene::RigidbodyComponent>(shape, JPH::EMotionType::Static, physics::Layers::STATIC);
		auto cubeBody = cube->GetComponent<scene::RigidbodyComponent>();
		cubeBody.RegisterBody(cube->GetEntityHandle());
		//cube->AddComponent<InteractableNote>("This is a note with custom text!");
		cube->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Test.txt");*/

		auto cam = GetCurrentScene()->GetEntityStorage().Get("Main Camera");

		auto player = scene::Entity::CreateEntity("Player");
		player->AddComponent<scene::CapsuleShapeComponent>(1.8f, 0.4f);
		auto capsule = player->GetComponent<scene::CapsuleShapeComponent>();
		player->AddComponent<scene::CharacterController>(
			scene::Transform(glm::vec3(0)),
			capsule.shape);
		player->AddComponent<MouseLook>(200.f);
		cam->SetParent(player, false);
		cam->SetLocalPosition({ 0, 1.8f - 0.4f, 0 });
		player->AddComponent<Tool>(6.f);

		/*auto door = scene::Entity::CreateEntity("Test");
		auto doorModel = graphics::Model::CreateModel("Assets/Models/door.fbx",
			GetCurrentScene()->GetModelStorage(), "DoorModel");
		door->AddComponent<scene::ModelRenderer>(doorModel);
		door->SetPosition(glm::vec3(0.f, 1.5f, 0.f));
		door->SetScale(glm::vec3(3.f));
		door->SetRotation(glm::vec3(90.f, 0.f, -90.f));*/

		/*auto spawn = GetCurrentScene()->GetEntityStorage().Get("PlayerSpawn");
		player->SetPosition(spawn->GetTransform().Position());*/

		auto scene = scene::Entity::CreateEntity("Scene");
		scene->SetRotation(glm::vec3(-90.f, 0.f, 0.f));
		scene->SetPosition(glm::vec3(0.f, 0.5f, 0.f));
		auto sceneModel = graphics::Model::CreateModel("Assets/Models/ApartmentScene.fbx",
			GetCurrentScene()->GetModelStorage(), "ApartmentScene");
		/*for (int i = 0; i < sceneModel->GetNumMeshes(); i++)
		{
			sceneModel->SetMaterial(GetCurrentScene()->GetMaterialStorage().Get("Lit"), i);
		}*/
		scene->AddComponent<scene::ModelRenderer>(sceneModel);

		auto light = scene::Entity::CreateEntity("Light");
		light->AddComponent<scene::PointLight>(1.f, 0.09f, 0.032f, 10.0f, glm::vec3(1.f));
		light->AddComponent<scene::MeshRenderer>(
			graphics::Mesh::LoadMeshFromFile(ASSET_PATH("Meshes/cube.obj"),
				GetCurrentScene()->GetMeshStorage()),
			graphics::Material::LoadMaterialFromFile(ASSET_PATH("Materials/Unlit.bmat"),
				GetCurrentScene()->GetMaterialStorage()));
		light->SetScale(glm::vec3(0.3f));
		/*light->SetParent(player, false);
		light->SetLocalPosition(glm::vec3(0.f, 1.8f - 0.4f, 0.f));*/
		light->SetPosition(glm::vec3(0.f, 2.5f, 0.f));

		/*auto dl = GetCurrentScene()->GetEntityStorage().Get("Directional Light")
			->GetComponent<scene::DirectionalLight>().intensity = 10.f;*/

		GetCurrentScene()->GetEntityStorage().Load("Player", player);
		GetCurrentScene()->GetEntityStorage().Load("Scene", scene);
		GetCurrentScene()->GetEntityStorage().Load("Light", light);

		scene::Entity::DestroyEntity(GetCurrentScene()->GetEntityStorage().Get("Cube"));
		//GetCurrentScene()->GetEntityStorage().Load("Door", door);

		Rml::Debugger::Initialise(Engine::Instance().GetUiManager().GetContext("main"));

		auto context = Engine::Instance().GetUiManager().GetContext("main");
		Engine::Instance().GetUiManager().SetPathPrefix("Assets/UI/");
		Engine::Instance().GetUiManager().LoadWindow("PlayerHUD", context, "PlayerHUD");

		FMODSystem::InitializeFMOD();
		FMODSystem::LoadBanks("Assets/FMOD/Banks");
		GameSystems::mToolSystem.Initialize();
		GameSystems::mDialogueSystem.Initialize();
		GameSystems::mNoteSystem.Initialize();
		GameSystems::mSolutionPanelSystem.Initialize();
		GameSystems::mPlayerController.Initialize();

#ifdef BASED_CONFIG_DEBUG
		GameSystems::mSoundTest.Initialize();
#endif
	}

	void Shutdown() override
	{
		App::Shutdown();
	}

	void Update(float deltaTime) override
	{
		App::Update(deltaTime);

		auto pos = GetCurrentScene()->GetEntityStorage().Get("Player")->GetTransform().Position();
		//BASED_TRACE("Pos: {} {} {}", pos.x, pos.y, pos.z);

		FMODSystem::Update(deltaTime);

		GameSystems::mMouseLookSystem.Update(deltaTime);

		GameSystems::mPlayerController.Update(deltaTime);

		GameSystems::mToolSystem.Update(deltaTime);

		GameSystems::mNoteSystem.Update(deltaTime);

		GameSystems::mDialogueTrigger.Update(deltaTime);

		GameSystems::mDialogueSystem.Update(deltaTime);

		GameSystems::mSolutionPanelSystem.Update(deltaTime);

#ifdef BASED_CONFIG_DEBUG
		GameSystems::mSoundTest.Update(deltaTime);
#endif
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
