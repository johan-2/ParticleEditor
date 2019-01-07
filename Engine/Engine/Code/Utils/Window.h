#pragma once
#include <Windows.h>
class Window
{
public:
	Window(const char* title, unsigned int x, unsigned int y, unsigned int width, unsigned int height, WNDPROC wndProc);
	~Window();

	// get window handles
	HINSTANCE GetHINSTANCE() { return _HINSTANCE; }
	HWND GetHWND()           { return _HWND; }

	// get the name of the window
	const char* GetWindowName()    { return _windowName; }

private:

	// creates the window
	void CreateApplicationWindow(const char* title, int x, int y, int width, int height, WNDPROC wndProc);

	// window handles
	HINSTANCE   _HINSTANCE;
	HWND        _HWND;
	const char* _windowName;
};

