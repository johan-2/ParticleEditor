#pragma once
#include <vector>
#include "ParticleSystemComponent.h"
#include "Input.h"

class Entity;
class TransformComponent;
class FreeMoveComponent;

class ParticleEditor
{
public:
	ParticleEditor(Input& input, FreeMoveComponent* moveComponent);
	~ParticleEditor();

	void Update();

private:

	// window functions
	void UpdateParticleSettingsWindow();
	void UpdateInfoWindow();
	void UpdateKeyCommands();
	void UpdateEditorSettingsWindow();
	void SaveParticle(char* destination);

	std::string FindFileFromDirectory(char* filter, char* title);

	// helper functions
	void ShowToolTip(const char* tip);
	void ReloadSystem();
	char* GetNumEmittersAsString();

	// entities needed for editor
	Entity* _cameraGame;
	Entity* _particleEntity;
	Entity* _grid;

	// cached components
	ParticleSystemComponent* _systemParticleComponent;
	TransformComponent*      _systemTransformComponent;
	FreeMoveComponent*       _cameraFreeMoveComponent;

	// stores the settings of each emitter in the current system
	std::vector<ParticleSettings> _particleSettings;

	// stores the blend states of each emitter in the current system
	std::vector<int> _blendEnum;

	// keep tracks of how many emitters this system have
	// and which emitter is currently bieng edited 
	unsigned int _numEmitters;
	unsigned int _currentEmitterIndex;

	// Camera Toggle
	bool _cameraMoveToggle;
	bool _systemUpdateToggle;

	// state for the movement of particlesystem entity
	int _moveState;

	// instance to input system
	Input& _input;

	float _clearColor[4];

};

