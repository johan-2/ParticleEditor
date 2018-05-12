#include "CameraManager.h"


CameraManager* CameraManager::_instance = 0;

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
