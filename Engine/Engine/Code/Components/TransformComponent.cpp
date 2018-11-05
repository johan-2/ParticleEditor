#include "TransformComponent.h"

TransformComponent::TransformComponent() : IComponent(COMPONENT_TYPE::TRANSFORM_COMPONENT)
{	
}

TransformComponent::~TransformComponent()
{	
}

void TransformComponent::Init(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
	_position = position;
	_rotation = rotation;
	_scale = scale;
	UpdateWorldMatrix();
}

void TransformComponent::AddTranslation(XMFLOAT3& amount)
{
	XMFLOAT3 zeroVector(0, 0, 0);

	if (XMVector3Equal(XMLoadFloat3(&amount), XMLoadFloat3(&zeroVector)))
		return;

	XMStoreFloat3(&_position, XMVectorAdd(XMLoadFloat3(&amount), XMLoadFloat3(&_position)));

	XMStoreFloat4x4(&_positionMatrix, XMMatrixTranslationFromVector(XMLoadFloat3(&_position)));
}

void TransformComponent::AddRotation(XMFLOAT3& amount)
{
	XMFLOAT3 zeroVector(0, 0, 0);

	if (XMVector3Equal(XMLoadFloat3(&amount), XMLoadFloat3(&zeroVector)))
		return;

	XMStoreFloat3(&_rotation, XMVectorAdd(XMLoadFloat3(&amount), XMLoadFloat3(&_rotation)));

	XMStoreFloat4x4(&_rotationMatrix, XMMatrixTranslationFromVector(XMLoadFloat3(&_rotation)));
}

void TransformComponent::AddScale(XMFLOAT3& amount)
{
	XMFLOAT3 zeroVector(0, 0, 0);

	if (XMVector3Equal(XMLoadFloat3(&amount), XMLoadFloat3(&zeroVector)))
		return;

	XMStoreFloat3(&_scale, XMVectorAdd(XMLoadFloat3(&amount), XMLoadFloat3(&_scale)));

	XMStoreFloat4x4(&_scaleMatrix, XMMatrixTranslationFromVector(XMLoadFloat3(&_scale)));
}

void TransformComponent::UpdateWorldMatrix() 
{		
	XMStoreFloat4x4(&_positionMatrix, XMMatrixTranslationFromVector(XMLoadFloat3(&_position)));
	XMStoreFloat4x4(&_scaleMatrix,    XMMatrixScalingFromVector(XMLoadFloat3(&_scale)));
	XMStoreFloat4x4(&_rotationMatrix, XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z)));

	XMStoreFloat4x4(&_worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&_scaleMatrix), XMLoadFloat4x4(&_rotationMatrix)));
	XMStoreFloat4x4(&_worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&_worldMatrix), XMLoadFloat4x4(&_positionMatrix)));
}

XMFLOAT3 TransformComponent::CalculateAxises(Axis axis, XMFLOAT3& forward, XMFLOAT3& right, XMFLOAT3& up)
{
	XMFLOAT3 f = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 r = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 u = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMStoreFloat3(&f, XMVector3TransformCoord(XMLoadFloat3(&f), XMLoadFloat4x4(&_rotationMatrix)));
	XMStoreFloat3(&r, XMVector3TransformCoord(XMLoadFloat3(&r), XMLoadFloat4x4(&_rotationMatrix)));
	XMStoreFloat3(&u, XMVector3TransformCoord(XMLoadFloat3(&u), XMLoadFloat4x4(&_rotationMatrix)));

	switch (axis)
	{
	case TransformComponent::FORWARD:

		XMStoreFloat3(&f, XMVector3Normalize(XMLoadFloat3(&f)));
		return f;
		break;
	case TransformComponent::RIGHT:
		XMStoreFloat3(&r, XMVector3Normalize(XMLoadFloat3(&r)));
		return r;
		break;
	case TransformComponent::UP:
		XMStoreFloat3(&u, XMVector3Normalize(XMLoadFloat3(&u)));
		return u;
		break;	
	case TransformComponent::ALL:
		XMStoreFloat3(&forward, XMVector3Normalize(XMLoadFloat3(&f)));
		XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&r)));
		XMStoreFloat3(&up, XMVector3Normalize(XMLoadFloat3(&u)));
		break;
	}

	return XMFLOAT3(0,0,0);
}

void TransformComponent::Update() 
{

}

