#pragma once
#include <vector>
#include "ParticleSystemComponent.h"

class Entity;
class TransformComponent;

class ParticleEditor
{
public:
	ParticleEditor();
	~ParticleEditor();

	void Setup();
	void Update();

private:

	void UpdateParticleSettingsWindow();

	// helper functions
	void ShowToolTip(const char* tip);
	void ReloadEmitter();
	char* GetNumEmittersAsString();

	// entities needed for editor
	Entity* _cameraGame;
	Entity* _particleEntity;
	Entity* _grid;

	// cached components
	ParticleSystemComponent* _systemParticleComponent;
	TransformComponent*      _systemTransformComponent;

	// stores the settings of each emitter in the current system
	std::vector<ParticleSettings> _particleSettings;

	// stores the blend states of each emitter in the current system
	std::vector<int> _blendEnum;

	// keep tracks of how many emitters this system have
	// and which emitter is currently bieng edited 
	unsigned int _numEmitters;
	unsigned int _currentEmitterIndex;
};

