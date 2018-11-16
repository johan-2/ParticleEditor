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

	void Init(XMFLOAT4 lightColor, XMFLOAT4 specularColor, float specularPower);
	
	// get the look direction of this light
	XMFLOAT3 GetLightDirection() { return _transform->GetForward(); }

	// get light properties
	const XMFLOAT4& GetLightColor()    { return _lightColor; }
	const XMFLOAT4& GetSpecularColor() { return _specularColor; }
	const float&    GetSpecularPower() { return _specularPower; }

	// set light properties
	void SetLightColor(XMFLOAT4 color)     { _lightColor = color; }
	void SetSpeculartColor(XMFLOAT4 color) { _specularColor = color; }
	void SetSpecularPower(float power)     { _specularPower = power; }

	void Update(const float& delta);

private:

	// pointer to transform
	TransformComponent* _transform;

	// light properties
	XMFLOAT3 _lightDir;
	XMFLOAT4 _lightColor;
	XMFLOAT4 _specularColor;
	float    _specularPower;
};

