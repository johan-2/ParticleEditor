#pragma once

const float SCREEN_WIDTH = 1920.0f;
const float SCREEN_HEIGHT = 1080.0f;

#define V_SYNC false
#define FULLSCREEN false

//////////////////////////////////////////////
// POST PROCESSING
// BLOOM
#define APPLY_BLOOM 1
#define BLOOM_INTENSITY 1.0f
#define BLOOM_USE_TWO_PASS_BLUR true
#define BLOOM_BLUR_SCALE_DOWN_PASS_1 2
#define BLOOM_BLUR_SCALE_DOWN_PASS_2 8
