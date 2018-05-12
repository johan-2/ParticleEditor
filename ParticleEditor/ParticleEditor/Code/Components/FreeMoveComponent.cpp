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
	//// get ptr to transform component
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

	if (input.GetInstance().IskeyPressed(DIK_F1))
		_active = !_active;

	if (!_active)
		return;

	XMFLOAT3 forward, right;
	_transform->GetAllAxis(forward, right, XMFLOAT3(0,0,0));

	XMFLOAT3& rotation = _transform->GetRotationRef();
	XMFLOAT3& pos = _transform->GetPositionRef();

	_gamepad->Update();

	// CONTROLLER
	//get stick values
	XMFLOAT2 rightStickInput(_gamepad->GetRightStickX(), _gamepad->GetRightStickY());
	XMFLOAT2 leftStickInput(_gamepad->GetLeftStickX(), _gamepad->GetLeftStickY());

	// get the normalized stick vector relative to the right and forward of the camera
	XMFLOAT3 leftStickX;
	XMStoreFloat3(&leftStickX, XMVectorMultiply(XMLoadFloat3(&right), XMLoadFloat(&leftStickInput.x)));

	XMFLOAT3 leftStickY;
	XMStoreFloat3(&leftStickY, XMVectorMultiply(XMLoadFloat3(&forward), XMLoadFloat(&leftStickInput.y)));

	XMFLOAT3 leftStick;
	XMStoreFloat3(&leftStick, XMVectorMultiply(XMLoadFloat3(&leftStickX), XMLoadFloat3(&leftStickY)));

	XMStoreFloat3(&leftStick, XMVector3Normalize(XMLoadFloat3(&leftStick)));

	if (!_gamepad->RStickDead())
		XMStoreFloat3(&rotation, XMVectorAdd(XMLoadFloat3(&rotation), XMLoadFloat3(&XMFLOAT3(rightStickInput.x * ROTATION_SPEED * deltaTime, -rightStickInput.y * ROTATION_SPEED * deltaTime, 0))));  

	float movement = MOVE_SPEED * deltaTime;

	if (!_gamepad->LStickDead())
		XMStoreFloat3(&pos, XMVectorAdd(XMLoadFloat3(&pos), XMVectorMultiply(XMLoadFloat3(&leftStick), XMLoadFloat(&movement))));

	// KEYBOARDMOUSE		
	// mouse rotation
	float mouseX = input.GetMouseX();
	float mouseY = input.GetMouseY();

	XMStoreFloat3(&rotation, XMVectorAdd(XMLoadFloat3(&rotation), XMLoadFloat3(&XMFLOAT3((mouseY * MOUSE_SENSITIVITY), (mouseX * MOUSE_SENSITIVITY), 0))));

	XMFLOAT2 keyboardInput(0,0);
	
	//keyboard movement
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
	XMStoreFloat3(&pos, XMVectorAdd(XMLoadFloat3(&pos), XMVectorMultiply(XMLoadFloat3(&direction), XMLoadFloat3(&XMFLOAT3(movement,movement,movement)))));
	
}


