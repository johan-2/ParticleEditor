#include "Gamepad.h"

Gamepad::Gamepad(int id)
{	
		ZeroMemory(&_previousState, sizeof(XINPUT_STATE));
		ZeroMemory(&_currentState, sizeof(XINPUT_STATE));		
		_id = id;
}

Gamepad::~Gamepad(){}

void Gamepad::Update() 
{		
		XINPUT_STATE state;
		if (XInputGetState(_id, &state) == ERROR_SUCCESS)
		{
			_previousState = _currentState;
			_currentState = state;			
		}					
}


bool Gamepad::LStickDead() 
{
	short X = _currentState.Gamepad.sThumbLX;
	short Y = _currentState.Gamepad.sThumbLY;

	if (X > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || X < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return false;

	if (Y > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || Y < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return false;

	return true;
}
bool Gamepad::RStickDead() 
{
	short X = _currentState.Gamepad.sThumbRX;
	short Y = _currentState.Gamepad.sThumbRY;

	if (X > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || X < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		return false;

	if (Y > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || Y < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		return false;

	return true;
}

float Gamepad::GetLeftStickX() 
{
	short X = _currentState.Gamepad.sThumbLX;

	// convert the short value to a float between -1 to 1
	return (static_cast <float> (X) / 32768.0f);
}

float Gamepad::GetLeftStickY() 
{
	short Y = _currentState.Gamepad.sThumbLY;
	
	return (static_cast <float> (Y) / 32768.0f);
}

float Gamepad::GetRightStickX() 
{
	short X = _currentState.Gamepad.sThumbRX;
	
	return (static_cast <float> (X) / 32768.0f);
}

float Gamepad::GetRightStickY() 
{
	short Y = _currentState.Gamepad.sThumbRY;

	return (static_cast <float> (Y) / 32768.0f);
}

float Gamepad::GetLeftTrigger() 
{
	BYTE trigger = _currentState.Gamepad.bLeftTrigger;

	return (static_cast <float> (trigger) / 255.0f);
}

float Gamepad::GetRightTrigger() 
{
	BYTE trigger = _currentState.Gamepad.bRightTrigger;

	return (static_cast <float> (trigger) / 255.0f);
}

void Gamepad::SetRumble(float leftMotor, float rightMotor) 
{
	XINPUT_VIBRATION state;

	ZeroMemory(&state, sizeof(XINPUT_VIBRATION));

	float left = leftMotor * 65535.0f;
	float right = rightMotor * 65535.0f;

	state.wLeftMotorSpeed = left;
	state.wRightMotorSpeed = right;

	XInputSetState(_id, &state);
}

bool Gamepad::IsButtonPressed(WORD button)
{
	return ((_currentState.Gamepad.wButtons & button) != 0);
}

bool Gamepad::WasButtonPressed(WORD button) 
{
	return (((_previousState.Gamepad.wButtons & button) == 0) && ((_currentState.Gamepad.wButtons & button) != 0));
}

bool Gamepad::WasButtonRealesed(WORD button)
{
	return (((_previousState.Gamepad.wButtons & button) != 0) && ((_currentState.Gamepad.wButtons & button) == 0));
}
