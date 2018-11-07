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

	void init(float movementSpeed, float rotationSpeed);
	void Update();
	
private:
	
	// updates the transform of the entity this component is atached to
	void UpdateMovement();
	
	// cached pointer to our transform component
	TransformComponent* _transform;	

	float _moveSpeed;
	float _rotationSpeed;
};

