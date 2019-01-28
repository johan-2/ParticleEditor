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
#include "RotationComponent.h"
#include "ShaderHelpers.h"
#include "PlanarReflectionShader.h"
#include "PostProcessingShader.h"
#include "InstancedModel.h"
#include "MathHelpers.h"
#include "UVScrollComponent.h"

SponzaTestScene::SponzaTestScene()
{
	// get systems
	CameraManager& CM  = *Systems::cameraManager;
	LightManager& LM   = *Systems::lightManager;
	Renderer& renderer = *Systems::renderer;

	// create shadowMap
	Entity* shadowMapRenderer = renderer.CreateShadowMap(150.0f, 8192.0f, XMFLOAT3(-6, 325, 9), XMFLOAT3(85.0f, -90.0f, 0), true);

	// create skybox
	_skyDome = renderer.skyDome = new SkyDome("Settings/SkyDomeDefault.json");

	renderer.ShowGBufferDebugImages();
	renderer.planarReflectionShader->ShowDebugQuads();
	renderer.postProcessingShader->ShowBloomBlurP2DebugQuad();

	// create game camera
	Entity* cameraGame = new Entity();
	cameraGame->AddComponent<TransformComponent>()->Init(XMFLOAT3(-32.28f, 6.22f, 0.20f), XMFLOAT3(13.0f, 89.5f, 0.0f));
	cameraGame->AddComponent<CameraComponent>()->Init3D(90);
	cameraGame->AddComponent<FreeMoveComponent>()->init(12.0f, 0.25f, 6.0f);
	CM.currentCameraGame = cameraGame->GetComponent<CameraComponent>();

	// create UIcamera
	Entity* cameraUI = new Entity();
	cameraUI->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, -1));
	cameraUI->AddComponent<CameraComponent>()->Init2D(XMFLOAT2(SystemSettings::SCREEN_WIDTH, SystemSettings::SCREEN_HEIGHT), XMFLOAT2(0.01f, 10.0f));
	CM.currentCameraUI = cameraUI->GetComponent<CameraComponent>();

	// set ambient light color	
	LM.SetAmbientColor(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));

	// create directional light and give it pointer to the depth render camera transform
	// it will use the forward of this camera as the light direction
	Entity* directionalLight = new Entity;
	directionalLight->AddComponent<LightDirectionComponent>()->Init(XMFLOAT4(0.8f, 0.8f, 0.8f, 1), shadowMapRenderer->GetComponent<TransformComponent>());

	Entity* sponza = new Entity();
	sponza->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0.025f, 0.025f, 0.025f));
	sponza->AddComponent<ModelComponent>()->InitModel("Models/sponza.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION | REFRACT, L"Textures/marble.dds", L"", L"", L"", true, 1);

	Entity* POM = new Entity();
	POM->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 5, 40), XMFLOAT3(0, 0, 90), XMFLOAT3(3, 1, 3));
	POM->AddComponent<ModelComponent>()->InitModel("Models/plane.obj", STANDARD | CAST_REFLECTION, L"Textures/Stone1.dds", L"Textures/stone1Normal_H.dds", L"", L"", false, 3, 0.15f);

	Entity* noPom = new Entity();
	noPom->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 5, 60), XMFLOAT3(0, 0, 90), XMFLOAT3(3, 1, 3));
	noPom->AddComponent<ModelComponent>()->InitModel("Models/plane.obj", STANDARD | CAST_REFLECTION, L"Textures/Stone1.dds", L"Textures/stone1Normal.dds", L"", L"", false, 3);

	Entity* sphere = new Entity();
	sphere->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 2.5f, 0.5f), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
	sphere->AddComponent<ModelComponent>()->InitModel("Models/sphere.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveTest.dds");
	sphere->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(0.1f, 1.0f, 0), 0.0f, 1.0f, 0.2f);

	Entity* bar = new Entity();
	bar->AddComponent<TransformComponent>()->Init(XMFLOAT3(-20, 2.5f, 0.5f), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 0.5f, 0.5f));
	bar->AddComponent<ModelComponent>()->InitModel("Models/cube.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION | REFRACT, L"", L"", L"", L"Textures/emissiveOrange.dds");
	bar->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(1.0f, 0.4f, 0.15f), 0.0f, 1.0f, 0.2f);
	bar->AddComponent<RotationComponent>()->Init(XMFLOAT3(2, 0.0f, 1.5f), 40);

	Entity* bar2 = new Entity();
	bar2->AddComponent<TransformComponent>()->Init(XMFLOAT3(20, 2.5f, 0.5f), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 0.5f, 0.1f));
	bar2->AddComponent<ModelComponent>()->InitModel("Models/cube.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION | REFRACT, L"", L"", L"", L"Textures/emissiveOrange.dds");
	bar2->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(1.0f, 0.4f, 0.15f), 0.0f, 1.0f, 0.2f);
	bar2->AddComponent<RotationComponent>()->Init(XMFLOAT3(2, 1.0f, 1.5f), 60);

	Entity* sphere2 = new Entity();
	sphere2->AddComponent<TransformComponent>()->Init(XMFLOAT3(-30.0f, 3.5f, -10.12f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.5f, 2.5f, 2.5f));
	sphere2->AddComponent<ModelComponent>()->InitModel("Models/sphere.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissivePurple.dds");
	sphere2->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(0.8f, 0.2f, 0.8f), 0.0f, 1.0f, 0.2f);

	Entity* sphere3 = new Entity();
	sphere3->AddComponent<TransformComponent>()->Init(XMFLOAT3(28.0f, 3.5, -10.12f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.5f, 2.5f, 2.5f));
	sphere3->AddComponent<ModelComponent>()->InitModel("Models/sphere.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveRed.dds");
	sphere3->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f, 0.2f);

	Entity* sphere4 = new Entity();
	sphere4->AddComponent<TransformComponent>()->Init(XMFLOAT3(-30, 3.5f, 11.25f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.5f, 2.5f, 2.5f));
	sphere4->AddComponent<ModelComponent>()->InitModel("Models/sphere.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveBlue.dds");
	sphere4->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(0.0f, 0.4f, 0.95f), 0.0f, 1.0f, 0.2f);

	Entity* sphere5 = new Entity();
	sphere5->AddComponent<TransformComponent>()->Init(XMFLOAT3(28.0f, 3.5, 11.25f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.5f, 2.5f, 2.5f));
	sphere5->AddComponent<ModelComponent>()->InitModel("Models/sphere.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissivePink.dds");
	sphere5->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(1.0f, 0.5f, 0.85f), 0.0f, 1.0f, 0.2f);

	Entity* floor = new Entity();
	floor->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0.01f, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(600, 1, 600));
	floor->AddComponent<ModelComponent>()->InitModel("Models/plane.obj", ALPHA_REFLECTION, L"Textures/sponza_floor_a_diff.dds", L"Textures/sponza_floor_a_ddn.dds", L"Textures/FlatHighSpecular.dds", L"", false, 800.0f);
	floor->GetComponent<ModelComponent>()->GetMeshes()[0]->SetReflectionData(Mesh::ReflectiveData(0.25f));

	/*Entity* water = new Entity();
	water->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 2.0f, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(400, 1, 400));
	water->AddComponent<ModelComponent>()->InitModel("models/plane.obj", ALPHA_WATER, L"", L"Textures/waterNormal.dds", L"Textures/FlatHighSpecular.dds", L"", false, 400.0f);
	water->AddComponent<UVScrollComponent>()->Init(XMFLOAT2(0.015f, -0.01f));
	water->GetComponent<ModelComponent>()->SetUVDVMap(L"Textures/waterDUDV.dds");
	water->GetComponent<ModelComponent>()->SetFoamMap(L"Textures/foam3.dds");
	water->GetComponent<ModelComponent>()->SetNoiseMap(L"Textures/perlinNoise2.dds");*/

	Entity* dust = new Entity();
	dust->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 10.0f, 0.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
	dust->AddComponent<ParticleSystemComponent>()->Init("Particles/dust.json");

	Entity* fire = new Entity();
	fire->AddComponent<TransformComponent>()->Init(XMFLOAT3(30.0f, 0.0f, -3.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.13, 0.13, 0.13));
	fire->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire->AddComponent<LightPointComponent>()->Init(4.5f, 4, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.1f);
	fire->AddComponent<ModelComponent>()->InitModel("Models/fogata1.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION);

	Entity* fire2 = new Entity();
	fire2->AddComponent<TransformComponent>()->Init(XMFLOAT3(30.0f, 0.0f, 6.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.13, 0.13, 0.13));
	fire2->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire2->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);
	fire2->AddComponent<ModelComponent>()->InitModel("Models/fogata1.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION);

	Entity* fire7 = new Entity();
	fire7->AddComponent<TransformComponent>()->Init(XMFLOAT3(-32.5f, 0.0f, -3.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.13, 0.13, 0.13));
	fire7->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire7->AddComponent<LightPointComponent>()->Init(4.5f, 4, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.1f);
	fire7->AddComponent<ModelComponent>()->InitModel("Models/fogata1.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION);

	Entity* fire8 = new Entity();
	fire8->AddComponent<TransformComponent>()->Init(XMFLOAT3(-32.5f, 0.0f, 6.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.13, 0.13, 0.13));
	fire8->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire8->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);
	fire8->AddComponent<ModelComponent>()->InitModel("Models/fogata1.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION);

	Entity* fire3 = new Entity();
	fire3->AddComponent<TransformComponent>()->Init(XMFLOAT3(12.25f, 2.75f, -3.5f));
	fire3->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire3->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	Entity* fire4 = new Entity();
	fire4->AddComponent<TransformComponent>()->Init(XMFLOAT3(12.25f, 2.75f, 5.5f));
	fire4->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire4->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	Entity* fire5 = new Entity();
	fire5->AddComponent<TransformComponent>()->Init(XMFLOAT3(-15.5f, 2.75f, -3.5f));
	fire5->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire5->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	Entity* fire6 = new Entity();
	fire6->AddComponent<TransformComponent>()->Init(XMFLOAT3(-15.5f, 2.75f, 5.5f));
	fire6->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire6->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	InstancedModel* instancedModel = new InstancedModel("Models/cube.obj", INSTANCED_OPAQUE | INSTANCED_CAST_SHADOW_DIR | INSTANCED_CAST_REFLECTION, L"Textures/stone2.dds", L"Textures/stone2Normal_H.dds", L"Textures/stone2Specular.dds", L"", false, 4.0f, 0.05f);

	std::vector<ModelInstance> instances;
	float spacing = 2.0f;
	for (int i = 0; i < 64; i++)
		for (int y = 0; y < 64; y++)		
			instances.emplace_back((ModelInstance(MATH_HELPERS::CreateWorldMatrix(XMFLOAT3(40 + (i * spacing), 2.5f, 40 + (y * spacing)), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)))));
		
	instancedModel->BuildInstanceBuffer(instances);	
}

SponzaTestScene::~SponzaTestScene()
{
}

void SponzaTestScene::Update()
{
	_skyDome->Update(Systems::time->GetDeltaTime());	
}
