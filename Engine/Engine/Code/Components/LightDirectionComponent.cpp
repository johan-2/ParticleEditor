#include "LightDirectionComponent.h"
#include "LightManager.h"

LightDirectionComponent::LightDirectionComponent() : IComponent(COMPONENT_TYPE::DIR_LIGHT_COMPONENT)
{
}

LightDirectionComponent::~LightDirectionComponent()
{
	LightManager::GetInstance().RemoveDirectionalLight();
}

void LightDirectionComponent::Init(XMFLOAT4 lightColor, XMFLOAT4 specularColor, float specularPower)
{
	_lightColor = lightColor;
	_specularColor = specularColor;
	_specularPower = specularPower;

	_transform = GetComponent<TransformComponent>();
	LightManager::GetInstance().SetDirectionalLight(this);	
}

void LightDirectionComponent::Update()
{
}
