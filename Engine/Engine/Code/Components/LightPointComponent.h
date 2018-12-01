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

	void Init(float radius, float intensity, XMFLOAT3 color, float attConstant = 0.0f, float attLinear = 1.0f, float attExponential = 0.0f);

	void Update(const float& delta);

	// get colors
	const XMFLOAT3& GetLightColor()    { return _color; }
	
	// get light properties
	const float&  GetIntensity()      { return _intensity; }
	
	const float&  GetRadius()         { return _radius; }
	const float&  GetAttConstant()    { return _attConstant; }
	const float&  GetAttLinear()      { return _attLinear; }
	const float&  GetAttExponential() { return _attExponential; }

private:

	// color
	XMFLOAT3 _color;
	
	// properties
	float _intensity;
	float _radius;
	
	float _attConstant;
	float _attLinear;
	float _attExponential;	
};

