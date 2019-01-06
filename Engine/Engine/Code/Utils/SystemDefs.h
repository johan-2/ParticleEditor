#pragma once

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080
#define V_SYNC     false
#define FULLSCREEN false

//////////////////////////////////////////////
// POST PROCESSING
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

	static void ReadSettings(const char* file);
	static void WriteSettings(const char* file);	
};


