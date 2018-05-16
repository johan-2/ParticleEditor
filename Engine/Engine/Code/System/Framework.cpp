#include "Framework.h"
#include <d3d11.h>
#include <iostream>
#include <DirectXMath.h>

#include "Framework.h"
#include "SystemDefs.h"
#include "DXManager.h"
#include "CameraManager.h"
#include "ShaderManager.h"
#include "LightManager.h"
#include "Renderer.h"
#include "World.h"
#include "input.h"
#include "Time.h"
#include "Mesh.h"
#include "Entity.h"
#include "FreeMoveComponent.h"
#include "TransformationComponent.h"
#include "UVScrollComponent.h"
#include "CameraComponent.h"
#include "QuadComponent.h"
#include "ModelComponent.h"
#include "ParticleEmitterComponent.h"
#include "LightPointComponent.h"
#include <iostream>
#include "GuiManager.h"
#include "DebugStats.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM Lparam);

Framework::Framework()
{

	// create window
	CreateWindowDx11("Engine", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	// init all directX stuff
	DXManager::GetInstance().Initialize(_window, SCREEN_WIDTH, SCREEN_HEIGHT, V_SYNC, FULLSCREEN);

	GuiManager::GetInstance().Initialize(_window);
	// load and create our shader objects	
	ShaderManager::GetInstance().Initialize();

	// initialize directinput
	Input::GetInstance().InitializeInputDevices(_hInstance, _window);

	// start timers
	Time::GetInstance();

	// start and run
	Start();
	Run();
	
}


Framework::~Framework()
{
	DXManager::GetInstance().Shutdown();
	ShaderManager::GetInstance().Shutdown();
	
	UnregisterClass((LPCSTR)_applicationName.c_str(), _hInstance);	

#ifdef _DEBUG
	delete _debugStats;
#endif 

}


void Framework::Start()
{

	// setup renderer to handle depthrendering
	Renderer::GetInstance().CreateDepthMap();

	// create game camera
	Entity* cameraGame = new Entity();
	cameraGame->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, -20), XMFLOAT3(0,0,0));
	cameraGame->AddComponent<CameraComponent>()->Init3D(70); 
	cameraGame->AddComponent<FreeMoveComponent>()->init();
	CameraManager::GetInstance().SetCurrentCameraGame(cameraGame->GetComponent<CameraComponent>());

	// create UIcamera
	Entity* cameraUI = new Entity();
	cameraUI->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, -1));
	cameraUI->AddComponent<CameraComponent>()->Init2D(XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), XMFLOAT2(0.01f, 1.0f));
	CameraManager::GetInstance().SetCurrentCameraUI(cameraUI->GetComponent<CameraComponent>());

	//set ambient light color	
	LightManager::GetInstance().SetAmbientColor(XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));

	// get transform of the camera that renders the depthmap
	TransformComponent* dt = CameraManager::GetInstance().GetCurrentCameraDepthMap()->GetComponent<TransformComponent>();

	// create directional light and give it the same position/ rotation as depthrender camera
	Entity* directionalLight = new Entity;
	directionalLight->AddComponent<TransformComponent>()->Init(dt->GetPositionVal(), dt->GetRotationVal());
	directionalLight->AddComponent<LightDirectionComponent>()->Init(XMFLOAT4(0.02f, 0.02f, 0.02f, 1), XMFLOAT4(1, 1, 1, 1), 100.0f);

	//// test entities
	Entity* box = new Entity();
	box->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, -2, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(2,2,2));
	box->AddComponent<TransformationComponent>()->Init(XMFLOAT3(1.0f, 1.0f, 0.0f), 40.0f);
	box->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::CUBE, DEFERRED | CAST_SHADOW_DIR, L"Textures/metalBox.dds", L"Textures/metalBoxNormal.dds", L"Textures/metalBoxSpecular.dds");

	Entity* floor = new Entity();
	floor->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, -8, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(10, 1, 10));
	floor->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::PLANE, DEFERRED | CAST_SHADOW_DIR, L"Textures/red.dds", L"Textures/bricksNormal.dds", L"Textures/bricksSpecular");

	Entity* wall = new Entity();
	wall->AddComponent<TransformComponent>()->Init(XMFLOAT3(-20, -6.5f, 12), XMFLOAT3(0, -80, 0), XMFLOAT3(10, 10, 3));
	wall->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::CUBE, DEFERRED | CAST_SHADOW_DIR, L"Textures/crateDamp.dds", L"Textures/crateDampNormal.dds", L"Textures/crateDampSpecular.dds");

	Entity* fire = new Entity();
	fire->AddComponent<TransformComponent>()->Init(XMFLOAT3(-15.0f, -7.5f, 12), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
	fire->AddComponent<ParticleEmitterComponent>()->Init("Particles/fire.json");

	Entity* pointLight1 = new Entity();
	pointLight1->AddComponent<TransformComponent>()->Init(XMFLOAT3(0.0f, -3.0f, -5), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
	pointLight1->AddComponent<LightPointComponent>()->Init(10.0f, 3.0f, XMFLOAT3(1, 1.0f, 0.8f), XMFLOAT3(1, 1, 1), 80.0f, 0.0f, 1.0f, 0.01f);

	Entity* pointLight2 = new Entity();
	pointLight2->AddComponent<TransformComponent>()->Init(XMFLOAT3(-15.0f, -5.0f, 12), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1));
	pointLight2->AddComponent<LightPointComponent>()->Init(15.0f, 3.0f, XMFLOAT3(1.0, 0.8, 0.0), XMFLOAT3(1, 0.8, 0.0), 80.0f, 0.0f, 0.6f, 0.1f);

#ifdef _DEBUG
	_debugStats = new DebugStats();
#endif
}

void Framework::Update()
{
	World::GetInstance().Update();

#ifdef _DEBUG
	_debugStats->Update();
#endif
}

void Framework::Render()
{
	Renderer::GetInstance().Render();
	ShaderManager::GetInstance().RenderGUI(ImGui::GetDrawData());
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
			DestroyWindow(_window);
		}

		// update everything
		Update();
		
		DXManager& DXM = DXManager::GetInstance();

		// clear rendertarget from last frame
		DXM.ClearRenderTarget(0.5, 0.5, 0.5, 1);

		// setup ImGui buffers before rendering
		ImGui::Render();

		// render everything
		Render();

		// swap buffers
		DXM.PresentScene();
	}
}

void Framework::CreateWindowDx11(char* title, int x, int y, int width, int height)
{
	_applicationName = title;
	HWND hwnd;
	WNDCLASSEX wc;
	
	_hInstance = GetModuleHandle(NULL);

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// set up window class with default settings
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc; // tell witch function that will be used when receving evnts from windows
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "failed to register windowClass", "ERROR", MB_OK);
		return;
	}
	// create the window 	
	int nStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX;
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		title,
		title,
		nStyle,
		x,
		y,
		width,
		height,
		NULL,
		NULL,
		_hInstance,
		NULL);

	// if window is null something went wrong and we return false
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Create window failed", "ERROR", MB_OK);
		PostQuitMessage(0);
		return;
	}

	// set focus on window
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	_window = hwnd;
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
	default:
		return DefWindowProc(hwnd, msg, wParam, Lparam);
	}

	return 0;
}