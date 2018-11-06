#include "TransformComponent.h"

TransformComponent::TransformComponent() : IComponent(COMPONENT_TYPE::TRANSFORM_COMPONENT)
{	
}

TransformComponent::~TransformComponent()
{	
}

// inits the transform properties and builds the world matrix
void TransformComponent::Init(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
	_position = position;
	_rotation = rotation;
	_scale    = scale;

	UpdateWorldMatrix();
}

// add value to position
void TransformComponent::AddTranslation(XMFLOAT3& amount)
{
	XMFLOAT3 zeroVector(0, 0, 0);

	if (XMVector3Equal(XMLoadFloat3(&amount), XMLoadFloat3(&zeroVector)))
		return;

	XMStoreFloat3(&_position, XMVectorAdd(XMLoadFloat3(&amount), XMLoadFloat3(&_position)));
}

// add value to rotation
void TransformComponent::AddRotation(XMFLOAT3& amount)
{
	XMFLOAT3 zeroVector(0, 0, 0);

	if (XMVector3Equal(XMLoadFloat3(&amount), XMLoadFloat3(&zeroVector)))
		return;

	XMStoreFloat3(&_rotation, XMVectorAdd(XMLoadFloat3(&amount), XMLoadFloat3(&_rotation)));
}

// add value to scale
void TransformComponent::AddScale(XMFLOAT3& amount)
{
	XMFLOAT3 zeroVector(0, 0, 0);

	if (XMVector3Equal(XMLoadFloat3(&amount), XMLoadFloat3(&zeroVector)))
		return;

	XMStoreFloat3(&_scale, XMVectorAdd(XMLoadFloat3(&amount), XMLoadFloat3(&_scale)));
}

// calculate all matrices for position, scale, rotation and mutliply them all together
void TransformComponent::UpdateWorldMatrix() 
{		
	XMStoreFloat4x4(&_positionMatrix, XMMatrixTranslationFromVector(XMLoadFloat3(&_position)));
	XMStoreFloat4x4(&_scaleMatrix,    XMMatrixScalingFromVector(XMLoadFloat3(&_scale)));
	XMStoreFloat4x4(&_rotationMatrix, XMMatrixRotationRollPitchYaw(XMConvertToRadians(_rotation.x), XMConvertToRadians(_rotation.y), XMConvertToRadians(_rotation.z)));

	XMStoreFloat4x4(&_worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&_scaleMatrix), XMLoadFloat4x4(&_rotationMatrix)));
	XMStoreFloat4x4(&_worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&_worldMatrix), XMLoadFloat4x4(&_positionMatrix)));
}

// get our forward orientation
XMFLOAT3 TransformComponent::GetForward()
{
	XMFLOAT3 f = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMStoreFloat3(&f, XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&f), XMLoadFloat4x4(&_rotationMatrix))));

	return f;
}

// get our right orientation
XMFLOAT3 TransformComponent::GetRight()
{
	XMFLOAT3 r = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMStoreFloat3(&r, XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&r), XMLoadFloat4x4(&_rotationMatrix))));

	return r;
}

// get our up orientation
XMFLOAT3 TransformComponent::GetUp()
{
	XMFLOAT3 u = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMStoreFloat3(&u, XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&u), XMLoadFloat4x4(&_rotationMatrix))));

	return u;
}

// set all orientations by the references passed in
void TransformComponent::GetAllAxis(XMFLOAT3& forward, XMFLOAT3& right, XMFLOAT3& up)
{
	XMFLOAT3 f = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 r = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 u = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMStoreFloat3(&forward, XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&f), XMLoadFloat4x4(&_rotationMatrix))));
	XMStoreFloat3(&right,   XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&r), XMLoadFloat4x4(&_rotationMatrix))));
	XMStoreFloat3(&up,      XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&u), XMLoadFloat4x4(&_rotationMatrix))));
}

void TransformComponent::Update(){}

