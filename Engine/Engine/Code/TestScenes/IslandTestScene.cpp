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

IslandTestScene::IslandTestScene()
{
	// get systems
	CameraManager& CM  = *Systems::cameraManager;
	LightManager& LM   = *Systems::lightManager;
	Renderer& renderer = *Systems::renderer;

	// create shadowMap
	Entity* shadowMapRenderer = renderer.CreateShadowMap(200.0f, 8192.0f, XMFLOAT3(-6, 325, 9), XMFLOAT3(85.0f, -90.0f, 0));

	// create skybox
	_skyDome = renderer.CreateSkyBox(L"SkyBoxes/ThickCloudsWater.dds", SKY_DOME_RENDER_MODE::CUBEMAP_COLOR_BLEND);

	// set skybox properties
	_skyDome->SetSunDirectionTransformPtr(shadowMapRenderer->GetComponent<TransformComponent>());

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
}

IslandTestScene::~IslandTestScene()
{
}
