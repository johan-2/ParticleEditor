#pragma once
#include "IComponent.h"
#include <DirectXMath.h>
#include "TransformComponent.h"

using namespace DirectX;

class Entity;

class RotationComponent : public IComponent
{
public:
	
	RotationComponent();
	~RotationComponent();

	void Init(XMFLOAT3 rotation, float speedRotation);
	void Update();

private:

	TransformComponent* _transform;

	XMFLOAT3 _rotation;
	float _speedRotation;
	float _speedTranslation;	
};

