#pragma once

#include <Windows.h>
#include <string>

class Framework
{
public:
	Framework();
	~Framework();

private:

	void Start();
	void Run();
	void Update();
	void Render();

	void CreateWindowDx11(char* title, int x, int y, int width, int height);

	HINSTANCE _hInstance;
	HWND _window;
	std::string _applicationName;
};

