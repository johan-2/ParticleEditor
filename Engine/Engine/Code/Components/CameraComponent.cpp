#include "CameraComponent.h"
#include "SystemDefs.h"
#include "TransformComponent.h"
#include "MathHelpers.h"

CameraComponent::CameraComponent() : IComponent(COMPONENT_TYPE::CAMERA_COMPONENT)
{
}

void CameraComponent::Init3D(const float& fov)
{
	// get ptr to transform component
	_transform = GetComponent<TransformComponent>();

	// Create the projection matrix for 3D rendering.	
	XMStoreFloat4x4(&_projectionMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 50000.0f));

	CalculateViewMatrix();
}

void CameraComponent::Init2D(const XMFLOAT2& size, const XMFLOAT2& nearfar)
{
	// get ptr to transform component
	_transform = GetComponent<TransformComponent>();

	// create ortho projection
	XMStoreFloat4x4(&_projectionMatrix, XMMatrixOrthographicLH(size.x, size.y, nearfar.x, nearfar.y));

	CalculateViewMatrix();
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Update(const float& delta)
{		
	CalculateViewMatrix();	
}

// calculates the view matrix from the position, forward and up vectors
void CameraComponent::CalculateViewMatrix() 
{
	XMFLOAT3 forward, right, up;
	const XMFLOAT3& pos = _transform->GetPositionRef();
	_transform->GetAllAxis(forward, right, up);
	
	// create the viewMatrix based on our position, look direction and updirection of the camera	
	XMStoreFloat3(&forward, XMVectorAdd(XMLoadFloat3(&pos), XMLoadFloat3(&forward)));

	XMStoreFloat4x4(&_viewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&pos), XMLoadFloat3(&forward), XMLoadFloat3(&up)));

	// create viewProjection matrices
	XMStoreFloat4x4(&_viewProjMatrix, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiply(&_viewMatrix, &_projectionMatrix)));
	XMStoreFloat4x4(&_viewProjMatrixTrans, XMMatrixTranspose(XMLoadFloat4x4(&_viewProjMatrix)));
}

XMFLOAT4X4 CameraComponent::GetReflectionViewProj(float yPosition, bool viewOnly)
{
	XMFLOAT4X4 rotationMatrix;
	XMFLOAT4X4 viewMatrix;

	XMFLOAT3 rotation = _transform->GetRotationVal();
	XMFLOAT3 position = _transform->GetPositionVal();

	position.y -= 2.0f * (position.y - yPosition);
	rotation.x = -rotation.x;

	XMFLOAT3 rotRadian(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
	
	XMStoreFloat4x4(&rotationMatrix, XMMatrixRotationRollPitchYaw(rotRadian.x, rotRadian.y, rotRadian.z));

	XMFLOAT3 f = XMFLOAT3(0.0f, 0.0f, 1.0f);	
	XMFLOAT3 u = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMFLOAT3 forward;
	XMFLOAT3 up;

	XMStoreFloat3(&forward, XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&f), XMLoadFloat4x4(&rotationMatrix))));	
	XMStoreFloat3(&up, XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&u), XMLoadFloat4x4(&rotationMatrix))));

	// create the viewMatrix based on our position, look direction and updirection of the camera	
	XMStoreFloat3(&forward, XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&forward)));

	XMStoreFloat4x4(&viewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&forward), XMLoadFloat3(&up)));

	if (!viewOnly)
		XMStoreFloat4x4(&viewMatrix, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiply(&viewMatrix, &_projectionMatrix)));

	return viewMatrix;
}