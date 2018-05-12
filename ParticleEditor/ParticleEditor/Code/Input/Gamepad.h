#pragma once

#include <Windows.h>
#include <Xinput.h>

class Gamepad 
{
public:
	Gamepad(int id);
	
	~Gamepad();

	

	void Update();

	bool LStickDead();
	bool RStickDead();

	float GetLeftStickX();
	float GetLeftStickY();
	float GetRightStickX();
	float GetRightStickY();
	
	float GetLeftTrigger();
	float GetRightTrigger();

	void SetRumble(float leftMotor, float rightMotor);

	bool IsButtonPressed(WORD button);
	bool WasButtonPressed(WORD button);
	bool WasButtonRealesed(WORD button);
private:

	XINPUT_STATE _previousState;
	XINPUT_STATE _currentState;	
	int _id;

	
};
