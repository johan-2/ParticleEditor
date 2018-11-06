#include "CameraManager.h"

CameraManager* CameraManager::_instance = 0;

// get and/or initialize the static instance of this class
CameraManager& CameraManager::GetInstance()
{
	if (_instance == 0)
		_instance = new CameraManager;

	return *_instance;
}

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
}
