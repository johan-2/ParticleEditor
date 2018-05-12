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
}

XMFLOAT4X4 TransformComponent::GetWorldMatrix() 
{
	
	XMFLOAT4X4 matrixPosition, matrixScale, matrixRotation, matrixWorld;

	XMStoreFloat4x4(&matrixPosition, XMMatrixIdentity());
	XMStoreFloat4x4(&matrixScale,    XMMatrixIdentity());
	XMStoreFloat4x4(&matrixRotation, XMMatrixIdentity());
	XMStoreFloat4x4(&matrixWorld,    XMMatrixIdentity());
	
	XMStoreFloat4x4(&matrixPosition, XMMatrixTranslationFromVector(XMLoadFloat3(&_position)));
	XMStoreFloat4x4(&matrixScale,    XMMatrixScalingFromVector(XMLoadFloat3(&_scale)));
	XMStoreFloat4x4(&matrixRotation, XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z)));

	XMStoreFloat4x4(&matrixWorld, XMMatrixMultiply(XMLoadFloat4x4(&matrixScale), XMLoadFloat4x4(&matrixRotation)));
	XMStoreFloat4x4(&matrixWorld, XMMatrixMultiply(XMLoadFloat4x4(&matrixWorld), XMLoadFloat4x4(&matrixPosition)));
	
	// return all matrices multiplied
	return matrixWorld;	
}

XMFLOAT3 TransformComponent::CalculateAxises(Axis axis, XMFLOAT3& forward, XMFLOAT3& right, XMFLOAT3& up)
{
	// get rotation matrix
	XMFLOAT4X4 matrixRotation; 
	XMStoreFloat4x4(&matrixRotation, XMMatrixIdentity());

	XMStoreFloat4x4(&matrixRotation, XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z)));

	XMFLOAT3 f = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 r = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 u = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMStoreFloat3(&f, XMVector3TransformCoord(XMLoadFloat3(&f), XMLoadFloat4x4(&matrixRotation)));
	XMStoreFloat3(&r, XMVector3TransformCoord(XMLoadFloat3(&r), XMLoadFloat4x4(&matrixRotation)));
	XMStoreFloat3(&u, XMVector3TransformCoord(XMLoadFloat3(&u), XMLoadFloat4x4(&matrixRotation)));

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

