#pragma once
#include <Windows.h>
class Window
{
public:
	Window(char* title, int x, int y, int width, int height, WNDPROC wndProc);
	~Window();

	// get window handles
	HINSTANCE GetHINSTANCE() { return _HINSTANCE; }
	HWND GetHWND()           { return _HWND; }

	// get the name of the window
	char* GetWindowName()    { return _windowName; }

private:

	// creates the window
	void CreateApplicationWindow(char* title, int x, int y, int width, int height, WNDPROC wndProc);

	// window handles
	HINSTANCE _HINSTANCE;
	HWND      _HWND;
	char*     _windowName;
};

