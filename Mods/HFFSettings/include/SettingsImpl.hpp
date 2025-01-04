#pragma once

#include "Settings.hpp"
#include <Classes.hpp>

class LookHScaleSetting : public SettingSlider<LookHScaleSetting, int> {
public:
    LookHScaleSetting() : SettingSlider(SettingCategory::Control, 0, 100, 12, "lookHScale", "水平灵敏度", "%d%%") {}
};

class LookVScaleSetting : public SettingSlider<LookVScaleSetting, int> {
public:
    LookVScaleSetting() : SettingSlider(SettingCategory::Control, 0, 100, 5, "lookVScale", "垂直灵敏度", "%d%%") {}
};

class BiggerButtonSetting : public SettingCheckbox<BiggerButtonSetting> {
public:
    BiggerButtonSetting() : SettingCheckbox(SettingCategory::Control, "biggerButton", false, "大按键 (需要重启)") {}
    void OnLoaded(bool value);
};

class DontJumpOnQuickSwipeSetting : public SettingCheckbox<DontJumpOnQuickSwipeSetting> {
public:
    DontJumpOnQuickSwipeSetting() : SettingCheckbox(SettingCategory::Control, "dontJumpOnQuickSwipe", false, "禁止快速滑动摇杆跳跃") {}
    void OnLoaded(bool value);
};

class DisableShadowSetting : public SettingCheckbox<DisableShadowSetting> {
private:
    float originShadowDistance;
public:
    DisableShadowSetting() : SettingCheckbox(SettingCategory::Video, "disableShadows", false, "禁用阴影") {}
    void OnAwake() {
        originShadowDistance = UnityEngine::QualitySettings::shadowDistance;
        if(GetValue()) UnityEngine::QualitySettings::shadowDistance = 0;
    }
    void OnValueChanged(bool value) {
        if(value) {
            UnityEngine::QualitySettings::shadowDistance = 0;
        } else {
            UnityEngine::QualitySettings::shadowDistance = originShadowDistance;
        }
    }
};

class CameraFOVSetting : public SettingSlider<CameraFOVSetting, int> {
public:
    CameraFOVSetting() : SettingSlider(SettingCategory::Video, 0, 20, 5, "cameraFov", "视野", "%d") {}
    void OnAwake() {
        Options::set_cameraFov(GetValue());
    }
    void OnValueChanged(int value) {
        Options::set_cameraFov(value);
    }
};

class CameraSmoothingSetting : public SettingSlider<CameraSmoothingSetting, int> {
public:
    CameraSmoothingSetting() : SettingSlider(SettingCategory::Video, 0, 20, 10, "cameraSmoothing", "平滑", "%d") {}
    void OnAwake() {
        Options::set_cameraSmoothing(GetValue());
    }
    void OnValueChanged(int value) {
        Options::set_cameraSmoothing(value);
    }
};

class CloudSetting : public SettingSlider<CloudSetting, int> {
public:
    CloudSetting() : SettingSlider(SettingCategory::Video, 0, 4, 2, "advancedVideoClouds", "云层质量", "%d") {}
    void OnAwake() {
        Options::set_advancedVideoClouds(GetValue());
    }
    void OnValueChanged(int value) {
        Options::set_advancedVideoClouds(value);
        Options::ApplyAdvancedVideo();
    }
};

class TargetFrameRateSetting : public SettingSlider<TargetFrameRateSetting, int> {
public:
    TargetFrameRateSetting() : SettingSlider(SettingCategory::Video, 0, 120, 60, "targetFrameRate", "帧率限制", "%d") {}
    void OnAwake() {
        UnityEngine::Application::targetFrameRate = GetValue();
    }
    void OnValueChanged(int value) {
        UnityEngine::Application::targetFrameRate = value;
    }
};

class VisibleBallSetting : public SettingCheckbox<VisibleBallSetting> {
public:
    BNM::UnityEngine::Object *ballObject;
    VisibleBallSetting() : SettingCheckbox(SettingCategory::Extra, "visibleBall", false, "显示球") {}
    void OnLoaded(bool value);
    void OnValueChanged(bool value) {
        if(!ballObject->Alive()) return;
        UnityEngine::GameObject::SetActive[ballObject](value);
    }
};

class PseudoPCSetting : public SettingCheckbox<PseudoPCSetting> {
public:
    BNM::UnityEngine::Object *touchStick;
    PseudoPCSetting() : SettingCheckbox(SettingCategory::Extra, "pseudoPC", false, "伪端游") {}
    void OnLoaded(bool value);
    void OnValueChanged(bool value);
};

class LocalSaveSetting : public SettingCheckbox<LocalSaveSetting> {
private:
    static void ApplyLocalSave();
public:
    LocalSaveSetting() : SettingCheckbox(SettingCategory::Other, "localSave", true, "本地存档") {}
    void OnLoaded(bool value) {
        if(value) {
            ApplyLocalSave();
        }
    }
};

class FPSDisplaySetting : public SettingCheckbox<FPSDisplaySetting> {
public:
    FPSDisplaySetting() : SettingCheckbox(SettingCategory::Other, "fpsDisplay", false, "帧数显示") {}
    void OnGUIExtra() {
        if(GetValue()) {
            const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize;
            ImGui::Begin("FPSDisplayText", nullptr, windowFlags);
            ImGui::Text("FPS: %.2f", 1 / UnityEngine::Time::unscaledDeltaTime.Get());
            ImGui::End();
        }
    }
};

enum class UITheme {
    Classic,
    Dark,
    Light
};
class UIThemeSetting : public SettingComboBox<UIThemeSetting, UITheme> {
private:
    void ApplyTheme(UITheme theme) {
        switch (theme) {
            case UITheme::Classic:
                ImGui::StyleColorsClassic();
                break;
            case UITheme::Dark:
                ImGui::StyleColorsDark();
                break;
            case UITheme::Light:
                ImGui::StyleColorsLight();
                break;
        }
    }
public:
    UIThemeSetting() : SettingComboBox(SettingCategory::Other, "uiTheme", UITheme::Dark, "UI主题", new std::array {"经典", "暗黑", "亮色"}) {}
    void OnImGuiLoaded() {
        ApplyTheme(GetValue());
    }
    void OnValueChanged(UITheme value) {
        ApplyTheme(value);
    }
};

class SharedSetting : public Setting {
public:
    SharedSetting() {
        category = SettingCategory::Other;
    }
    void OnLoaded();
};