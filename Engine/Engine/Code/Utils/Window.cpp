#include "Window.h"
#include "FrameWork.h"

Window::Window(const char* title, int x, int y, int width, int height, WNDPROC wndProc)
{
	CreateApplicationWindow(title, x, y, width, height, wndProc);
}

Window::~Window()
{
	UnregisterClass((LPCSTR)_windowName, _HINSTANCE);
}

void Window::CreateApplicationWindow(const char* title, int x, int y, int width, int height, WNDPROC wndProc)
{
	_windowName = title;
	HWND hwnd;
	WNDCLASSEX wc;

	_HINSTANCE = GetModuleHandle(NULL);

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// setup window settings
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = wndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = _HINSTANCE;
	wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = title;
	wc.cbSize        = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "failed to register windowClass", "ERROR", MB_OK);
		return;
	}

	// set style flags	
	int nStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX;

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, title, title, nStyle,
		                  x, y, width, height, NULL, NULL, _HINSTANCE, NULL);

	// show message box that window failed
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

	_HWND = hwnd;
}
