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

	XMStoreFloat3(&_transform->GetRotationRef(), XMVectorAdd(XMLoadFloat3(&_transform->GetRotationRef()), XMVectorMultiply(XMLoadFloat3(&_rotation), XMLoadFloat3(&XMFLOAT3(rotationAmount, rotationAmount, rotationAmount)))));
	
	if (_axis == TransformComponent::Axis::NONE)
		return;

	float translationAmount = _speedTranslation * delta;

	switch (_axis)
	{
	case TransformComponent::RIGHT:
		XMStoreFloat3(&_transform->GetPositionRef(), XMVectorAdd(XMLoadFloat3(&_transform->GetPositionRef()), XMVectorMultiply(XMLoadFloat3(&_transform->GetRight()), XMLoadFloat3(&XMFLOAT3(translationAmount,translationAmount,translationAmount)))));
		break;
	case TransformComponent::FORWARD:
		XMStoreFloat3(&_transform->GetPositionRef(), XMVectorAdd(XMLoadFloat3(&_transform->GetPositionRef()), XMVectorMultiply(XMLoadFloat3(&_transform->GetForward()), XMLoadFloat3(&XMFLOAT3(translationAmount, translationAmount, translationAmount)))));
		break;
	case TransformComponent::UP:
		XMStoreFloat3(&_transform->GetPositionRef(), XMVectorAdd(XMLoadFloat3(&_transform->GetPositionRef()), XMVectorMultiply(XMLoadFloat3(&_transform->GetUp()), XMLoadFloat3(&XMFLOAT3(translationAmount, translationAmount, translationAmount)))));
		break;
	
	}
}