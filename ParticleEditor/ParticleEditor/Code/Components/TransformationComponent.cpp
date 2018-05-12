#include "TransformationComponent.h"
#include "Time.h"


TransformationComponent::TransformationComponent() : IComponent(COMPONENT_TYPE::TRANSFORMATION_COMPONENT),
	_state(IDLE),
	_lastState(IDLE),
	_startPosCircle(XMFLOAT3(1, 0, 8.f)),
	_circleSpeed(5.0f),
	_moveSpeed(1.0f),
	_sinTimer(0.0f),
	_sinValue(0.0f),
	_backForthLength(10.0f)
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

	if (_state == IDLE)
	{
		_lastState = IDLE;
		_transform->GetPositionRef() = XMFLOAT3(0, 0, 0);
	}
	else if (_state == CIRCLE)
	{
		if (_lastState != CIRCLE)
		{
			_transform->GetPositionRef() = _startPosCircle;
			_circleRotation = XMFLOAT3(0, 0, 0);
			_lastState = CIRCLE;
		}

		_circleRotation.y += 40 * delta;

		// get rotation matrix
		XMFLOAT4X4 matrixRotation; XMStoreFloat4x4(&matrixRotation, XMMatrixIdentity());
		XMStoreFloat4x4(&matrixRotation, XMMatrixRotationRollPitchYaw(XMConvertToRadians(_circleRotation.x), XMConvertToRadians(_circleRotation.y), XMConvertToRadians(_circleRotation.z)));
				
		XMFLOAT3 r = XMFLOAT3(1.0f, 0.0f, 0.0f);
		XMStoreFloat3(&r, XMVector3TransformCoord(XMLoadFloat3(&r), XMLoadFloat4x4(&matrixRotation)));
		XMStoreFloat3(&r, XMVector3Normalize(XMLoadFloat3(&r)));
		
		float movement = _circleSpeed * delta;
		XMStoreFloat3(&_transform->GetPositionRef(), XMVectorAdd(XMLoadFloat3(&_transform->GetPositionRef()), XMVectorMultiply(XMLoadFloat3(&r), XMLoadFloat3(&XMFLOAT3(movement, movement, movement)))));

	}
	else if (_state == BACKFORTH)
	{
		if (_lastState != BACKFORTH)
		{
			_transform->GetPositionRef() = XMFLOAT3(0, 0, 0);
			_lastState = BACKFORTH;
			_sinTimer = 0;
			_sinValue = 0;
		}

		_sinTimer += _moveSpeed * delta;
		_sinValue = std::sin(_sinTimer);

		XMStoreFloat3(&_transform->GetPositionRef(), XMVectorMultiply(XMLoadFloat3(&XMFLOAT3(_backForthLength, 0, 0)), XMLoadFloat3(&XMFLOAT3(_sinValue, _sinValue, _sinValue))));
	}
}