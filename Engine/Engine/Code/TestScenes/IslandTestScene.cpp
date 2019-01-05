#include "IslandTestScene.h"
#include "DXManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Renderer.h"
#include "Systems.h"
#include "SystemDefs.h"
#include "Mesh.h"
#include "ModelComponent.h"
#include "ParticleSystemComponent.h"
#include "LightPointComponent.h"
#include "CameraComponent.h"
#include "FreeMoveComponent.h"
#include "UVScrollComponent.h"

IslandTestScene::IslandTestScene()
{
	// get systems
	CameraManager& CM  = *Systems::cameraManager;
	LightManager& LM   = *Systems::lightManager;
	Renderer& renderer = *Systems::renderer;

	// create shadowMap
	Entity* shadowMapRenderer = renderer.CreateShadowMap(250.0f, 8192.0f, XMFLOAT3(-6, 325, 9), XMFLOAT3(85.0f, -90.0f, 0));

	// create skybox
	_skyDome = renderer.CreateSkyDome("Settings/SkyDomeDefault.json");

	renderer.CreateDebugImages();

	// create game camera
	Entity* cameraGame = new Entity();
	cameraGame->AddComponent<TransformComponent>()->Init(XMFLOAT3(-64.28f, 12.22f, 0.41f), XMFLOAT3(13.0f, 89.5f, 0.0f));
	cameraGame->AddComponent<CameraComponent>()->Init3D(70);
	cameraGame->AddComponent<FreeMoveComponent>()->init(30.0f, 0.25f);
	CM.SetCurrentCameraGame(cameraGame->GetComponent<CameraComponent>());

	// create UIcamera
	Entity* cameraUI = new Entity();
	cameraUI->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, -1));
	cameraUI->AddComponent<CameraComponent>()->Init2D(XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), XMFLOAT2(0.01f, 10.0f));
	CM.SetCurrentCameraUI(cameraUI->GetComponent<CameraComponent>());

	// set ambient light color	
	LM.SetAmbientColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));

	// create directional light and give it pointer to the depth render camera transform
	// it will use the forward of this camera as the light direction
	Entity* directionalLight = new Entity;
	directionalLight->AddComponent<LightDirectionComponent>()->Init(XMFLOAT4(0.8f, 0.8f, 0.8f, 1), shadowMapRenderer->GetComponent<TransformComponent>());

	Entity* lake = new Entity();
	lake->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0.5f, 0.5f, 0.5f));
	lake->AddComponent<ModelComponent>()->InitModel("Models/terrain.fbx", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION | REFRACT, L"Textures/Dirt_21_Diffuse.dds", L"Textures/Dirt_21_Normal.dds", L"Textures/Dirt_21_Specular.dds", L"", false, 6);

	Entity* water = new Entity();
	water->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 5.0f, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(50, 1, 50));
	water->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::PLANE, ALPHA_WATER, L"", L"Textures/waterNormal.dds", L"Textures/FlatHighSpecular.dds", L"", 10.0f);
	water->AddComponent<UVScrollComponent>()->Init(XMFLOAT2(0.02f, -0.01f));
	water->GetComponent<ModelComponent>()->SetUVDVMap(L"Textures/waterDuDv.dds");

	Entity* tree = new Entity();
	tree->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0.0f, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0.1f, 0.1f, 0.1f));
	tree->AddComponent<ModelComponent>()->InitModel("Models/white_oak.obj", STANDARD | CAST_REFLECTION | CAST_SHADOW_DIR | REFRACT);

	Entity* sphere = new Entity();
	sphere->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 10, 10), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 2, 2));
	sphere->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::SPHERE, STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveTest.dds");
	sphere->AddComponent<LightPointComponent>()->Init(20, 20, XMFLOAT3(0.1f, 1.0f, 0), 0.0f, 1.0f, 0.2f);

	Entity* tree1 = new Entity();
	tree1->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 7.0f, 65), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0f, 1.0f, 1.0f));
	tree1->AddComponent<ModelComponent>()->InitModel("Models/Palm_01.obj", STANDARD | CAST_REFLECTION | CAST_SHADOW_DIR);
}

IslandTestScene::~IslandTestScene()
{
}

void IslandTestScene::Update(const float& delta)
{
	_skyDome->Update(delta);
}