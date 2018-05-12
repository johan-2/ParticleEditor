#pragma once

#include <dinput.h>
#include <map>

#define MOUSE0 0
#define MOUSE1 1
#define MOUSE2 2
#define MOUSE3 3

class Input
{
public:
	Input();
	~Input();
	
	static Input& GetInstance();

	void InitializeInputDevices(HINSTANCE hinstance, HWND hwnd);
	void Update();
		
	bool IsKeyHeld(unsigned char key);
	bool IskeyPressed(unsigned char key);
	bool IsKeyReleased(unsigned char key);

	bool IsMouseHeld(unsigned char button);
	bool IsMousePressed(unsigned char button);
	bool IsMouseReleased(unsigned char button);

	float GetMouseX();
	float GetMouseY();

private:

	void UpdateKeyboard();
	void UpdateMouse();
	void UpdateGuiInput();

	IDirectInput8* _directInput;
	IDirectInputDevice8* _keyboard;
	IDirectInputDevice8* _mouse;

	unsigned char _currentkeyboardState[256];
	unsigned char _previouskeyboardState[256];

	DIMOUSESTATE _currentMouseState;
	DIMOUSESTATE _previousMouseState;

	HWND _hwnd;

	static Input* _instance;
	std::map <unsigned int, bool> keys;

};


