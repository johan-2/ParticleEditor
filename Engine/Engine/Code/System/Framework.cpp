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

	CreateWindowDx11("Engine", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	DXManager::GetInstance().Initialize(_window, SCREEN_WIDTH, SCREEN_HEIGHT, V_SYNC, FULLSCREEN);

	GuiManager::GetInstance().Initialize(_window);
	// load and create our shader objects	
	ShaderManager::GetInstance().Initialize();

	// initialize directinput
	Input::GetInstance().InitializeInputDevices(_hInstance, _window);

	// start timers
	Time::GetInstance();


	Start();
	Run();
	
}


Framework::~Framework()
{
	DXManager::GetInstance().Shutdown();
	ShaderManager::GetInstance().Shutdown();
	
	UnregisterClass((LPCSTR)_applicationName.c_str(), _hInstance);	

	delete _debugStats;
}


void Framework::Start()
{

	// setup renderer to handle depthrendering
	Renderer::GetInstance().CreateDepthMap();

	// create game camera
	Entity* cameraGame = new Entity();
	cameraGame->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, -20), XMFLOAT3(0,0,0));
	CameraComponent* camGame = cameraGame->AddComponent<CameraComponent>(); camGame->Init3D(70);
	cameraGame->AddComponent<FreeMoveComponent>()->init();
	CameraManager::GetInstance().SetCurrentCameraGame(camGame);

	// create UIcamera
	Entity* cameraUI = new Entity();
	cameraUI->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, -1));
	CameraComponent* camUI = cameraUI->AddComponent<CameraComponent>(); camUI->Init2D(XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), XMFLOAT2(0.01f, 1.0f));
	CameraManager::GetInstance().SetCurrentCameraUI(camUI);

	//set ambient light color	
	LightManager::GetInstance().SetAmbientColor(XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));

	// create directional light, set to same values as the depthrendercamera
	TransformComponent* dt = CameraManager::GetInstance().GetCurrentCameraDepthMap()->GetComponent<TransformComponent>();
	Entity* directionalLight = new Entity;
	directionalLight->AddComponent<TransformComponent>()->Init(dt->GetPositionVal(), dt->GetRotationVal());
	directionalLight->AddComponent<LightDirectionComponent>()->Init(XMFLOAT4(1.0f, 1.0f, 1.0f, 1), XMFLOAT4(1, 1, 1, 1), 100.0f);

	//// test entities
	Entity* testEntity1 = new Entity();
	testEntity1->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0));
	testEntity1->AddComponent<TransformationComponent>()->Init(XMFLOAT3(1.0f, 1.0f, 0.0f), 40.0f);
	testEntity1->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::CUBE, AMBIENT | DIRECTIONAL , L"Textures/metalBox.dds", L"Textures/metalBoxNormal.dds", L"Textures/metalBoxSpecular.dds");

	_debugStats = new DebugStats();
}

void Framework::Update()
{
	World::GetInstance().Update();

	_debugStats->Update();

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
		DXM.ClearRenderTarget(0, 0, 0, 1);

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