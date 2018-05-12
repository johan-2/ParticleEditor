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
	
	// cant return const& here becuase GetForward of transform is a vec3 allocated on the stack
	XMFLOAT3 GetLightDirection() { return _transform->GetForward(); }

	const XMFLOAT4& GetLightColor() { return _lightColor; }
	void SetLightColor(XMFLOAT4 color) { _lightColor = color; }

	const XMFLOAT4& GetSpecularColor() { return _specularColor; }
	void SetSpeculartColor(XMFLOAT4 color) { _specularColor = color; }

	const float& GetSpecularPower() { return _specularPower; }
	void SetSpecularPower(float power) { _specularPower = power; }

	void Update();

private:

	TransformComponent* _transform;
	XMFLOAT3 _lightDir;
	XMFLOAT4 _lightColor;
	XMFLOAT4 _specularColor;
	float _specularPower;


};

