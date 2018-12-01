#pragma once
#include <DirectXMath.h>
#include "IComponent.h"
#include "TransformComponent.h"

using namespace DirectX;

class Entity;

class LightDirectionComponent : public IComponent
{
public:
	LightDirectionComponent();
	~LightDirectionComponent();

	void Init(XMFLOAT4 lightColor, TransformComponent* transformOverride = nullptr);
	
	// get the look direction of this light
	XMFLOAT3 GetLightDirection();

	// get light properties
	const XMFLOAT4& GetLightColor()    { return _lightColor; }

	// set light properties
	void SetLightColor(XMFLOAT4 color)     { _lightColor = color; }

	void Update(const float& delta);

private:

	// pointer to transform
	TransformComponent* _transform;

	// pointer to transform to overide light direction
	TransformComponent* _overrideTransform;

	// light properties
	XMFLOAT3 _lightDir;
	XMFLOAT4 _lightColor;
};

