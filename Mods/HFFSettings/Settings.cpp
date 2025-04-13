#include <Settings.hpp>
#include <SettingsImpl.hpp>
#include <mini/ini.h>
#include <Classes.hpp>

bool settingsWindowOpened = false;
extern void WriteSettings();
bool ShouldToggleMenu();

std::string CategoryToString(SettingCategory category) {
    switch (category) {
        case SettingCategory::Control: return "Control";
        case SettingCategory::Video: return "Video";
        case SettingCategory::Extra: return "Extra";
        case SettingCategory::Other: return "Other";
    }
}

std::string TranslateCategory(SettingCategory category) {
    switch (category) {
        case SettingCategory::Control: return "控制";
        case SettingCategory::Video: return "视频";
        case SettingCategory::Extra: return "额外";
        case SettingCategory::Other: return "其它";
    }
}

SettingsManager *SettingsManager::instance;

void SettingsManager::Init() {
    instance = new SettingsManager;

    // Control
    instance->AddSetting<LookHScaleSetting>();
    instance->AddSetting<LookVScaleSetting>();
    instance->AddSetting<BiggerButtonSetting>();
    instance->AddSetting<DontJumpOnQuickSwipeSetting>();
    // Video
    instance->AddSetting<DisableShadowSetting>();
    instance->AddSetting<CameraFOVSetting>();
    instance->AddSetting<CameraSmoothingSetting>();
    instance->AddSetting<CloudSetting>();
    instance->AddSetting<TargetFrameRateSetting>();
    // Extra
    instance->AddSetting<VisibleBallSetting>();
    instance->AddSetting<PseudoPCSetting>();
    instance->AddSetting<DisplayLevelPassTriggersSetting>();
    instance->AddSetting<DisplayFallTriggersSetting>();
    instance->AddSetting<DisplayCheckpointsSetting>();
    instance->AddSetting<FreeCameraSetting>();
    instance->AddSetting<TimePauseSetting>();
    // Other
    instance->AddSetting<LocalSaveSetting>();
    instance->AddSetting<FPSDisplaySetting>();
    instance->AddSetting<UIThemeSetting>();
    instance->AddSetting<UIWindowRoundingSetting>();

    instance->AddSetting<SharedSetting>();

    ImGuiManager::AddOnLoadedCallback([]() {
        for(auto &category : instance->settings) {
            for(auto &setting : category.second) {
                setting->OnImGuiLoaded();
            }
        }
    });

    for(auto &category : instance->settings) {
        for(auto &setting : category.second) {
            setting->OnLoaded();
        }
    }

    WriteSettings();
}

void SettingsManager::OnUpdate() {
    if(ShouldToggleMenu()) settingsWindowOpened = !settingsWindowOpened;
    bool isCheated = false;
    for(auto &category : settings) {
        for(auto &setting : category.second) {
            setting->OnUpdate();
            isCheated = isCheated || setting->CheatCheck();
        }
    }
    SharedData::SetData<bool>("HFFSettings::isCheated", isCheated);
}

void SettingsManager::OnGUI() {
    ImGuiIO &io = ImGui::GetIO();
    for(auto &category : settings) {
        for(auto &setting : category.second) {
            setting->OnGUIExtra();
        }
    }
    if(!settingsWindowOpened) return;
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    if(ImGui::Begin("HFF手游设置插件v0.0.5")) {
        if(ImGui::BeginTabBar("SettingsTabBar")) {
            for(auto &category : settings) {
                if(ImGui::BeginTabItem(TranslateCategory(category.first).c_str())) {
                    for(auto &setting : category.second) {
                        setting->OnGUI();
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

void SettingsManager::AddSetting(Setting *setting) {
    settings[setting->category].push_back(setting);
}

template<typename T>
void SettingsManager::AddSetting() {
    AddSetting(new T);
}

template<typename I, typename T> requires std::same_as<T, int> || std::same_as<T, float>
void SettingSlider<I, T>::OnLoaded() {
    OnLoaded(_value);
}

template<typename I, typename T> requires std::same_as<T, int> || std::same_as<T, float>
void SettingSlider<I, T>::OnGUI() {
    if constexpr (std::is_same_v<T, int>) {
        if(ImGui::SliderInt(_label, (int *) &_value, _min, _max, _format)) {
            if(BNM::AttachIl2Cpp()) {
                OnValueChanged(_value);
                BNM::DetachIl2Cpp();
            }
            settingsIni[CategoryToString(category)][_key] = std::to_string(_value);
            WriteSettings();
        }
    } else if constexpr (std::is_same_v<T, float>) {
        if(ImGui::SliderFloat(_label, (float *) &_value, _min, _max, _format)) {
            if(BNM::AttachIl2Cpp()) {
                OnValueChanged(_value);
                BNM::DetachIl2Cpp();
            }
            settingsIni[CategoryToString(category)][_key] = std::to_string(_value);
            WriteSettings();
        }
    }
}

template<typename I>
void SettingCheckbox<I>::OnLoaded() {
    OnLoaded(_value);
}

template<typename I>
void SettingCheckbox<I>::OnGUI() {
    if(ImGui::Checkbox(_label, &_value)) {
        if(BNM::AttachIl2Cpp()) {
            OnValueChanged(_value);
            BNM::DetachIl2Cpp();
        }
        settingsIni[CategoryToString(category)][_key] = std::to_string(_value);
        WriteSettings();
    }
}

template<typename I, typename T> requires std::is_enum_v<T>
void SettingComboBox<I, T>::OnLoaded() {
    OnLoaded(_value);
}

template<typename I, typename T> requires std::is_enum_v<T>
void SettingComboBox<I, T>::OnGUI() {
    if(ImGui::Combo(_label, (int *) &_value, _items, _items_count)) {
        if(BNM::AttachIl2Cpp()) {
            OnValueChanged(_value);
            BNM::DetachIl2Cpp();
        }
        settingsIni[CategoryToString(category)][_key] = std::to_string(int(_value));
        WriteSettings();
    }
}

bool ShouldToggleMenu() {
    using namespace UnityEngine;
    if(Input::touchCount == 3)
    {
        for(int i = 0; i < 3; i++)
        {
            Touch touch = Input::GetTouch(i);
            if(touch.phase != TouchPhase::Began || touch.position.x > float(Screen::width) / 2.0f) return false;
        }
        return true;
    }
    return false;
}