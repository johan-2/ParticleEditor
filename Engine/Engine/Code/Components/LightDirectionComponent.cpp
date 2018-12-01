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

void LightDirectionComponent::Init(XMFLOAT4 lightColor, TransformComponent* transformOverride)
{
	// set light properties
	_lightColor = lightColor;

	// get pointer to transform
	_transform = GetComponent<TransformComponent>();

	_overrideTransform = transformOverride;

	// set this light to be used for rendering
	Systems::lightManager->SetDirectionalLight(this);
}

XMFLOAT3 LightDirectionComponent::GetLightDirection()
{
	if (_overrideTransform != nullptr)
		return _overrideTransform->GetForward();

	return _transform->GetForward();
}

void LightDirectionComponent::Update(const float& delta)
{
}
