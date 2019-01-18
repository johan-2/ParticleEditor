#pragma once
#include <vector>

class SkyDome;
class SponzaTestScene

{
public:
	SponzaTestScene();
	~SponzaTestScene();

	void Update();

private:

	SkyDome* _skyDome;
};

