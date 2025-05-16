#include "based/pch.h"
#include "based/log.h"
#include "based/main.h"
#include "based/graphics/mesh.h"
#include "based/scene/components.h"
#include "based/input/mouse.h"
#include "based/scene/entity.h"

#include "GameSystems.h"
#include "based/core/basedtime.h"
#include "based/graphics/model.h"
#include "based/input/keyboard.h"
#include "Interaction/InteractionTrigger.h"
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
		Engine::Instance().GetWindow().SetFullscreen(false);

		//core::Time::SetTimeScale(0.f);

		//scene::Scene::LoadScene("Assets/Scenes/ApartmentLevel.bscn");
		scene::Scene::LoadScene(ASSET_PATH("Scenes/Default3D.bscn"));

		/*auto floor = scene::Entity::CreateEntity("F");
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
		floor->AddComponent<Interactable>();
		floor->AddComponent<InteractableNote>(std::filesystem::path("Assets/Notes/TestNote.txt"));

		GetCurrentScene()->GetEntityStorage().Load("F", floor);*/

		/*auto cube = GetCurrentScene()->GetEntityStorage().Get("Cube");//GetCurrentScene()->GetEntityStorage().Get("Cube");
		cube->SetPosition({ 5, 1.f, 5 });
		cube->AddComponent<scene::BoxShapeComponent>(glm::vec3{ 1, 1, 1 },
			cube->GetTransform().Position(), glm::vec3{0, 0, 0});
		auto shape = cube->GetComponent<scene::BoxShapeComponent>();
		cube->AddComponent<scene::RigidbodyComponent>(shape, JPH::EMotionType::Static, physics::Layers::UNUSED4);
		//cube->AddComponent<InteractableNote>("This is a note with custom text!");
		cube->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Test.txt");
		cube->AddComponent<Interactable>();*/

		auto cam = GetCurrentScene()->GetEntityStorage().Get("Main Camera");

		auto player = scene::Entity::CreateEntity("Player");
		player->AddComponent<scene::CapsuleShapeComponent>(1.8f, 0.4f);
		auto capsule = player->GetComponent<scene::CapsuleShapeComponent>();
		player->AddComponent<input::InputComponent>();
		auto& input = player->GetComponent<input::InputComponent>();
		Engine::Instance().GetInputManager().AddInputMapping(input, "IMC_Default");
		player->AddComponent<scene::CharacterController>(
			scene::Transform(glm::vec3(0)),
			capsule.shape);
		player->AddComponent<MouseLook>(250.f);
		cam->SetParent(player, false);
		cam->SetLocalPosition({ 0, 1.8f - 0.4f, 0 });
		player->AddComponent<Tool>(6.f);
		player->SetPosition(glm::vec3(0.f, 10.f, 0.f));

		/*auto spawn = GetCurrentScene()->GetEntityStorage().Get("PlayerSpawn");
		player->SetPosition(spawn->GetTransform().Position());*/

		auto scene = scene::Entity::CreateEntity("Scene");
		scene->SetRotation(glm::vec3(-90.f, 0.f, 0.f));
		scene->SetPosition(glm::vec3(0.f, 0.5f, 0.f));
		auto sceneModel = graphics::Model::CreateModel("Assets/Models/ApartmentScene_StaticGeo.fbx",
			GetCurrentScene()->GetModelStorage(), "ApartmentScene");
		scene->AddComponent<scene::ModelRenderer>(sceneModel);
		sceneModel->GenerateModelCollisions(scene->GetTransform());

		auto scene_NoCollision = scene::Entity::CreateEntity("Scene 2");
		scene_NoCollision->SetPosition(glm::vec3(0.f, 0.5f, 0.f));
		auto noCollisionModel = graphics::Model::CreateModel("Assets/Models/ApartmentScene_NoCollisionGeo.fbx",
			GetCurrentScene()->GetModelStorage(), "ApartmentScene_NoCollision");
		scene_NoCollision->AddComponent<scene::ModelRenderer>(noCollisionModel);

		auto light = scene::Entity::CreateEntity("Light");
		light->AddComponent<scene::PointLight>(1.f, 0.09f, 0.032f, 1.0f, glm::vec3(1.f));
		/*light->AddComponent<scene::MeshRenderer>(
			graphics::Mesh::LoadMeshFromFile(ASSET_PATH("Meshes/cube.obj"),
				GetCurrentScene()->GetMeshStorage()),
			graphics::Material::LoadMaterialFromFile(ASSET_PATH("Materials/Unlit.bmat"),
				GetCurrentScene()->GetMaterialStorage()));*/
		light->SetScale(glm::vec3(0.3f));
		light->SetPosition(glm::vec3(-0.173f, 5.f, -3.f));

		auto light2 = scene::Entity::CreateEntity("Light 2");
		light2->AddComponent<scene::PointLight>(1.f, 0.09f, 0.032f, 3.0f, glm::vec3(1.f));
		/*light2->AddComponent<scene::MeshRenderer>(
			graphics::Mesh::LoadMeshFromFile(ASSET_PATH("Meshes/cube.obj"),
				GetCurrentScene()->GetMeshStorage()),
			graphics::Material::LoadMaterialFromFile(ASSET_PATH("Materials/Unlit.bmat"),
				GetCurrentScene()->GetMaterialStorage()));*/
		light2->SetScale(glm::vec3(0.3f));
		light2->SetPosition(glm::vec3(16.6f, 5.f, -6.f));

		/*auto dl = GetCurrentScene()->GetEntityStorage().Get("Directional Light")
			->GetComponent<scene::DirectionalLight>().intensity = 10.f;*/

		auto col = scene::Entity::CreateEntity("PaintingCollider");
		col->SetPosition(glm::vec3(3.84f, 0.9f, -5.77f));
		col->SetScale(glm::vec3(0.482f, 1.f, 1.48f));
		col->AddComponent<scene::BoxShapeComponent>(col->GetTransform().Scale(), 
			col->GetTransform().Position(), glm::vec3(0.f));
		auto colShape = col->GetComponent<scene::BoxShapeComponent>();
		col->AddComponent<scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, physics::Layers::UNUSED4);
		col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Painting.txt");
		col->AddComponent<Interactable>();
		GetCurrentScene()->GetEntityStorage().Load(col->GetEntityName(), col);

		col = scene::Entity::CreateEntity("ResearchCollider");
		col->SetPosition(glm::vec3(4.94f, 2.63f, -2.16f));
		col->SetScale(glm::vec3(0.642f, 0.295f, 0.554f));
		col->AddComponent<scene::BoxShapeComponent>(col->GetTransform().Scale(),
			col->GetTransform().Position(), glm::vec3(0.f));
		colShape = col->GetComponent<scene::BoxShapeComponent>();
		col->AddComponent<scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, physics::Layers::UNUSED4);
		col->AddComponent<InteractableNote>(std::filesystem::path("Assets/Notes/ResearchNotes1.txt"));
		col->AddComponent<Interactable>();
		GetCurrentScene()->GetEntityStorage().Load(col->GetEntityName(), col);

		col = scene::Entity::CreateEntity("ToolsCollider");
		col->SetPosition(glm::vec3(8.39f, 2.88f, -2.16f));
		col->SetScale(glm::vec3(1.061f, 0.295f, 0.554f));
		col->AddComponent<scene::BoxShapeComponent>(col->GetTransform().Scale(),
			col->GetTransform().Position(), glm::vec3(0.f));
		colShape = col->GetComponent<scene::BoxShapeComponent>();
		col->AddComponent<scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, physics::Layers::UNUSED4);
		col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Tools.txt");
		col->AddComponent<Interactable>();
		GetCurrentScene()->GetEntityStorage().Load(col->GetEntityName(), col);

		col = scene::Entity::CreateEntity("ChairCollider");
		col->SetPosition(glm::vec3(9.13f, 0.68f + 0.5f, -4.62f));
		col->SetRotation(glm::vec3(0.f, 45.f, 0.f));
		col->SetScale(glm::vec3(1.62f, 0.727f, 0.846f));
		col->AddComponent<scene::BoxShapeComponent>(col->GetTransform().Scale(),
			col->GetTransform().Position(), col->GetTransform().EulerAngles());
		colShape = col->GetComponent<scene::BoxShapeComponent>();
		col->AddComponent<scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, physics::Layers::UNUSED4);
		col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Chair.txt");
		col->AddComponent<Interactable>();
		GetCurrentScene()->GetEntityStorage().Load(col->GetEntityName(), col);

		col = scene::Entity::CreateEntity("HiddenNotesCollider");
		col->SetPosition(glm::vec3(13.9f, 0.82f, -3.84f));
		col->SetScale(glm::vec3(0.481f));
		col->AddComponent<scene::BoxShapeComponent>(col->GetTransform().Scale(),
			col->GetTransform().Position(), glm::vec3(0.f));
		colShape = col->GetComponent<scene::BoxShapeComponent>();
		col->AddComponent<scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, physics::Layers::UNUSED4);
		col->AddComponent<InteractableNote>(std::filesystem::path("Assets/Notes/ResearchNotes2.txt"));
		col->AddComponent<Interactable>();
		GetCurrentScene()->GetEntityStorage().Load(col->GetEntityName(), col);

		col = scene::Entity::CreateEntity("TeleswapperCollider");
		col->SetPosition(glm::vec3(18.02f, 2.95f, -9.38f));
		col->SetScale(glm::vec3(0.676f, 0.380f, 0.676f));
		col->AddComponent<scene::BoxShapeComponent>(col->GetTransform().Scale(),
			col->GetTransform().Position(), glm::vec3(0.f));
		colShape = col->GetComponent<scene::BoxShapeComponent>();
		col->AddComponent<scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, physics::Layers::UNUSED4);
		col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Teleswapper.txt");
		col->AddComponent<Interactable>();
		GetCurrentScene()->GetEntityStorage().Load(col->GetEntityName(), col);

		col = scene::Entity::CreateEntity("BodyCollider");
		col->SetPosition(glm::vec3(16.f, 0.86f, -7.64f));
		col->SetRotation(glm::vec3(0.f, 27.04f, 0.f));
		col->SetScale(glm::vec3(2.02f, 0.491f, 1.18f));
		col->AddComponent<scene::BoxShapeComponent>(col->GetTransform().Scale(),
			col->GetTransform().Position(), col->GetTransform().EulerAngles());
		colShape = col->GetComponent<scene::BoxShapeComponent>();
		col->AddComponent<scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, physics::Layers::UNUSED4);
		col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Body.txt");
		col->AddComponent<Interactable>();
		GetCurrentScene()->GetEntityStorage().Load(col->GetEntityName(), col);

		col = scene::Entity::CreateEntity("TrashCollider");
		col->SetPosition(glm::vec3(15.98f, 0.86f, -10.26f));
		col->SetScale(glm::vec3(0.338f));
		col->AddComponent<scene::BoxShapeComponent>(col->GetTransform().Scale(),
			col->GetTransform().Position(), glm::vec3(0.f));
		colShape = col->GetComponent<scene::BoxShapeComponent>();
		col->AddComponent<scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, physics::Layers::UNUSED4);
		col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Body.txt");
		col->AddComponent<Interactable>();
		GetCurrentScene()->GetEntityStorage().Load(col->GetEntityName(), col);

		GetCurrentScene()->GetEntityStorage().Load("Player", player);
		GetCurrentScene()->GetEntityStorage().Load("Scene", scene);
		GetCurrentScene()->GetEntityStorage().Load("Scene 2", scene_NoCollision);
		GetCurrentScene()->GetEntityStorage().Load("Light", light);
		GetCurrentScene()->GetEntityStorage().Load("Light 2", light2);

		scene::Entity::DestroyEntity(GetCurrentScene()->GetEntityStorage().Get("Cube"));
		//GetCurrentScene()->GetEntityStorage().Load("Door", door);

		Rml::Debugger::Initialise(Engine::Instance().GetUiManager().GetContext("main"));
		//Rml::Debugger::SetVisible(true);

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

		/*auto pos = GetCurrentScene()->GetEntityStorage().Get("Player")->GetTransform().Position();
		BASED_TRACE("{} {} {}", pos.x, pos.y, pos.z);*/

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
