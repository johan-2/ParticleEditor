#pragma once
#include "IEditorWindow.h"
#include <DirectXMath.h>

using namespace DirectX;
class MasterEditor;

class SkyDomeWindow : public IEditorWindow
{
public:
	SkyDomeWindow(MasterEditor* masterEditor);
	virtual ~SkyDomeWindow();

	void Render();

	void ShowToolTip(const char* tip);
	void ColorPickerNoAlpha(const char* ID, const char* header, const char* pickerName, const char* toolTip, XMFLOAT4* data);
	void FloatSlider(const char* ID, const char* header, const char* toolTip, float min, float max, float* data);
	void FloatSlider2(const char* ID, const char* header, const char* toolTip, float min, float max, float* data);
	void FloatInput3(const char* ID, const char* header, const char* toolTip, float* data);
	void FloatInput(const char* ID, const char* header, const char* toolTip, float* data);
};

