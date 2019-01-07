#include "LightPointComponent.h"
#include "Entity.h"
#include "LightManager.h"
#include "Systems.h"

LightPointComponent::LightPointComponent() : IComponent(POINT_LIGHT_COMPONENT)
{
}

LightPointComponent::~LightPointComponent()
{
}

// set light properties
void LightPointComponent::Init(float radius, float intensity, XMFLOAT3 color, float attConstant, float attLinear, float attExponential)
{
	_radius         = radius;
	_intensity      = intensity;
	_color          = color;
	_attConstant    = attConstant;
	_attLinear      = attLinear;
	_attExponential = attExponential;

	Systems::lightManager->AddPointLight(this);
}

void LightPointComponent::Update(const float& delta)
{
}