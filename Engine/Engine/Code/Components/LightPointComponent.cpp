#include "LightPointComponent.h"
#include "Entity.h"
#include "LightManager.h"
#include "Systems.h"

LightPointComponent::LightPointComponent() : IComponent(POINT_LIGHT_COMPONENT)
{
	Systems::lightManager->AddPointLight(this);
}

LightPointComponent::~LightPointComponent()
{
}

// set light properties
void LightPointComponent::Init(float radius, float intensity, XMFLOAT3 color, XMFLOAT3 specularColor, float specularPower, float attConstant, float attLinear, float attExponential)
{
	_radius         = radius;
	_intensity      = intensity;
	_color          = color;
	_specularColor  = specularColor;
	_specularPower  = specularPower;
	_attConstant    = attConstant;
	_attLinear      = attLinear;
	_attExponential = attExponential;
}

void LightPointComponent::Update(const float& delta)
{
}