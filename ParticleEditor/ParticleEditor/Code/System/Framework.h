#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include "ParticleEmitterComponent.h"

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

	void ShowToolTip(const char* tip);
	void ReloadEmitter();
	void SaveParticle(char* destination);
	void UpdateParticleSettingsWindow();
	void UpdateEditorSettingsWindow();
	void UpdateInfoWindow();
	char* GetNumEmittersAsString();

	HINSTANCE _hInstance;
	HWND _window;
	std::string _applicationName;

	Entity* _cameraGame;
	Entity* _particleEntity;

	Entity* _grid;

	std::vector<ParticleSettings> _particleSettings;
	std::vector<int> _blendEnum;
	unsigned int _numEmitters;
	unsigned int _currentEmitterIndex;

	ParticleEmitterComponent* _particleComponent;
	TransformationComponent* _transformation;
	TransformComponent* _transform;

	float _clearColor[4];
};

