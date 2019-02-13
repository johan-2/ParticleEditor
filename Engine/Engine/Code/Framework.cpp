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
#include "MasterEditor.h"
#include <algorithm>
#include <random>
#include <chrono>
#include "Color32.h"
#include "Window.h"
#include "Systems.h"
#include "ParticleEditor.h"
#include "Renderer.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM Lparam);

Framework::Framework()
{
	// create the window for the application
	_window = new Window("Particle Editor", 0, 0, SystemSettings::SCREEN_WIDTH, SystemSettings::SCREEN_HEIGHT, WndProc);
	
	// init all systems
	Systems::InitSystems(_window, SystemSettings::SCREEN_WIDTH, SystemSettings::SCREEN_HEIGHT, SystemSettings::FULLSCREEN, SystemSettings::V_SYNC);

	// start and run
	Start();
	Run();	
}

Framework::~Framework()
{
	delete _window;
	Systems::DeleteSystems();
}

void Framework::Start()
{
	// get systems
	CameraManager& CM = *Systems::cameraManager;
	LightManager& LM = *Systems::lightManager;
	Renderer& renderer = *Systems::renderer;

	// create shadowMap
	Entity* shadowMapRenderer = renderer.CreateShadowMap(50.0f, 8192.0f, XMFLOAT3(-6, 100, 9), XMFLOAT3(85.0f, -90.0f, 0), false);

	// create skybox
	_skyDome = renderer.skyDome = new SkyDome("Settings/SkyDomeParticleEditor.json");

	// create game camera
	Entity* cameraGame = new Entity();
	cameraGame->AddComponent<TransformComponent>()->Init(XMFLOAT3(0.0f, 3.22f, -8.20f), XMFLOAT3(4.0f, 0.0f, 0.0f));
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

	// create the particle editor and pass in some dependencies
	_particleEditor = new ParticleEditor(*Systems::input, cameraGame->GetComponent<FreeMoveComponent>(), *Systems::renderer, *Systems::time);
	_masterEditor   = new MasterEditor();
	_masterEditor->showStatsWindow = false;
}

void Framework::Update()
{
	Systems::world->Update();
	_skyDome->Update(Systems::time->GetDeltaTime());
	_particleEditor->Update();
	_masterEditor->Update();
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
			DestroyWindow(_window->hwnd);
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