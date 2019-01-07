#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "SystemSettingsWindow.h"

//////////////////////////////////////////////// SYSTEM SETTINGS
class SystemSettings
{
public:
	SystemSettings();
	~SystemSettings();

	static float SCREEN_WIDTH;
	static float SCREEN_HEIGHT;
	static bool  V_SYNC;
	static bool  FULLSCREEN;
	static bool  USE_HDR;

	static void ReadSettings(const char* file);
	static void WriteSettings(const char* file, SystemSettingsWindow::Settings settings);
};

//////////////////////////////////////////////// POST PROCESSING
class PostProcessing	
{
public:
	PostProcessing();
	~PostProcessing();

	// BLOOM
	static bool  APPLY_BLOOM; 
	static bool  BLOOM_USE_TWO_PASS_BLUR; 
	static float BLOOM_INTENSITY; 
	static float BLOOM_BLUR_SCALE_DOWN_PASS_1; 
	static float BLOOM_BLUR_SCALE_DOWN_PASS_2; 

	// DEPTH OF FIELD
	static bool  APPLY_DEPTH_OF_FIELD;
	static float START_END_DOF_DST[2];

	static void ReadSettings(const char* file);
	static void WriteSettings(const char* file);	
};


