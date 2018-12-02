#include "Framework.h"
#include <d3d11.h>
#include <iostream>
#include <DirectXMath.h>
#include "Framework.h"
#include "SystemDefs.h"
#include "DXManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Renderer.h"
#include "World.h"
#include "input.h"
#include "Time.h"
#include "Mesh.h"
#include "Entity.h"
#include "FreeMoveComponent.h"
#include "RotationComponent.h"
#include "UVScrollComponent.h"
#include "CameraComponent.h"
#include "QuadComponent.h"
#include "ModelComponent.h"
#include "ParticleSystemComponent.h"
#include "LightPointComponent.h"
#include <iostream>
#include "GuiManager.h"
#include "DebugStats.h"
#include <algorithm>
#include <random>
#include <chrono>
#include "Color32.h"
#include "Window.h"
#include "Systems.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM Lparam);

Framework::Framework()
{
	// create the window for the application
	_window = new Window("Engine", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WndProc);
	
	// init all systems
	Systems::InitSystems(_window, SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN, V_SYNC);

	// start and run
	Start();
	Run();	
}

Framework::~Framework()
{
	delete _window;
	Systems::DeleteSystems();

#ifdef _DEBUG
	delete _debugStats;
#endif 
}

void Framework::Start()
{
	// get systems
	CameraManager& CM = *Systems::cameraManager;
	LightManager& LM  = *Systems::lightManager;

	// create game camera
	Entity* cameraGame = new Entity();
	cameraGame->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 20, -40), XMFLOAT3(30,0,0));
	cameraGame->AddComponent<CameraComponent>()->Init3D(70); 
	cameraGame->AddComponent<FreeMoveComponent>()->init(20.0f, 0.25f);
	CM.SetCurrentCameraGame(cameraGame->GetComponent<CameraComponent>());

	// create UIcamera
	Entity* cameraUI = new Entity();
	cameraUI->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, -1));
	cameraUI->AddComponent<CameraComponent>()->Init2D(XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), XMFLOAT2(0.01f, 10.0f));
	CM.SetCurrentCameraUI(cameraUI->GetComponent<CameraComponent>());

	//set ambient light color	
	LM.SetAmbientColor(XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));

	// get transform of the camera that renders the depthmap
	TransformComponent* dt = CM.GetCurrentCameraDepthMap()->GetComponent<TransformComponent>();

	// create directional light and give it the same position/rotation as the depth render camera
	Entity* directionalLight = new Entity;
	directionalLight->AddComponent<LightDirectionComponent>()->Init(XMFLOAT4(0.8f, 0.8f, 0.8f, 1), dt);

	Entity* ship = new Entity();
	ship->AddComponent<TransformComponent>()->Init(XMFLOAT3(-25, 0, 10), XMFLOAT3(0, 0, 0), XMFLOAT3(2.5, 2.5, 2.5));
	ship->AddComponent<ModelComponent>()->InitModel("Models/Freigther_BI_Export.OBJ", DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION);
	
	Entity* grave = new Entity();
	grave->AddComponent<TransformComponent>()->Init(XMFLOAT3(5, 0, 0), XMFLOAT3(0, 30, 0), XMFLOAT3(0.1, 0.1, 0.1));
	grave->AddComponent<ModelComponent>()->InitModel("Models/grave_18_.fbx", DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION, L"Textures/grave_18_D.dds", L"Textures/grave_18_N.dds", L"Textures/grave_18_S.dds");

	Entity* rifle = new Entity();
	rifle->AddComponent<TransformComponent>()->Init(XMFLOAT3(-10, 8, -20), XMFLOAT3(0, 0, 0), XMFLOAT3(0.5, 0.5, 0.5));
	rifle->AddComponent<RotationComponent>()->Init(XMFLOAT3(GetRandomFloat(-10, 10), GetRandomFloat(-10, 10), GetRandomFloat(-10, 10)), GetRandomFloat(0.1f, 5.0f));
	rifle->AddComponent<ModelComponent>()->InitModel("Models/M24_R_High_Poly_Version_obj.obj", DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION);

	Entity* sphere = new Entity();
	sphere->AddComponent<TransformComponent>()->Init(XMFLOAT3(15, 4, -7), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
	sphere->AddComponent<RotationComponent>()->Init(XMFLOAT3(GetRandomFloat(-10, 10), GetRandomFloat(-10, 10), GetRandomFloat(-10, 10)), GetRandomFloat(0.1f, 5.0f));
	sphere->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::SPHERE, DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION, L"Textures/Dirt_21_Diffuse.dds", L"Textures/Dirt_21_Normal.dds", L"Textures/Dirt_21_Specular.dds");
	
	Entity* floor = new Entity();
	floor->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(20, 1, 20));
	floor->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::PLANE, ALPHA_REFLECTION, L"Textures/marble.dds", L"Textures/marbleNormal.dds", L"Textures/FlatHighSpecular.dds", 5.0f);

	Entity* fire = new Entity();
	fire->AddComponent<TransformComponent>()->Init(XMFLOAT3(12.0f, 0.0f, -6.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.25, 0.25, 0.25));
	fire->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire->AddComponent<LightPointComponent>()->Init(9, 8, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.1f);
	fire->AddComponent<ModelComponent>()->InitModel("Models/fogata1.obj", DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION);

	Entity* fire2 = new Entity();
	fire2->AddComponent<TransformComponent>()->Init(XMFLOAT3(11.0f, 0.0f, 12.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0.25, 0.25, 0.25));
	fire2->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire2->AddComponent<LightPointComponent>()->Init(9, 10, XMFLOAT3(0.8f, 0.4f, 0.0f), 0.0f, 1.0f, 0.0f);
	fire2->AddComponent<ModelComponent>()->InitModel("Models/fogata1.obj", DEFERRED | CAST_SHADOW_DIR | CAST_REFLECTION);

#ifdef _DEBUG
	_debugStats = new DebugStats();
#endif
}

void Framework::Update()
{
	Systems::world->Update();

#ifdef _DEBUG
	_debugStats->Update();
#endif
}

void Framework::Render()
{
	Systems::renderer->Render();
}

void Framework::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DXManager& DXM = *Systems::dxManager;

		// update imgui
		Systems::guiManager->Update();

		// update timers
		Systems::time->Update();

		//uppdate input
		Systems::input->Update();
		
		//destroy window if escape is pressed, will send quit message with windowproc to end loop
		if (Systems::input->IskeyPressed(DIK_ESCAPE))
		{
			DXM.SetFullscreen(false);
			DestroyWindow(_window->GetHWND());
		}

		// update everything
		Update();
		
		// render everything
		Render();

		// swap buffers
		DXM.PresentScene();
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM Lparam)
{
	// just handle closing, destroying and quit messages with wndproc, all other input is done with directinput
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		SetFocus(GetConsoleWindow());
		break;
	case WM_SETCURSOR:
		Systems::guiManager->UpdateMouseCursor();
		break;
		// sent when size,position or Z order is changed
		// works for everything that is freezing the application
		// like draging window, fullscreen change and using any windows type functions 
		// like opening the file directory to save/open files
		// when this happens we need to save the last deltaTime
		// so we can use it for the first frame when the application starts uppdating again
		// otherwise we end up with a massive delta during the first frame
	case WM_WINDOWPOSCHANGING:
		if (Systems::time != nullptr)
			Systems::time->OnWindowChange();
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, Lparam);
	}

	return 0;
}

// returns a randomized float
float Framework::GetRandomFloat(float min, float max)
{
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);
	std::uniform_real_distribution<float> distribution(min, max);

	return distribution(generator);
}