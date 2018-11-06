#pragma once
#include <D3D11.h>

// structure to keep info about our hardware
struct HardwareInfo
{
public:
	unsigned int  numerator;
	unsigned int  denominator;
	unsigned int  refreshrate;
	unsigned int  videoCardMemory;
	char          videoCardDescription[128];
};

class HardwareProperties
{
public:
	HardwareProperties(int screenWidth, int screenHeight);
	~HardwareProperties();

	// return hardware information
	const HardwareInfo& GetHardwareInfo() { return _hardwareInfo; }

private:

	// gets hardware info
	void GetHardwareInfo(int screenWidth, int screenHeight);
	
	// stores hardware info
	HardwareInfo _hardwareInfo;	
};

