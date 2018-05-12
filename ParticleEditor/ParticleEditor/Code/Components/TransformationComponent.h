#pragma once
#include "IComponent.h"
#include <DirectXMath.h>
#include "TransformComponent.h"

using namespace DirectX;

class Entity;

enum MoveState
{
	IDLE,
	CIRCLE,
	BACKFORTH,
};

class TransformationComponent : public IComponent
{
public:
	
	TransformationComponent();
	~TransformationComponent();

	void Init(XMFLOAT3 rotation, float speedRotation, float speedTranslation = 1.0f, TransformComponent::Axis axis = TransformComponent::Axis::NONE);
	void Update();

	XMFLOAT3* GetRotationPtr() { return &_rotation; }

	MoveState _state;

private:

	TransformComponent* _transform;

	XMFLOAT3 _rotation;
	float _speedRotation;
	float _speedTranslation;
	TransformComponent::Axis _axis;

	XMFLOAT3 _circleRotation;
	XMFLOAT3 _startPosCircle;
	float _circleSpeed;

	float _backForthLength;
	float _moveSpeed;
	float _sinTimer;
	float _sinValue;

	MoveState _lastState;
};

