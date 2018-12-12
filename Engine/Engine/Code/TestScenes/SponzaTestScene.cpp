#include "SponzaTestScene.h"
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

SponzaTestScene::SponzaTestScene()
{
	// get systems
	CameraManager& CM  = *Systems::cameraManager;
	LightManager& LM   = *Systems::lightManager;
	Renderer& renderer = *Systems::renderer;

	// create shadowMap
	Entity* shadowMapRenderer = renderer.CreateShadowMap(250.0f, 8192.0f, XMFLOAT3(-6, 325, 9), XMFLOAT3(85.0f, -90.0f, 0));

	// create skybox
	SkyBox* skyBox = renderer.CreateSkyBox(L"SkyBoxes/ThickCloudsWater.dds", SKY_DOME_RENDER_MODE::CUBEMAP_COLOR_BLEND);

	// set skybox properties
	skyBox->SetSunDirectionTransformPtr(shadowMapRenderer->GetComponent<TransformComponent>());
	skyBox->SetSunDistance(5.0f);
	skyBox->SetCubeMapColorBlendSettings(XMFLOAT4(21, 90, 251, 255), XMFLOAT4(199, 176, 135, 255), 40, 55, 80, false);

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

	//set ambient light color	
	LM.SetAmbientColor(XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f));

	// create directional light and give it pointer to the depth render camera transform
	// it will use the forward of this camera as the light direction
	Entity* directionalLight = new Entity;
	directionalLight->AddComponent<LightDirectionComponent>()->Init(XMFLOAT4(0.8f, 0.8f, 0.8f, 1), shadowMapRenderer->GetComponent<TransformComponent>());

	Entity* sponza = new Entity();
	sponza->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0.05f, 0.05f, 0.05f));
	sponza->AddComponent<ModelComponent>()->InitModel("Models/sponza.obj", DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION, L"Textures/marble.dds", L"", L"", L"", true, 1);

	Entity* sphere = new Entity();
	sphere->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 5, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 2, 2));
	sphere->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::SPHERE, DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveTest.dds");
	sphere->AddComponent<LightPointComponent>()->Init(20, 20, XMFLOAT3(0.1, 1.0f, 0), 0.0f, 1.0f, 0.2f);

	Entity* sphere2 = new Entity();
	sphere2->AddComponent<TransformComponent>()->Init(XMFLOAT3(-60, 7, -20.25f), XMFLOAT3(0, 0, 0), XMFLOAT3(4, 4, 4));
	sphere2->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::SPHERE, DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissivePurple.dds");
	sphere2->AddComponent<LightPointComponent>()->Init(20, 20, XMFLOAT3(0.8, 0.2f, 0.8), 0.0f, 1.0f, 0.2f);

	Entity* sphere3 = new Entity();
	sphere3->AddComponent<TransformComponent>()->Init(XMFLOAT3(56.0f, 7, -20.25f), XMFLOAT3(0, 0, 0), XMFLOAT3(4, 4, 4));
	sphere3->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::SPHERE, DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveRed.dds");
	sphere3->AddComponent<LightPointComponent>()->Init(20, 20, XMFLOAT3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f, 0.2f);

	Entity* sphere4 = new Entity();
	sphere4->AddComponent<TransformComponent>()->Init(XMFLOAT3(-60, 7, 22.5f), XMFLOAT3(0, 0, 0), XMFLOAT3(4, 4, 4));
	sphere4->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::SPHERE, DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveBlue.dds");
	sphere4->AddComponent<LightPointComponent>()->Init(20, 20, XMFLOAT3(0.0f, 0.4f, 0.95f), 0.0f, 1.0f, 0.2f);

	Entity* sphere5 = new Entity();
	sphere5->AddComponent<TransformComponent>()->Init(XMFLOAT3(56.0f, 7, 22.5f), XMFLOAT3(0, 0, 0), XMFLOAT3(4, 4, 4));
	sphere5->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::SPHERE, DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissivePink.dds");
	sphere5->AddComponent<LightPointComponent>()->Init(20, 20, XMFLOAT3(1.0f, 0.5f, 0.85f), 0.0f, 1.0f, 0.2f);

	Entity* floor = new Entity();
	floor->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0.05f, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(50, 1, 50));
	floor->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::PLANE, ALPHA_REFLECTION, L"Textures/sponza_floor_a_diff.dds", L"Textures/sponza_floor_a_ddn.dds", L"Textures/FlatHighSpecular.dds", L"", 20.0f);
	floor->GetComponent<ModelComponent>()->GetMeshes()[0]->SetReflectionData(Mesh::ReflectiveData(0.2f, true, true));

	Entity* fire = new Entity();
	fire->AddComponent<TransformComponent>()->Init(XMFLOAT3(60.0f, 0.0f, -6.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.25, 0.25, 0.25));
	fire->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire->AddComponent<LightPointComponent>()->Init(9, 8, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.1f);
	fire->AddComponent<ModelComponent>()->InitModel("Models/fogata1.obj", DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION);

	Entity* fire2 = new Entity();
	fire2->AddComponent<TransformComponent>()->Init(XMFLOAT3(60.0f, 0.0f, 12.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.25, 0.25, 0.25));
	fire2->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire2->AddComponent<LightPointComponent>()->Init(9, 10, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);
	fire2->AddComponent<ModelComponent>()->InitModel("Models/fogata1.obj", DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION);

	Entity* fire7 = new Entity();
	fire7->AddComponent<TransformComponent>()->Init(XMFLOAT3(-65.0f, 0.0f, -6.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.25, 0.25, 0.25));
	fire7->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire7->AddComponent<LightPointComponent>()->Init(9, 8, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.1f);
	fire7->AddComponent<ModelComponent>()->InitModel("Models/fogata1.obj", DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION);

	Entity* fire8 = new Entity();
	fire8->AddComponent<TransformComponent>()->Init(XMFLOAT3(-65.0f, 0.0f, 12.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.25, 0.25, 0.25));
	fire8->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire8->AddComponent<LightPointComponent>()->Init(9, 10, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);
	fire8->AddComponent<ModelComponent>()->InitModel("Models/fogata1.obj", DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION);

	Entity* fire3 = new Entity();
	fire3->AddComponent<TransformComponent>()->Init(XMFLOAT3(24.5f, 5.5f, -7.0f));
	fire3->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire3->AddComponent<LightPointComponent>()->Init(9, 10, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	Entity* fire4 = new Entity();
	fire4->AddComponent<TransformComponent>()->Init(XMFLOAT3(24.5f, 5.5f, 11.0f));
	fire4->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire4->AddComponent<LightPointComponent>()->Init(9, 10, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	Entity* fire5 = new Entity();
	fire5->AddComponent<TransformComponent>()->Init(XMFLOAT3(-31.0f, 5.5f, -7.0f));
	fire5->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire5->AddComponent<LightPointComponent>()->Init(9, 10, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	Entity* fire6 = new Entity();
	fire6->AddComponent<TransformComponent>()->Init(XMFLOAT3(-31.0f, 5.5f, 11.0f));
	fire6->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire6->AddComponent<LightPointComponent>()->Init(9, 10, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);
}

SponzaTestScene::~SponzaTestScene()
{
}
