#pragma once
#include <vector>
#include "ParticleSystemComponent.h"
#include "Input.h"
#include "Time.h"
#include "Renderer.h"

class Entity;
class TransformComponent;
class FreeMoveComponent;
class ModelComponent;

class ParticleEditor
{
public:
	ParticleEditor(Input& input, FreeMoveComponent* moveComponent, Renderer& renderer, Time& time);
	~ParticleEditor();

	void Update();

private:

	// store misc editor settings we need to keep track of here
	struct EditorSettings
	{
		bool   renderSkybox           = true;
		bool   emitterAsWireFrame     = true;
		bool   gridAsWireFrame        = true;
		int    moveState              = 0;
		bool   showGrid               = true;
		bool   showParticleModel      = true;
		double sinCounter             = 0;
		float  moveSpeed              = 3;
		XMFLOAT3 systemRotationAmount = XMFLOAT3(0, 0, 0);
		XMFLOAT3 systemPosition       = XMFLOAT3(0, 0, 0);
		XMFLOAT3 systemRotation       = XMFLOAT3(0, 0, 0);
	};

	// window functions
	void UpdateParticleSettingsWindow();
	void UpdateInfoWindow();
	void UpdateKeyCommands();
	void UpdateEditorSettingsWindow();
	void SaveParticle(const char* destination);

	// opens the windows file directory and returns the path to the selected file
	std::string FindFileFromDirectory(const char* filter, const char* title);

	// update system entity movement
	void UpdateEntityMovement();

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
	ModelComponent*          _systemModelComponent;
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

	// states for certain settings
	EditorSettings _miscSettings;

	// dependencies
	Input&    _input;
	Renderer& _renderer;
	Time&     _time;

	// clear color skybox is disabled
	float _clearColor[4];
};

