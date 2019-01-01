#pragma once
#include "IEditorWindow.h"

class MasterEditor;

class SkyDomeWindow : public IEditorWindow
{
public:
	SkyDomeWindow(MasterEditor* masterEditor);
	virtual ~SkyDomeWindow();

	void Render();
};

