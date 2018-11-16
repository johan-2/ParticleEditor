#pragma once

#include <Windows.h>
class Time
{
public:
	Time();
	~Time();

	void Update();
	
	const float& GetDeltaTime() { return _delta; }
	const int& GetFps() { return _fps; }

	void OnWindowChange();	

private:

	void CalculateDelta();
	void CalculateFPS();

	// for delta
	float _ticksPerSecond;	
	INT64 _startTimeDelta;
	float _delta;

	bool _useLastDelta;
	float _lastDelta;

	// for fps
	int _fps,_count;
	unsigned long _startTimeFps;

};

