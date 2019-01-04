#include "SkyDomeWindow.h"
#include "MasterEditor.h"
#include "imgui.h"
#include "Systemdefs.h"
#include "SkyDome.h"
#include "Systems.h"
#include "Renderer.h"
#include "TexturePool.h"

SkyDomeWindow::SkyDomeWindow(MasterEditor* masterEditor) : IEditorWindow(masterEditor)
{
}

SkyDomeWindow::~SkyDomeWindow()
{
}

void SkyDomeWindow::Render()
{
	SkyDome* skyDome         = Systems::renderer->GetSkyDome();
	SkySettings* skySettings = skyDome->GetSkySettings();
	SunMoon* sunMoon         = skyDome->GetSoonMoonSettings();
	ID3D11ShaderResourceView* previewTex = Systems::texturePool->GetTexture(L"Textures/domeMap.dds");

	// set properties of next window;
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 0.20f, SCREEN_HEIGHT * 0.8f));
	ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));

	// create window to display information
	ImGui::Begin("Skydome Settings", nullptr);

	// speed 
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.10f);
	FloatSlider("28", "Time Multiplier", "one cycle is set to 1 minute, alter the duration with this multiplier", 0.0f, 20.0f, &skySettings->speedMultiplier);
	ImGui::Spacing(1);
	ImGui::PopItemWidth();

	// sky color layers percentage
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "SKY LAYERS USAGE PERCENT");
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.05f);
	FloatSlider("1", "Bottom Layer Percent", "this color will be used from 0 to set value",   0.0f,                          skySettings->skyMidColor.w, &skySettings->skyBottomColor.w);
	FloatSlider("2", "Mid Layer Percent", "this color will be used from bottom to set value", skySettings->skyBottomColor.w, skySettings->skyTopColor.w, &skySettings->skyMidColor.w);
	FloatSlider("3", "Top Layer Percent", "this color will be used from mid to set value",    skySettings->skyMidColor.w,    100.0f,                     &skySettings->skyTopColor.w);
	ImGui::PopItemWidth();

	// sky colors
	ImGui::Spacing(10);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "SKY COLORS");
	ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1), "Top Colors:");
	ImGui::SameLine();
	ColorPickerNoAlpha("11", "Day",    "the color of the top part of skydome during day",    "pick4", &skySettings->topSkyColorDay);
	ImGui::SameLine();
	ColorPickerNoAlpha("12", "Sunset", "the color of the top part of skydome during Sunset", "pick5", &skySettings->topSkyColorSunSet);
	ImGui::SameLine();
	ColorPickerNoAlpha("13", "Night",  "the color of the top part of skydome during Night",  "pick6", &skySettings->topSkyColorNight);

	ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1), "Mid Colors:");
	ImGui::SameLine();
	ColorPickerNoAlpha("14", "Day",    "the color of the mid part of skydome during day",    "pick7", &skySettings->midSkyColorDay);
	ImGui::SameLine();
	ColorPickerNoAlpha("15", "Sunset", "the color of the mid part of skydome during Sunset", "pick8", &skySettings->midSkyColorSunSet);
	ImGui::SameLine();
	ColorPickerNoAlpha("16", "Night",  "the color of the mid part of skydome during Night",  "pick9", &skySettings->midSkyColorNight);

	ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1), "Cur Colors:");
	ImGui::SameLine();
	ColorPickerNoAlpha("4", "Bottom", "the color of the bottom part of skydome", "pick1", &skySettings->skyBottomColor);
	ImGui::SameLine();
	ColorPickerNoAlpha("5", "Mid", "the color of the middle part of skydome",    "pick2", &skySettings->skyMidColor);
	ImGui::SameLine();
	ColorPickerNoAlpha("6", "Top", "the color of the top part of skydome",       "pick3", &skySettings->skyTopColor);

	// blend timings
	ImGui::Spacing(10);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "SKY COLORS BLEND TIMINGS");
	ImGui::Image((void*)previewTex, ImVec2(128, 128), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.06f);
	FloatSlider2("7",  "Blend Top Sunset", "when the top sky color will start blending from day to sunset color",   -1.0f, 1.0f, &skySettings->sunsetTopSkyColorStartEndBlend.x);
	FloatSlider2("8",  "Blend Top Night",  "when the top sky color will start blending from sunset to night color", -1.0f, 1.0f, &skySettings->nightTopSkyColorStartEndBlend.x);
	FloatSlider2("9",  "Blend Mid Sunset", "when the Mid sky color will start blending from day to sunset color",   -1.0f, 1.0f, &skySettings->sunsetMidSkyColorStartEndBlend.x);
	FloatSlider2("10", "Blend Mid Night",  "when the Mid sky color will start blending from sunset to night color", -1.0f, 1.0f, &skySettings->nightMidSkyColorStartEndBlend.x);
	ImGui::PopItemWidth();

	// lightning settings
	ImGui::Spacing(10);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "LIGHTNING SETTINGS");
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.05f);
	FloatSlider("17",  "Switch to moonLight threshold", "at this sun height we will change to cast light from the moon", -1.0f, 0.0f, &skySettings->switchToMoonLightThreshold);

	ImGui::Spacing(3);
	FloatSlider2("23", "Blend Light Sunset", "when the dir light color will start to blend to sunset light color", -1.0f, 1.0f, &skySettings->sunsetLightColorStartEndBlend.x);
	FloatSlider2("24", "Blend Light Night",  "when the dir light color will start to blend to night light color",  -1.0f, 1.0f, &skySettings->nightLightColorStartEndBlend.x);
	FloatSlider2("18", "Day Light Fade",     "when the day light will start and finish to fadeout",                -1.0f, 1.0f, &skySettings->dayLightStartEndfade.x);
	FloatSlider2("19", "Night Light Fade",   "when the night light will start and finish to fadeout",              -1.0f, 1.0f, &skySettings->nightLightStartEndfade.x);

	ImGui::Spacing(3);
	ColorPickerNoAlpha("20", "day dir light Color",    "the color of the directional light at day",    "pick10", &skySettings->normalDirLightColor);
	ColorPickerNoAlpha("21", "sunset dir light Color", "the color of the directional light at sunset", "pick11", &skySettings->sunsetDirLightColor);
	ColorPickerNoAlpha("22", "night dir light Color",  "the color of the directional light at night",  "pick12", &skySettings->nightDirLightColor);
	ImGui::PopItemWidth();

	ImGui::Spacing(3);
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.08f);
	FloatInput3("25", "Start Rotation", "the rotation of the sun when application starts", &skySettings->startRotation.x);
	FloatInput3("26", "End Rotation", "the rotation of the sun when one cycle is passed, the x rotation should always be at 360", &skySettings->endRotation.x);
	float shadowDst = skySettings->shadowMapDistance.x;
	FloatInput("27", "ShadowMap distance", "the distance from where the shadow camera is rendering", &shadowDst);
	skySettings->shadowMapDistance = XMFLOAT3(shadowDst, shadowDst, shadowDst);

	if (ImGui::Button("Back"))
		GoToMain();

	// end rendering of this window
	ImGui::End();
}

