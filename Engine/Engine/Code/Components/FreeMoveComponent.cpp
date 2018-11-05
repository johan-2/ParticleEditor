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
	delete _gamepad;
}

void FreeMoveComponent::init()
{	
	_transform = GetComponent<TransformComponent>();
	_gamepad = new Gamepad(0);
}

void FreeMoveComponent::Update()
{	
	UpdateMovement();		
}

void FreeMoveComponent::UpdateMovement()
{	
	const float& deltaTime = Time::GetInstance().GetDeltaTime();
	Input& input = Input::GetInstance();

	XMFLOAT3 forward, right;
	_transform->GetAllAxis(forward, right, XMFLOAT3(0,0,0));
	
	XMFLOAT3 translation(0, 0, 0);

	float movement = MOVE_SPEED * deltaTime;

	float mouseX = input.GetMouseX();
	float mouseY = input.GetMouseY();

	XMFLOAT2 keyboardInput(0,0);
	
	if (input.IsKeyHeld(DIK_A))
		keyboardInput.x -= 1.0f;
	if (input.IsKeyHeld(DIK_D))
		keyboardInput.x += 1.0f;
	if (input.IsKeyHeld(DIK_S))
		keyboardInput.y -= 1.0f;
	if (input.IsKeyHeld(DIK_W))
		keyboardInput.y += 1.0f;	

	XMFLOAT3 direction; 
	XMStoreFloat3(&direction, XMVectorAdd(XMVectorMultiply(XMLoadFloat3(&right), XMLoadFloat3(&XMFLOAT3(keyboardInput.x, keyboardInput.x, keyboardInput.x))), XMVectorMultiply(XMLoadFloat3(&forward), XMLoadFloat3(&XMFLOAT3(keyboardInput.y, keyboardInput.y, keyboardInput.y)))));
	XMStoreFloat3(&direction, XMVector3Normalize(XMLoadFloat3(&direction)));
	XMStoreFloat3(&translation, XMVectorMultiply(XMLoadFloat3(&direction), XMLoadFloat3(&XMFLOAT3(movement, movement, movement))));
	
	_transform->AddRotation(XMFLOAT3(mouseY * MOUSE_SENSITIVITY, mouseX * MOUSE_SENSITIVITY, 0));
	_transform->AddTranslation(translation);

	_transform->UpdateWorldMatrix();
}


