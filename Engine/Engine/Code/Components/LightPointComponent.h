#pragma once
#include "IComponent.h"
#include <DirectXMath.h>
#include "TransformComponent.h"

class Entity;

class LightPointComponent : public IComponent
{
public:
	
	LightPointComponent();
	~LightPointComponent();

	void Init(float radius, float intensity, XMFLOAT3 color, XMFLOAT3 specularColor = XMFLOAT3(1, 1, 1), float specularPower = 80.0f, float attConstant = 0.0f, float attLinear = 1.0f, float attExponential = 0.0f);

	void Update();

	const XMFLOAT3& GetLightColor() { return _color; }
	const XMFLOAT3& GetSpecularColor() { return _specularColor; }
	const float& GetIntensity() { return _intensity; }
	const float& GetSpecularPower() { return _specularPower; }
	const float& GetRadius() { return _radius; }
	const float& GetAttConstant() { return _attConstant; }
	const float& GetAttLinear() { return _attLinear; }
	const float& GetAttExponential() { return _attExponential; }

private:

	float _intensity;
	float _radius;
	XMFLOAT3 _color;
	XMFLOAT3 _specularColor;
	float _specularPower;
	float _attConstant;
	float _attLinear;
	float _attExponential;
	
};

