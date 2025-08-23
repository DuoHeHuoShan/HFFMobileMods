#pragma once

#include "Settings.hpp"
#include <Classes.hpp>

std::vector<BNM::UnityEngine::Object *> CreateHitboxesInComponents(std::vector<void *> components, BNM::Structures::Unity::Color color);

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

class DisplayLevelPassTriggersSetting : public SettingCheckbox<DisplayLevelPassTriggersSetting> {
private:
    BNM::Class LevelPassTrigger;
    std::vector<BNM::UnityEngine::Object *> hitboxes;
    void Create() {
        using namespace UnityEngine;
        using namespace BNM::Structures;
        hitboxes = CreateHitboxesInComponents(Object::FindObjectsOfType(LevelPassTrigger.GetMonoType(), false)->ToVector(), Unity::Color(0, 1, 0, 0.33f));
    }
public:
    DisplayLevelPassTriggersSetting() : SettingCheckbox(SettingCategory::Extra, "displayLevelPassTrigger", false, "显示通关点") {}
    void OnLoaded() {
        LevelPassTrigger = BNM::Class("HumanAPI", "LevelPassTrigger");
    }
    void OnValueChanged(bool value) {
        if(value) {
            Create();
        } else {
            for(void *go : hitboxes) {
                UnityEngine::Object::Destroy(go);
            }
            hitboxes.clear();
        }
    }
    void OnLevelChanged(int level) {
        hitboxes.clear();
        if(!GetValue() || level == -1) return;
        Create();
    }
};

class DisplayFallTriggersSetting : public SettingCheckbox<DisplayFallTriggersSetting> {
private:
    BNM::Class FallTrigger;
    std::vector<BNM::UnityEngine::Object *> hitboxes;
    void Create() {
        using namespace UnityEngine;
        using namespace BNM::Structures;
        hitboxes = CreateHitboxesInComponents(Object::FindObjectsOfType(FallTrigger.GetMonoType(), false)->ToVector(), Unity::Color(1, 0, 0, 0.33f));
    }
public:
    DisplayFallTriggersSetting() : SettingCheckbox(SettingCategory::Extra, "displayFallTrigger", false, "显示死亡点") {}
    void OnLoaded() {
        FallTrigger = BNM::Class("HumanAPI", "FallTrigger");
    }
    void OnValueChanged(bool value) {
        if(value) {
            Create();
        } else {
            for(void *go : hitboxes) {
                UnityEngine::Object::Destroy(go);
            }
            hitboxes.clear();
        }
    }
    void OnLevelChanged(int level) {
        hitboxes.clear();
        if(!GetValue() || level == -1) return;
        Create();
    }
};

class DisplayCheckpointsSetting : public SettingCheckbox<DisplayCheckpointsSetting> {
private:
    BNM::Class Checkpoint;
    std::vector<BNM::UnityEngine::Object *> hitboxes;
    void Create() {
        using namespace UnityEngine;
        using namespace BNM::Structures;
        hitboxes = CreateHitboxesInComponents(Object::FindObjectsOfType(Checkpoint.GetMonoType(), false)->ToVector(), Unity::Color(1, 0.66f, 0, 0.33f));
    }
public:
    DisplayCheckpointsSetting() : SettingCheckbox(SettingCategory::Extra, "displayCheckpoint", false, "显示存档点") {}
    void OnLoaded() {
        Checkpoint = BNM::Class("HumanAPI", "Checkpoint");
    }
    void OnValueChanged(bool value) {
        if(value) {
            Create();
        } else {
            for(void *go : hitboxes) {
                UnityEngine::Object::Destroy(go);
            }
            hitboxes.clear();
        }
    }
    void OnLevelChanged(int level) {
        hitboxes.clear();
        if(!GetValue() || level == -1) return;
        Create();
    }
};

class FreeCameraSetting : public SettingButton<FreeCameraSetting> {
public:
    void *camTransform;
    FreeCameraSetting() : SettingButton(SettingCategory::Extra, "切换自由视角") {}
    void OnLoaded();
    void OnClick();
};

class TimePauseSetting : public SettingButton<TimePauseSetting> {
public:
    TimePauseSetting() : SettingButton(SettingCategory::Extra, "切换暂停状态") {}
    void OnClick();
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

class LagFixSetting : public SettingCheckbox<LagFixSetting> {
public:
    LagFixSetting() : SettingCheckbox(SettingCategory::Other, "lagFix", true, "卡顿修复") {}
    void OnLoaded();
};

class MPFixSetting : public SettingCheckbox<MPFixSetting> {
public:
    MPFixSetting() : SettingCheckbox(SettingCategory::Other, "mpFix", true, "联机修复 (需要重启)") {}
    void OnLoaded(bool value);
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

class UIWindowRoundingSetting : public SettingSlider<UIWindowRoundingSetting, float> {
public:
    UIWindowRoundingSetting() : SettingSlider(SettingCategory::Other, 0, 12, 0, "uiWindowRounding", "窗口圆角", "%.1f") {}
    void OnImGuiLoaded() {
        ImGuiManager::SetStyleVar(ImGuiStyleVar_WindowRounding, GetValue());
    }
    void OnValueChanged(float value) {
        ImGuiManager::SetStyleVar(ImGuiStyleVar_WindowRounding, value);
    }
};

class SharedSetting : public Setting {
public:
    SharedSetting() {
        category = SettingCategory::Other;
    }
    void OnLoaded();
    void OnUpdate();
};