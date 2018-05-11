#pragma once
#include "IComponent.h"
#include <DirectXMath.h>
#include "TransformComponent.h"

using namespace DirectX;

class Entity;

class TransformationComponent : public IComponent
{
public:
	
	TransformationComponent();
	~TransformationComponent();

	void Init(XMFLOAT3 rotation, float speedRotation, float speedTranslation = 1.0f, TransformComponent::Axis axis = TransformComponent::Axis::NONE);
	void Update();

private:

	TransformComponent* _transform;

	XMFLOAT3 _rotation;
	float _speedRotation;
	float _speedTranslation;
	TransformComponent::Axis _axis;
};

