#include "LightDirectionComponent.h"
#include "LightManager.h"
#include "Systems.h"

LightDirectionComponent::LightDirectionComponent() : IComponent(COMPONENT_TYPE::DIR_LIGHT_COMPONENT)
{
}

// removes this light as the active one when this component is destroyed
LightDirectionComponent::~LightDirectionComponent()
{
	Systems::lightManager->RemoveDirectionalLight();
}

void LightDirectionComponent::Init(XMFLOAT4 lightColor, XMFLOAT4 specularColor, float specularPower)
{
	// set light properties
	_lightColor    = lightColor;
	_specularColor = specularColor;
	_specularPower = specularPower;

	// get pointer to transform
	_transform = GetComponent<TransformComponent>();

	// set this light to be used for rendering
	Systems::lightManager->SetDirectionalLight(this);
}

void LightDirectionComponent::Update(const float& delta)
{
}
