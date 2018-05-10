#include "Framework.h"
#include <d3d11.h>
#include <iostream>

#include "DXManager.h"
#include "SystemDefs.h"


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM Lparam);

Framework::Framework()
{

	CreateWindowDx11("Engine", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	DXManager::GetInstance().Initialize(_window, SCREEN_WIDTH, SCREEN_HEIGHT, V_SYNC, FULLSCREEN);

	Start();
	Run();
	
}


Framework::~Framework()
{
	DXManager::GetInstance().Shutdown();
	
	UnregisterClass((LPCSTR)_applicationName.c_str(), _hInstance);	
}


void Framework::Start()
{
	
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

		
		
		DXManager& DXM = DXManager::GetInstance();
		DXM.ClearRenderTarget(0, 0, 0, 1);
		DXM.PresentScene();
		

	}
}

void Framework::Update()
{

}

void Framework::Render()
{

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
	default:
		return DefWindowProc(hwnd, msg, wParam, Lparam);
	}

	return 0;
}