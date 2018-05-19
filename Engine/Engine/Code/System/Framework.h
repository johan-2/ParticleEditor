#pragma once

#include <Windows.h>
#include <string>

class Entity;
class DebugStats;

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

	float GetRandomFloat(float min, float max);

	HINSTANCE _hInstance;
	HWND _window;
	std::string _applicationName;

	DebugStats* _debugStats;
};

