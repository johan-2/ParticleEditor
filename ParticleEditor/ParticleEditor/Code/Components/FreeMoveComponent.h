#pragma once

#include "CameraManager.h"
#include "IComponent.h"

class Entity;
class Gamepad;

using namespace DirectX;

class FreeMoveComponent : public IComponent
{
public:
	FreeMoveComponent();
	~FreeMoveComponent();

	void init();
	void Update();
	
private:
	
	void UpdateMovement();

	Gamepad* _gamepad;
	TransformComponent* _transform;
	bool _active;
		
};

