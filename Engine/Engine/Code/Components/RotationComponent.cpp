#include "RotationComponent.h"
#include "Time.h"


RotationComponent::RotationComponent() : IComponent(COMPONENT_TYPE::TRANSFORMATION_COMPONENT)
{
	
}

RotationComponent::~RotationComponent()
{

}

void RotationComponent::Init(XMFLOAT3 rotation, float speedRotation)
{
	_rotation = rotation;
	_speedRotation = speedRotation;
	
	_transform = GetComponent<TransformComponent>();
}

void RotationComponent::Update() 
{	
	const float& delta = Time::GetInstance().GetDeltaTime();

	XMFLOAT3 zeroVector(0, 0, 0);

	if (!XMVector3Equal(XMLoadFloat3(&_rotation), XMLoadFloat3(&zeroVector)))
	{
		float rotationAmount = _speedRotation * delta;
		XMFLOAT3 newRotation(0, 0, 0);

		XMStoreFloat3(&newRotation, XMVectorMultiply(XMLoadFloat3(&_rotation), XMLoadFloat3(&XMFLOAT3(rotationAmount, rotationAmount, rotationAmount))));

		_transform->AddRotation(newRotation);
		_transform->UpdateWorldMatrix();
	}
}