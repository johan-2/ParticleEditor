#pragma once

#include <Windows.h>
class Time
{
public:
	Time();
	~Time();

	static Time& GetInstance();

	void Update();
	
	const float& GetDeltaTime() { return _delta; }
	const int& GetFps() { return _fps; }

private:

	void CalculateDelta();
	void CalculateFPS();

	// for delta
	float _ticksPerSecond;	
	INT64 _startTimeDelta;
	float _delta;

	// for fps
	int _fps,_count;
	unsigned long _startTimeFps;


	static Time* _instance;
};

