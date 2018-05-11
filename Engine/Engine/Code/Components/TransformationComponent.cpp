#include "TransformationComponent.h"
#include "Time.h"


TransformationComponent::TransformationComponent() : IComponent(COMPONENT_TYPE::TRANSFORMATION_COMPONENT)
{

}

TransformationComponent::~TransformationComponent()
{

}

void TransformationComponent::Init(XMFLOAT3 rotation, float speedRotation, float speedTranslation, TransformComponent::Axis axis)
{
	_rotation = rotation;
	_speedRotation = speedRotation;
	_speedTranslation = speedTranslation;
	_axis = axis;

	_transform = GetComponent<TransformComponent>();
}


void TransformationComponent::Update() 
{

	const float& delta = Time::GetInstance().GetDeltaTime();

	float rotationAmount = _speedRotation * delta;

	XMStoreFloat3(&_transform->GetRotationRef(), XMVectorAdd(&_transform->GetRotationRef(), XMVectorMultiply(XMLoadFloat3(&_rotation), XMLoadFloat(&rotationAmount))));

	if (_axis == TransformComponent::Axis::NONE)
		return;

	float translationAmount = _speedTranslation * delta;

	switch (_axis)
	{
	case TransformComponent::RIGHT:
		XMStoreFloat3(&_transform->GetPositionRef(), XMVectorAdd(&_transform->GetPositionRef(), XMVectorMultiply(XMLoadFloat3(&_transform->GetRight()), XMLoadFloat(&translationAmount))));
		break;
	case TransformComponent::FORWARD:
		XMStoreFloat3(&_transform->GetPositionRef(), XMVectorAdd(&_transform->GetPositionRef(), XMVectorMultiply(XMLoadFloat3(&_transform->GetForward()), XMLoadFloat(&translationAmount))));
		break;
	case TransformComponent::UP:
		XMStoreFloat3(&_transform->GetPositionRef(), XMVectorAdd(&_transform->GetPositionRef(), XMVectorMultiply(XMLoadFloat3(&_transform->GetUp()), XMLoadFloat(&translationAmount))));
		break;
	
	}

	

}