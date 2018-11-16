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
	directionalLight->AddComponent<TransformComponent>()->Init(dt->GetPositionVal(), dt->GetRotationVal());
	directionalLight->AddComponent<LightDirectionComponent>()->Init(XMFLOAT4(0.8f, 0.8f, 0.8f, 1), XMFLOAT4(1, 1, 1, 1), 80.0f);
	
	Entity* box = new Entity();
	box->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 4, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 2, 2));
	box->AddComponent<RotationComponent>()->Init(XMFLOAT3(GetRandomFloat(-10,10), GetRandomFloat(-10, 10), GetRandomFloat(-10, 10)), GetRandomFloat(0.1f, 5.0f));
	box->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::CUBE, DEFERRED | CAST_SHADOW_DIR, L"Textures/rock.dds", L"Textures/rockNormal.dds", L"Textures/rockSpecular.dds");
	
	Entity* floor = new Entity();
	floor->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(12, 1, 12));
	floor->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::PLANE, DEFERRED | CAST_SHADOW_DIR, L"Textures/bricks.dds", L"Textures/bricksNormal.dds", L"Textures/bricksSpecular.dds");

	Entity* alphaTest = new Entity();
	alphaTest->AddComponent<TransformComponent>()->Init(XMFLOAT3(-18, 5, -20), XMFLOAT3(90, 0, 0), XMFLOAT3(12, 0.01f, 12));
	alphaTest->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::CUBE, ALPHA_FORWARD | CAST_SHADOW_DIR, L"Textures/AlphaTest.dds", L"Textures/pavingNormal.dds", L"Textures/pavingSpecular.dds");

	Entity* alphaTest2 = new Entity();
	alphaTest2->AddComponent<TransformComponent>()->Init(XMFLOAT3(-15, 5, -10), XMFLOAT3(90, 0, 0), XMFLOAT3(12, 0.01f, 12));
	alphaTest2->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::CUBE, ALPHA_FORWARD | CAST_SHADOW_DIR, L"Textures/AlphaTest.dds", L"Textures/pavingNormal.dds", L"Textures/pavingSpecular.dds");

	Entity* wall = new Entity();
	wall->AddComponent<TransformComponent>()->Init(XMFLOAT3(-20, 2, 12), XMFLOAT3(0, -70, 0), XMFLOAT3(10, 10, 3));
	wall->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::CUBE, DEFERRED | CAST_SHADOW_DIR, L"Textures/crateDamp.dds", L"Textures/crateDampNormal.dds", L"Textures/crateDampSpecular.dds");

	Entity* fire = new Entity();
	fire->AddComponent<TransformComponent>()->Init(XMFLOAT3(-15.0f, 0.5f, 12), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
	fire->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");
	fire->AddComponent<LightPointComponent>()->Init(6, 8, XMFLOAT3(0.8f, 0.4f, 0.0f), XMFLOAT3(0.8f, 0.4f, 0.0f), 80.0f, 0.0f, 1.0f, 0.1f);

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
		
		// clear rendertarget from last frame
		DXM.ClearRenderTarget(0, 0, 0, 1);

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
		//Systems::time->OnWindowChange();
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