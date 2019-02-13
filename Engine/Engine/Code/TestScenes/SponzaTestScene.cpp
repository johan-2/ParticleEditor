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
#include "PingPongComponent.h"

SponzaTestScene::SponzaTestScene()
{
	// get systems
	CameraManager& CM  = *Systems::cameraManager;
	LightManager& LM   = *Systems::lightManager;
	Renderer& renderer = *Systems::renderer;

	// create shadowMap
	Entity* shadowMapRenderer = renderer.CreateShadowMap(200.0f, 8192.0f, XMFLOAT3(-6, 325, 9), XMFLOAT3(85.0f, -90.0f, 0), true);

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
	LM.ambientColor = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	// create directional light and give it pointer to the depth render camera transform
	// it will use the forward of this camera as the light direction
	Entity* directionalLight = new Entity;
	directionalLight->AddComponent<LightDirectionComponent>()->Init(XMFLOAT4(0.8f, 0.8f, 0.8f, 1), shadowMapRenderer->GetComponent<TransformComponent>());

	Entity* sponza = new Entity();
	sponza->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0.025f, 0.025f, 0.025f));
	sponza->AddComponent<ModelComponent>()->InitModel("Models/sponza.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"Textures/marble.dds", L"", L"", L"", true, 1);

	Entity* smallSphereLight = new Entity();
	smallSphereLight->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 2.5f, 0.5f), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
	smallSphereLight->AddComponent<PingPongComponent>()->Init(XMFLOAT3(10.0f, 0.0f, 0.0f), 1.5f);
	smallSphereLight->AddComponent<ModelComponent>()->InitModel("Models/sphere.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveTest.dds");
	smallSphereLight->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(0.1f, 1.0f, 0), 0.0f, 1.0f, 0.2f);

	// spinning bar lights
	Entity* bar = new Entity();
	bar->AddComponent<TransformComponent>()->Init(XMFLOAT3(-20, 2.5f, 0.5f), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 0.5f, 0.5f));
	bar->AddComponent<ModelComponent>()->InitModel("Models/cube.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveOrange.dds");
	bar->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(1.0f, 0.4f, 0.15f), 0.0f, 1.0f, 0.2f);
	bar->AddComponent<RotationComponent>()->Init(XMFLOAT3(2, 0.0f, 1.5f), 40);

	Entity* bar2 = new Entity();
	bar2->AddComponent<TransformComponent>()->Init(XMFLOAT3(20, 2.5f, 0.5f), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 0.5f, 0.1f));
	bar2->AddComponent<ModelComponent>()->InitModel("Models/cube.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveOrange.dds");
	bar2->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(1.0f, 0.4f, 0.15f), 0.0f, 1.0f, 0.2f);
	bar2->AddComponent<RotationComponent>()->Init(XMFLOAT3(2, 1.0f, 1.5f), 60);

	// big sphere lights
	Entity* BigSphereLight1 = new Entity();
	BigSphereLight1->AddComponent<TransformComponent>()->Init(XMFLOAT3(-30.0f, 3.5f, -10.12f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.5f, 2.5f, 2.5f));
	BigSphereLight1->AddComponent<PingPongComponent>()->Init(XMFLOAT3(0.0f, 0.1f, 0.0f), 1.5f);
	BigSphereLight1->AddComponent<ModelComponent>()->InitModel("Models/sphere.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissivePurple.dds");
	BigSphereLight1->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(0.8f, 0.2f, 0.8f), 0.0f, 1.0f, 0.2f);

	Entity* BigSphereLight2 = new Entity();
	BigSphereLight2->AddComponent<TransformComponent>()->Init(XMFLOAT3(28.0f, 3.5, -10.12f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.5f, 2.5f, 2.5f));
	BigSphereLight2->AddComponent<PingPongComponent>()->Init(XMFLOAT3(0.0f, 0.1f, 0.0f), 1.5f);
	BigSphereLight2->AddComponent<ModelComponent>()->InitModel("Models/sphere.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveRed.dds");
	BigSphereLight2->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f, 0.2f);

	Entity* BigSphereLight3 = new Entity();
	BigSphereLight3->AddComponent<TransformComponent>()->Init(XMFLOAT3(-30, 3.5f, 11.25f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.5f, 2.5f, 2.5f));
	BigSphereLight3->AddComponent<PingPongComponent>()->Init(XMFLOAT3(0.0f, 0.1f, 0.0f), 1.5f);
	BigSphereLight3->AddComponent<ModelComponent>()->InitModel("Models/sphere.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveBlue.dds");
	BigSphereLight3->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(0.0f, 0.4f, 0.95f), 0.0f, 1.0f, 0.2f);

	Entity* BigSphereLight4 = new Entity();
	BigSphereLight4->AddComponent<TransformComponent>()->Init(XMFLOAT3(28.0f, 3.5, 11.25f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.5f, 2.5f, 2.5f));
	BigSphereLight4->AddComponent<PingPongComponent>()->Init(XMFLOAT3(0.0f, 0.1f, 0.0f), 1.5f);
	BigSphereLight4->AddComponent<ModelComponent>()->InitModel("Models/sphere.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"", L"", L"", L"Textures/emissivePink.dds");
	BigSphereLight4->AddComponent<LightPointComponent>()->Init(10, 10, XMFLOAT3(1.0f, 0.5f, 0.85f), 0.0f, 1.0f, 0.2f);

	// create reflective marble floor
	Entity* floor = new Entity();
	floor->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0.02f, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(600, 1, 600));
	floor->AddComponent<ModelComponent>()->InitModel("Models/plane.obj", ALPHA_REFLECTION | REFRACT, L"Textures/sponza_floor_a_diff.dds", L"Textures/sponza_floor_a_ddn.dds", L"Textures/FlatHighSpecular.dds", L"", false, 800.0f);
	floor->GetComponent<ModelComponent>()->meshes[0]->planarReflectionFraction = 0.25f;

	// create water
	Entity* water = new Entity();
	water->AddComponent<TransformComponent>()->Init(XMFLOAT3(-30.0f, 1.75f, 50), XMFLOAT3(0, 0, 0), XMFLOAT3(4, 1, 4));
	water->AddComponent<ModelComponent>()->InitModel("models/waterPlane.obj", ALPHA_WATER, L"", L"Textures/waterNormal.dds", L"Textures/FlatHighSpecular.dds", L"", false, 3.0f);;
	water->AddComponent<UVScrollComponent>()->Init(XMFLOAT2(0.04f, 0.02f));

	// get water properties
	ModelComponent* waterPlane   = water->GetComponent<ModelComponent>();
	WaterSettings* waterSettings = &waterPlane->meshes[0]->waterSettings;
	
	// set maps
	waterPlane->SetDUDVMap(L"Textures/waterDUDV.dds");
	waterPlane->SetFoamMap(L"Textures/foam3.dds");
	waterPlane->SetNoiseMap(L"Textures/perlinNoise2.dds");
	waterPlane->SetNormalMap2(L"Textures/waterNormal2.dds");

	// set water settings
	waterSettings->normalScrollStrength1 = 0.1f;
	waterSettings->normalScrollStrength2 = 0.2f;
	waterSettings->colorTint             = XMFLOAT4(0.3f, 0.8f, 0.95f, 1);
	waterSettings->applyFoam             = false;
	waterSettings->foamToDepth           = 1.0f;
	waterSettings->reflectivePower       = 1.0f;

	Entity* fountain = new Entity();
	fountain->AddComponent<TransformComponent>()->Init(XMFLOAT3(-30.0f, 0.0f, 50.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(4, 3, 4));
	fountain->AddComponent<ModelComponent>()->InitModel("Models/poollow.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION | REFRACT, L"Textures/poolDiffuse.dds", L"Textures/poolNormal.dds", L"Textures/poolSpecular.dds", L"Textures/poolEmissive.dds", false, 1.0f, 0.01);

	// create fountain model
	Entity* fountainStatue = new Entity();
	fountainStatue->AddComponent<TransformComponent>()->Init(XMFLOAT3(-30.0f, 3.5f, 50.0f), XMFLOAT3(0, 160, 0), XMFLOAT3(20, 20, 20));
	fountainStatue->AddComponent<ModelComponent>()->InitModel("Models/statue.obj", STANDARD | CAST_SHADOW_DIR | CAST_REFLECTION, L"Textures/statueDiffuse.dds", L"Textures/statueNormal.dds", L"", L"", false);

	// water pillar model
	InstancedModel* waterPillars = new InstancedModel("Models/cube.obj", INSTANCED_OPAQUE | INSTANCED_CAST_SHADOW_DIR | INSTANCED_CAST_REFLECTION);
	waterPillars->AddInstance(XMFLOAT3(-42, 2.8f, 57.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0, 6.0f, 1.0f));
	waterPillars->AddInstance(XMFLOAT3(-18, 2.8f, 57.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0, 6.0f, 1.0f));
	waterPillars->AddInstance(XMFLOAT3(-42, 2.8f, 43.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0, 6.0f, 1.0f));
	waterPillars->AddInstance(XMFLOAT3(-18, 2.8f, 43.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0, 6.0f, 1.0f));
	waterPillars->BuildInstanceBuffer();

	// water light bulb model
	InstancedModel* waterBulbs = new InstancedModel("Models/sphere.obj", INSTANCED_OPAQUE | INSTANCED_CAST_SHADOW_DIR | INSTANCED_CAST_REFLECTION, L"", L"", L"", L"Textures/emissiveOrange.dds");
	waterBulbs->AddInstance(XMFLOAT3(-42, 6.5f, 57.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.0, 2.0f, 2.0f));
	waterBulbs->AddInstance(XMFLOAT3(-18, 6.5f, 57.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.0, 2.0f, 2.0f));
	waterBulbs->AddInstance(XMFLOAT3(-42, 6.5f, 43.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.0, 2.0f, 2.0f));
	waterBulbs->AddInstance(XMFLOAT3(-18, 6.5f, 43.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(2.0, 2.0f, 2.0f));
	waterBulbs->BuildInstanceBuffer();

	// water pillar lights
	Entity* waterLight = new Entity();
	waterLight->AddComponent<TransformComponent>()->Init(XMFLOAT3(-42, 4.5f, 57.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 2.0f, 2.0f));
	waterLight->AddComponent<LightPointComponent>()->Init(20, 10, XMFLOAT3(1.0f, 0.4f, 0.15f), 0.0f, 1.0f, 0.2f);

	Entity* waterLight2 = new Entity();
	waterLight2->AddComponent<TransformComponent>()->Init(XMFLOAT3(-18, 4.5f, 57.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 2.0f, 2.0f));
	waterLight2->AddComponent<LightPointComponent>()->Init(20, 10, XMFLOAT3(1.0f, 0.4f, 0.15f), 0.0f, 1.0f, 0.2f);

	Entity* waterLight3 = new Entity();
	waterLight3->AddComponent<TransformComponent>()->Init(XMFLOAT3(-42, 4.5f, 43.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 2.0f, 2.0f));	
	waterLight3->AddComponent<LightPointComponent>()->Init(20, 10, XMFLOAT3(1.0f, 0.4f, 0.15f), 0.0f, 1.0f, 0.2f);

	Entity* waterLight4 = new Entity();
	waterLight4->AddComponent<TransformComponent>()->Init(XMFLOAT3(-18, 4.5f, 43.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 2.0f, 2.0f));
	waterLight4->AddComponent<LightPointComponent>()->Init(20, 10, XMFLOAT3(1.0f, 0.4f, 0.15f), 0.0f, 1.0f, 0.2f);

	// dust particle
	Entity* dust = new Entity();
	dust->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 10.0f, 0.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
	dust->AddComponent<ParticleSystemComponent>()->Init("Particles/dust.json");

	// fire place models
	InstancedModel* firePlaces = new InstancedModel("Models/fogata1.obj", INSTANCED_OPAQUE | INSTANCED_CAST_SHADOW_DIR | INSTANCED_CAST_REFLECTION);
	firePlaces->AddInstance(XMFLOAT3(30.0f, 0.0f, -3.0f),  XMFLOAT3(0, 0, 0), XMFLOAT3(0.13, 0.13, 0.13));
	firePlaces->AddInstance(XMFLOAT3(30.0f, 0.0f, 6.0f),   XMFLOAT3(0, 0, 0), XMFLOAT3(0.13, 0.13, 0.13));
	firePlaces->AddInstance(XMFLOAT3(-32.5f, 0.0f, -3.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.13, 0.13, 0.13));
	firePlaces->AddInstance(XMFLOAT3(-32.5f, 0.0f, 6.0f),  XMFLOAT3(0, 0, 0), XMFLOAT3(0.13, 0.13, 0.13));
	firePlaces->BuildInstanceBuffer();

	// ground fires and lights
	Entity* fireFloor1 = new Entity();
	fireFloor1->AddComponent<TransformComponent>()->Init(XMFLOAT3(30.0f, 0.0f, -3.0f));
	fireFloor1->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fireFloor1->AddComponent<LightPointComponent>()->Init(4.5f, 4, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.1f);
	
	Entity* fireFloor2 = new Entity();
	fireFloor2->AddComponent<TransformComponent>()->Init(XMFLOAT3(30.0f, 0.0f, 6.0f));
	fireFloor2->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fireFloor2->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);
	
	Entity* fireFloor3 = new Entity();
	fireFloor3->AddComponent<TransformComponent>()->Init(XMFLOAT3(-32.5f, 0.0f, -3.0f));
	fireFloor3->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fireFloor3->AddComponent<LightPointComponent>()->Init(4.5f, 4, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.1f);

	Entity* fireFloor4 = new Entity();
	fireFloor4->AddComponent<TransformComponent>()->Init(XMFLOAT3(-32.5f, 0.0f, 6.0f));
	fireFloor4->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fireFloor4->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);
	
	// chandeler fires and lights
	Entity* fireHanging1 = new Entity();
	fireHanging1->AddComponent<TransformComponent>()->Init(XMFLOAT3(12.25f, 2.75f, -3.5f));
	fireHanging1->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fireHanging1->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	Entity* fireHanging2 = new Entity();
	fireHanging2->AddComponent<TransformComponent>()->Init(XMFLOAT3(12.25f, 2.75f, 5.5f));
	fireHanging2->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fireHanging2->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	Entity* fireHanging3 = new Entity();
	fireHanging3->AddComponent<TransformComponent>()->Init(XMFLOAT3(-15.5f, 2.75f, -3.5f));
	fireHanging3->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fireHanging3->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	Entity* fireHanging4 = new Entity();
	fireHanging4->AddComponent<TransformComponent>()->Init(XMFLOAT3(-15.5f, 2.75f, 5.5f));
	fireHanging4->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fireHanging4->AddComponent<LightPointComponent>()->Init(4.5f, 5, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);

	// create parallax occlussion mapping tests
 	Entity* POM = new Entity();
	POM->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0.1f, 40), XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1, 3));
	POM->AddComponent<ModelComponent>()->InitModel("Models/plane.obj", STANDARD, L"Textures/Stone1.dds", L"Textures/stone1Normal_H.dds", L"", L"", false, 3, 0.15f);

	Entity* noPOM = new Entity();
	noPOM->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0.1f, 60), XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1, 3));
	noPOM->AddComponent<ModelComponent>()->InitModel("Models/plane.obj", STANDARD, L"Textures/Stone1.dds", L"Textures/stone1Normal.dds", L"", L"", false, 3);

	// grid aligned dynamic instanced spheres, generate two different sine waves for animations
	_sineTimers[0] = XM_PI / 2;
	_sineTimers[1] = XM_PI + (XM_PI / 2);

	_sineWaves[0] = sin(_sineTimers[0]);
	_sineWaves[1] = sin(_sineTimers[1]);

	_instancedSpheres = new InstancedModel("Models/sphere.obj", INSTANCED_OPAQUE | INSTANCED_CAST_SHADOW_DIR | INSTANCED_CAST_REFLECTION, L"Textures/stone2.dds", L"Textures/stone2Normal_H.dds", L"Textures/stone2Specular.dds", L"", false, 3.0f, 0.08f);
	float spacing = 2.0f;
	int count = 0;
	for (int i = 0; i < 19; i++)
		for (int y = 0; y < 19; y++)		
			_instancedSpheres->AddInstance(XMFLOAT3(40 + (i * spacing), 5.0f + (2.0f * _sineWaves[count ++ % 2]), 40 + (y * spacing)), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
		
	_instancedSpheres->BuildInstanceBuffer();	
}

SponzaTestScene::~SponzaTestScene()
{
}

void SponzaTestScene::Update()
{
	const float& delta = Systems::time->GetDeltaTime();
	_skyDome->Update(delta);	

	_sineTimers[0] += delta;
	_sineTimers[1] += delta;

	_sineWaves[0] = sin(_sineTimers[0]);
	_sineWaves[1] = sin(_sineTimers[1]);

	float spacing = 2.0f;
	int count = 0;
	for (int i = 0; i < 19; i++)
		for (int y = 0; y < 19; y++) 		
			_instancedSpheres->modelInstances[count].worldMatrix = MATH_HELPERS::CreateWorldMatrix(XMFLOAT3(40 + (i * spacing), 5.0f + (2.0f * _sineWaves[count % 2]), 40 + (y * spacing)), XMFLOAT3(0, 0, -90 + (90 * _sineWaves[count % 2])), XMFLOAT3(1.5f + (1.0f * _sineWaves[count % 2]), 1.5f + (1.0f * _sineWaves[count % 2]), 1.5f + (1.0f * _sineWaves[count++ % 2])));			
		
	_instancedSpheres->BuildInstanceBuffer();

}
