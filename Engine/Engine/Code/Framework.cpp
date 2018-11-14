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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM Lparam);

Framework::Framework()
{
	// create the window for the application
	_window = new Window("Engine", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WndProc);
	
	// init all directX stuff
	DXManager::GetInstance().Initialize(_window->GetHWND(), SCREEN_WIDTH, SCREEN_HEIGHT, V_SYNC, FULLSCREEN);

	// initialize dear im gui
	GuiManager::GetInstance().Initialize(_window->GetHWND());

	// initialize directinput
	Input::GetInstance().InitializeInputDevices(_window->GetHINSTANCE(), _window->GetHWND());

	// start timers
	Time::GetInstance();

	// setup renderer 
	Renderer::GetInstance().Initailize();

	// start and run
	Start();
	Run();	
}

Framework::~Framework()
{
	delete _window;
}

void Framework::Start()
{
	// create game camera
	Entity* cameraGame = new Entity();
	cameraGame->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 20, -40), XMFLOAT3(30,0,0));
	cameraGame->AddComponent<CameraComponent>()->Init3D(70); 
	cameraGame->AddComponent<FreeMoveComponent>()->init(20.0f, 0.25f);
	CameraManager::GetInstance().SetCurrentCameraGame(cameraGame->GetComponent<CameraComponent>());

	// create UIcamera
	Entity* cameraUI = new Entity();
	cameraUI->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, -1));
	cameraUI->AddComponent<CameraComponent>()->Init2D(XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), XMFLOAT2(0.01f, 10.0f));
	CameraManager::GetInstance().SetCurrentCameraUI(cameraUI->GetComponent<CameraComponent>());

	//set ambient light color	
	LightManager::GetInstance().SetAmbientColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// create directional light and give it the same position/rotation as the depth render camera
	Entity* directionalLight = new Entity;
	directionalLight->AddComponent<TransformComponent>()->Init(XMFLOAT3(0,0,0), XMFLOAT3(60, 30, 0));
	directionalLight->AddComponent<LightDirectionComponent>()->Init(XMFLOAT4(0.8f, 0.8f, 0.8f, 1), XMFLOAT4(1, 1, 1, 1), 80.0f);
}

void Framework::Update()
{
	World::GetInstance().Update();
}

void Framework::Render()
{
	Renderer::GetInstance().Render();
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

		// update imgui
		GuiManager::GetInstance().Update();

		// update timers
		Time::GetInstance().Update();

		//uppdate input
		Input::GetInstance().Update();
		
		//destroy window if escape is pressed, will send quit message with windowproc to end loop
		if (Input::GetInstance().IskeyPressed(DIK_ESCAPE))
		{
			DXManager::GetInstance().SetFullscreen(false);
			DestroyWindow(_window->GetHWND());
		}

		// update everything
		Update();
		
		DXManager& DXM = DXManager::GetInstance();

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
		GuiManager::GetInstance().UpdateMouseCursor();
		break;
		// sent when size,position or Z order is changed
		// works for everything that is freezing the application
		// like draging window, fullscreen change and using any windows type functions 
		// like opening the file directory to save/open files
		// when this happens we need to save the last deltaTime
		// so we can use it for the first frame when the application starts uppdating again
		// otherwise we end up with a massive delta during the first frame
	case WM_WINDOWPOSCHANGING:
		Time::GetInstance().OnWindowChange();
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