void SkyDomeWindow::ShowToolTip(const char* tip)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::SetTooltip(tip);
		ImGui::EndTooltip();
	}
}

void SkyDomeWindow::ColorPickerNoAlpha(const char* ID, const char* header, const char* pickerName, const char* toolTip, XMFLOAT4* data)
{
	if (ImGui::ColorButton(ID, ImVec4(data->x, data->y, data->z, 0), ImGuiColorEditFlags_NoAlpha))
		ImGui::OpenPopup(pickerName);

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1.0), header);
	ShowToolTip(toolTip);

	if (ImGui::BeginPopup(pickerName))
	{
		ImGui::ColorPicker4(pickerName, (float*)data, ImGuiColorEditFlags_NoAlpha);
		ImGui::EndPopup();
	}
}

void SkyDomeWindow::FloatSlider(const char* ID, const char* header, const char* toolTip, float min, float max, float* data)
{
	ImGui::SliderFloat(ID, data, min, max, "%.2f");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), header);
	ShowToolTip(toolTip);
}

void SkyDomeWindow::FloatSlider2(const char* ID, const char* header, const char* toolTip, float min, float max, float* data)
{
	ImGui::SliderFloat2(ID, data, min, max, "%.2f");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), header);
	ShowToolTip(toolTip);
}

void SkyDomeWindow::FloatInput3(const char* ID, const char* header, const char* toolTip, float* data)
{
	ImGui::InputFloat3(ID, data, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), header);
	ShowToolTip(toolTip);
}

void SkyDomeWindow::FloatInput(const char* ID, const char* header, const char* toolTip, float* data)
{
	ImGui::InputFloat(ID, data, 1.0f, 1.0f, 1);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), header);
	ShowToolTip(toolTip);
}