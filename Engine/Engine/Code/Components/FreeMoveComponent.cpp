#include "FreeMoveComponent.h"
#include <DirectXMath.h>
#include "input.h"
#include "Time.h"
#include "TransformComponent.h"
#include "Gamepad.h"
#include "SystemDefs.h"

FreeMoveComponent::FreeMoveComponent() : IComponent(COMPONENT_TYPE::FREE_MOVE_COMPONENT)
{
}

FreeMoveComponent::~FreeMoveComponent()
{
}

void FreeMoveComponent::init(float movementSpeed, float rotationSpeed)
{	
	_moveSpeed     = movementSpeed;
	_rotationSpeed = rotationSpeed;
	_transform     = GetComponent<TransformComponent>();
}

void FreeMoveComponent::Update()
{	
	UpdateMovement();		
}

void FreeMoveComponent::UpdateMovement()
{	
	// get delta and access to input
	const float& deltaTime = Time::GetInstance().GetDeltaTime();
	Input& input           = Input::GetInstance();

	// get all axises of our transform
	XMFLOAT3 forward, right, up;
	_transform->GetAllAxis(forward, right, up);
	
	XMFLOAT3 translation(0, 0, 0);

	// get how much we should move
	float movement = _moveSpeed * deltaTime;

	// get mouse movement
	float mouseX = input.GetMouseX();
	float mouseY = input.GetMouseY();

	XMFLOAT2 keyInput(0,0);
	
	// get keyboard input values
	if (input.IsKeyHeld(DIK_A))	keyInput.x -= 1.0f;
	if (input.IsKeyHeld(DIK_D)) keyInput.x += 1.0f;
	if (input.IsKeyHeld(DIK_S)) keyInput.y -= 1.0f;
	if (input.IsKeyHeld(DIK_W)) keyInput.y += 1.0f;	

	// get normalized move direction based on orientation of transform and keyboard input
	XMFLOAT3 direction; 
	XMStoreFloat3(&direction,
		XMVector3Normalize(
		XMVectorAdd(
			XMVectorMultiply(XMLoadFloat3(&right),   XMLoadFloat3(&XMFLOAT3(keyInput.x, keyInput.x, keyInput.x))),
			XMVectorMultiply(XMLoadFloat3(&forward), XMLoadFloat3(&XMFLOAT3(keyInput.y, keyInput.y, keyInput.y))))));
	
	// get translation amount based on direction and how much we should move
	XMStoreFloat3(&translation, XMVectorMultiply(XMLoadFloat3(&direction), XMLoadFloat3(&XMFLOAT3(movement, movement, movement))));
	
	// add rotation and translation to transform
	_transform->AddRotation(XMFLOAT3(mouseY * _rotationSpeed, mouseX * _rotationSpeed, 0));
	_transform->AddTranslation(translation);

	// calculate new world matrix
	_transform->UpdateWorldMatrix();
}


