#include "CameraComponent.h"
#include "SystemDefs.h"
#include "TransformComponent.h"


CameraComponent::CameraComponent() : IComponent(COMPONENT_TYPE::CAMERA_COMPONENT)
{
}

void CameraComponent::Init3D(const float& fov)
{
	// get ptr to transform component
	_transform = GetComponent<TransformComponent>();

	// Create the projection matrix for 3D rendering.	
	XMStoreFloat4x4(&_projectionMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 500.0f));
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

void CameraComponent::Update() 
{		
	CalculateViewMatrix();	
}

// calculates the view matrix from the position, forward and up vectors
void CameraComponent::CalculateViewMatrix() 
{
	XMMATRIX rotationMatrix =  XMMatrixIdentity();

	XMFLOAT3 forward, right, up;
	const XMFLOAT3& pos = _transform->GetPositionRef();
	_transform->GetAllAxis(forward, right, up);
	
	// create the viewMatrix based on our position, look direction and updirection of the camera	
	XMStoreFloat3(&forward, XMVectorAdd(XMLoadFloat3(&pos), XMLoadFloat3(&forward)));

	XMStoreFloat4x4(&_viewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&pos), XMLoadFloat3(&forward), XMLoadFloat3(&up)));
}