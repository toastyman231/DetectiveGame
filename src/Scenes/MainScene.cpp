#include "MainScene.h"

#include <filesystem>

#include "based/scene/components.h"
#include "based/scene/entity.h"
#include "based/graphics/model.h"
#include "../Audio/FMODSystem.h"
#include "../GameSystems.h"
#include "../KeyImageBindings.h"
#include "../Audio/AmbienceSystem.h"
#include "../Interaction/InteractionTrigger.h"
#include "based/input/mouse.h"

struct InteractableNote;
struct InteractionDialogueTrigger;
struct Tool;
struct MouseLook;

void MainScene::InitializeScene()
{
	Scene::InitializeScene();

	based::input::Mouse::SetCursorVisible(false);
	based::input::Mouse::SetCursorMode(based::input::CursorMode::Confined);

	FMODSystem::InitializeFMOD();
	FMODSystem::LoadBanks("Assets/FMOD/Banks");

	auto machinerySFX = FMODSystem::CreateFMODEvent("event:/Machine Interactions");
	auto paperSFX = FMODSystem::CreateFMODEvent("event:/PageTurning");

	LoadScene(ASSET_PATH("Scenes/Default3D.bscn"));

	auto cam = GetEntityStorage().Get("Main Camera");

	// ENTITY - PLAYER
	auto player = based::scene::Entity::CreateEntity("Player");
	player->AddComponent<based::scene::CapsuleShapeComponent>(1.8f, 0.4f);
	auto capsule = player->GetComponent<based::scene::CapsuleShapeComponent>();
	player->AddComponent<based::input::InputComponent>();
	auto& input = player->GetComponent<based::input::InputComponent>();
	based::Engine::Instance().GetInputManager().AddInputMapping(input, "IMC_Default", 1);
	auto innerCapsule = new JPH::CapsuleShape(1.8f, 0.4f);
	player->AddComponent<based::scene::CharacterController>(
		based::scene::Transform(glm::vec3(0)),
		capsule.shape, innerCapsule);
	player->AddComponent<MouseLook>(100.f);
	cam->SetParent(player, false);
	cam->SetLocalPosition({ 0, 1.8f - 0.4f, 0 });
	player->AddComponent<Tool>(6.f);
	player->SetPosition(glm::vec3(0.f, 10.f, 0.f));
	player->GetComponent<based::scene::CharacterController>()
		.Character->SetListener(reinterpret_cast<JPH::CharacterContactListener*>(&GameSystems::mAmbienceSystem));

	/*auto spawn = GetCurrentScene()->GetEntityStorage().Get("PlayerSpawn");
	player->SetPosition(spawn->GetTransform().Position());*/

	// ENTITY - Geometry
	auto scene = based::scene::Entity::CreateEntity("Scene");
	scene->SetRotation(glm::vec3(-90.f, 0.f, 0.f));
	scene->SetPosition(glm::vec3(0.f, 0.5f, 0.f));
	auto sceneModel = based::graphics::Model::CreateModel(
		"Assets/Models/ApartmentScene_StaticGeo.fbx",
		GetModelStorage(), "ApartmentScene");
	scene->AddComponent<based::scene::ModelRenderer>(sceneModel);
	sceneModel->GenerateModelCollisions(scene->GetTransform());

	auto scene_NoCollision = based::scene::Entity::CreateEntity("Scene 2");
	scene_NoCollision->SetPosition(glm::vec3(0.f, 0.5f, 0.f));
	auto noCollisionModel = based::graphics::Model::CreateModel(
		"Assets/Models/ApartmentScene_NoCollisionGeo.fbx",
		GetModelStorage(), "ApartmentScene_NoCollision");
	scene_NoCollision->AddComponent<based::scene::ModelRenderer>(noCollisionModel);

	// ENTITIES - Ambience
	auto ambience = based::scene::Entity::CreateEntity("Ambience");
	ambience->SetPosition(glm::vec3(-0.18f, 2.55f, -3.4f));
	ambience->SetScale(glm::vec3(3.45f, 2.88f, 8.21f));
	ambience->AddComponent<AmbienceArea>(1.f);
	ambience->AddComponent<based::scene::BoxShapeComponent>(
		ambience->GetTransform().Scale(),
		ambience->GetTransform().Position(),
		glm::vec3(0.f));
	auto& colShape = ambience->GetComponent<based::scene::BoxShapeComponent>();
	ambience->AddComponent<based::scene::TriggerComponent>(colShape, JPH::EMotionType::Static);
	GetEntityStorage().Load(ambience->GetEntityName(), ambience);

	auto ambience2 = based::scene::Entity::CreateEntity("Ambience 2");
	ambience2->SetPosition(glm::vec3(6.82f, 2.55f, -6.1f));
	ambience2->SetScale(glm::vec3(3.45f, 2.88f, 5.52f));
	ambience2->AddComponent<AmbienceArea>(0.f);
	ambience2->AddComponent<based::scene::BoxShapeComponent>(
		ambience2->GetTransform().Scale(),
		ambience2->GetTransform().Position(),
		glm::vec3(0.f));
	colShape = ambience2->GetComponent<based::scene::BoxShapeComponent>();
	ambience2->AddComponent<based::scene::TriggerComponent>(colShape, JPH::EMotionType::Static);
	GetEntityStorage().Load(ambience2->GetEntityName(), ambience2);

	// ENTITIES - Lights
	auto light = based::scene::Entity::CreateEntity("Light");
	light->AddComponent<based::scene::PointLight>(1.f, 0.09f, 0.032f, 1.0f, glm::vec3(1.f));
	/*light->AddComponent<scene::MeshRenderer>(
		graphics::Mesh::LoadMeshFromFile(ASSET_PATH("Meshes/cube.obj"),
			GetCurrentScene()->GetMeshStorage()),
		graphics::Material::LoadMaterialFromFile(ASSET_PATH("Materials/Unlit.bmat"),
			GetCurrentScene()->GetMaterialStorage()));*/
	light->SetScale(glm::vec3(0.3f));
	light->SetPosition(glm::vec3(-0.173f, 5.f, -3.f));

	auto light2 = based::scene::Entity::CreateEntity("Light 2");
	light2->AddComponent<based::scene::PointLight>(1.f, 0.09f, 0.032f, 3.0f, glm::vec3(1.f));
	/*light2->AddComponent<scene::MeshRenderer>(
		graphics::Mesh::LoadMeshFromFile(ASSET_PATH("Meshes/cube.obj"),
			GetCurrentScene()->GetMeshStorage()),
		graphics::Material::LoadMaterialFromFile(ASSET_PATH("Materials/Unlit.bmat"),
			GetCurrentScene()->GetMaterialStorage()));*/
	light2->SetScale(glm::vec3(0.3f));
	light2->SetPosition(glm::vec3(16.6f, 5.f, -6.f));

	/*auto dl = GetCurrentScene()->GetEntityStorage().Get("Directional Light")
		->GetComponent<scene::DirectionalLight>().intensity = 10.f;*/

	// ENTITY - Painting
	auto col = based::scene::Entity::CreateEntity("PaintingCollider");
	col->SetPosition(glm::vec3(3.84f, 0.9f, -5.77f));
	col->SetScale(glm::vec3(0.482f, 1.f, 1.48f));
	col->AddComponent<based::scene::BoxShapeComponent>(col->GetTransform().Scale(),
	                                                   col->GetTransform().Position(), glm::vec3(0.f));
	colShape = col->GetComponent<based::scene::BoxShapeComponent>();
	col->AddComponent<based::scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, based::physics::Layers::UNUSED4);
	col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Painting.txt");
	col->AddComponent<Interactable>();
	GetEntityStorage().Load(col->GetEntityName(), col);

	// ENTITY - Research
	col = based::scene::Entity::CreateEntity("ResearchCollider");
	col->SetPosition(glm::vec3(4.94f, 2.63f, -2.16f));
	col->SetScale(glm::vec3(0.642f, 0.295f, 0.554f));
	col->AddComponent<based::scene::BoxShapeComponent>(col->GetTransform().Scale(),
	                                                   col->GetTransform().Position(), glm::vec3(0.f));
	colShape = col->GetComponent<based::scene::BoxShapeComponent>();
	col->AddComponent<based::scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, based::physics::Layers::UNUSED4);
	col->AddComponent<InteractableNote>(std::filesystem::path("Assets/Notes/ResearchNotes1.txt"));
	col->AddComponent<Interactable>();
	GetEntityStorage().Load(col->GetEntityName(), col);

	// ENTITY - Tools
	col = based::scene::Entity::CreateEntity("ToolsCollider");
	col->SetPosition(glm::vec3(8.39f, 2.88f, -2.16f));
	col->SetScale(glm::vec3(1.061f, 0.295f, 0.554f));
	col->AddComponent<based::scene::BoxShapeComponent>(col->GetTransform().Scale(),
	                                                   col->GetTransform().Position(), glm::vec3(0.f));
	colShape = col->GetComponent<based::scene::BoxShapeComponent>();
	col->AddComponent<based::scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, based::physics::Layers::UNUSED4);
	col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Tools.txt");
	col->AddComponent<Interactable>();
	GetEntityStorage().Load(col->GetEntityName(), col);

	// ENTITY - Chair
	col = based::scene::Entity::CreateEntity("ChairCollider");
	col->SetPosition(glm::vec3(9.13f, 0.68f + 0.5f, -4.62f));
	col->SetRotation(glm::vec3(0.f, 45.f, 0.f));
	col->SetScale(glm::vec3(1.62f, 0.727f, 0.846f));
	col->AddComponent<based::scene::BoxShapeComponent>(col->GetTransform().Scale(),
	                                                   col->GetTransform().Position(), col->GetTransform().EulerAngles());
	colShape = col->GetComponent<based::scene::BoxShapeComponent>();
	col->AddComponent<based::scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, based::physics::Layers::UNUSED4);
	col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Chair.txt");
	col->AddComponent<Interactable>();
	GetEntityStorage().Load(col->GetEntityName(), col);

	// ENTITY - Hidden Notes
	col = based::scene::Entity::CreateEntity("HiddenNotesCollider");
	col->SetPosition(glm::vec3(13.9f, 0.82f, -3.84f));
	col->SetScale(glm::vec3(0.481f));
	col->AddComponent<based::scene::BoxShapeComponent>(col->GetTransform().Scale(),
	                                                   col->GetTransform().Position(), glm::vec3(0.f));
	colShape = col->GetComponent<based::scene::BoxShapeComponent>();
	col->AddComponent<based::scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, based::physics::Layers::UNUSED4);
	col->AddComponent<InteractableNote>(std::filesystem::path("Assets/Notes/ResearchNotes2.txt"));
	col->AddComponent<Interactable>();
	GetEntityStorage().Load(col->GetEntityName(), col);

	// ENTITY - Teleswapper
	col = based::scene::Entity::CreateEntity("TeleswapperCollider");
	col->SetPosition(glm::vec3(18.02f, 2.95f, -9.38f));
	col->SetScale(glm::vec3(0.676f, 0.380f, 0.676f));
	col->AddComponent<based::scene::BoxShapeComponent>(col->GetTransform().Scale(),
	                                                   col->GetTransform().Position(), glm::vec3(0.f));
	colShape = col->GetComponent<based::scene::BoxShapeComponent>();
	col->AddComponent<based::scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, based::physics::Layers::UNUSED4);
	col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Teleswapper.txt");
	col->AddComponent<Interactable>(std::vector<SolutionWord>{}, machinerySFX);
	GetEntityStorage().Load(col->GetEntityName(), col);

	// ENTITY - Body
	col = based::scene::Entity::CreateEntity("BodyCollider");
	col->SetPosition(glm::vec3(16.f, 0.86f, -7.64f));
	col->SetRotation(glm::vec3(0.f, 27.04f, 0.f));
	col->SetScale(glm::vec3(2.02f, 0.491f, 1.18f));
	col->AddComponent<based::scene::BoxShapeComponent>(col->GetTransform().Scale(),
	                                                   col->GetTransform().Position(), col->GetTransform().EulerAngles());
	colShape = col->GetComponent<based::scene::BoxShapeComponent>();
	col->AddComponent<based::scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, based::physics::Layers::UNUSED4);
	col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Body.txt");
	col->AddComponent<Interactable>();
	GetEntityStorage().Load(col->GetEntityName(), col);

	// Entity - Trash
	col = based::scene::Entity::CreateEntity("TrashCollider");
	col->SetPosition(glm::vec3(15.98f, 0.86f, -10.26f));
	col->SetScale(glm::vec3(0.338f));
	col->AddComponent<based::scene::BoxShapeComponent>(col->GetTransform().Scale(),
	                                                   col->GetTransform().Position(), glm::vec3(0.f));
	colShape = col->GetComponent<based::scene::BoxShapeComponent>();
	col->AddComponent<based::scene::RigidbodyComponent>(colShape, JPH::EMotionType::Static, based::physics::Layers::UNUSED4);
	col->AddComponent<InteractionDialogueTrigger>("Assets/Dialogue/Body.txt");
	col->AddComponent<Interactable>(std::vector<SolutionWord>{}, paperSFX);
	GetEntityStorage().Load(col->GetEntityName(), col);

	GetEntityStorage().Load("Player", player);
	GetEntityStorage().Load("Scene", scene);
	GetEntityStorage().Load("Scene 2", scene_NoCollision);
	GetEntityStorage().Load("Light", light);
	GetEntityStorage().Load("Light 2", light2);

	based::scene::Entity::DestroyEntity(GetEntityStorage().Get("Cube"));
	//GetCurrentScene()->GetEntityStorage().Load("Door", door);

	auto context = based::Engine::Instance().GetUiManager().GetContext("main");
	based::Engine::Instance().GetUiManager().SetPathPrefix("Assets/UI/");
	auto doc = based::Engine::Instance().GetUiManager().LoadWindow("PlayerHUD", context, "PlayerHUD");
	auto interactImage = doc->document->GetElementById("interact-image");
	auto binding = based::ui::ElementBinding(interactImage,
		[this](Rml::Element* elem)
		{
			auto input = based::managers::InputManager::GetInputComponentForPlayer(0);
			if (!input || !elem || !elem->IsVisible()) return;

			auto keyName = input->GetKeyImageForAction("IA_Interact", mKeyMaps);

			elem->SetAttribute("sprite", keyName);
		});
	based::Engine::Instance().GetUiManager().AddBinding(binding);

	GameSystems::mToolSystem.Initialize();
	GameSystems::mDialogueSystem.Initialize();
	GameSystems::mNoteSystem.Initialize();
	GameSystems::mSolutionPanelSystem.Initialize();
	GameSystems::mPlayerController.Initialize();
	GameSystems::mMouseLookSystem.Initialize();
	GameSystems::mAmbienceSystem.Initialize();

#ifdef BASED_CONFIG_DEBUG
	GameSystems::mSoundTest.Initialize();
#endif
}

void MainScene::UpdateScene(float deltaTime)
{
	Scene::UpdateScene(deltaTime);

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

void MainScene::ShutdownScene()
{
	Scene::ShutdownScene();

	GameSystems::mAmbienceSystem.Shutdown();
}
