#pragma once
#include <Windows.h>
#include <string>

class Window;
class ParticleEditor;

class Framework
{
public:
	Framework();
	~Framework();

private:

	// functions that make up our application loop
	void Start();
	void Run();
	void Update();
	void Render();

	// helper for generating a random float
	float GetRandomFloat(float min, float max);

	// window and debug stats
	Window*         _window;
	ParticleEditor* _particleEditor;
};

