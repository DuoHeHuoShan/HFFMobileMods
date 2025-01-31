#pragma once
#include <vector>
#include <map>
#include <concepts>
#include <imgui.h>
#include <imgui_manager.hpp>
#include <string_view>
#include <mini/ini.h>

extern mINI::INIStructure settingsIni;

enum class SettingCategory {
    Control,
    Video,
    Extra,
    Other
};

std::string CategoryToString(SettingCategory category);

std::string TranslateCategory(SettingCategory category);

class Setting {
public:
    SettingCategory category;
    virtual void OnLoaded() {}
    virtual void OnImGuiLoaded() {}
    virtual void OnGUI() {}
    virtual void OnGUIExtra() {}
    virtual void OnUpdate() {}
    virtual void OnAwake() {}
};

class SettingsManager {
private:
    std::map<SettingCategory, std::vector<Setting *>> settings {
        {SettingCategory::Control, {}},
        {SettingCategory::Video, {}},
        {SettingCategory::Extra, {}},
        {SettingCategory::Other, {}}
    };
public:
    static SettingsManager *instance;
    static void Init();
    void OnGUI();
    void OnAwake() {
        for(auto &category : settings) {
            for(auto &setting : category.second) {
                setting->OnAwake();
            }
        }
    }
    void OnUpdate();

    void AddSetting(Setting *setting);
    template<typename T>
    void AddSetting();
};

template<typename I, typename T> requires std::same_as<T, int> || std::same_as<T, float>
class SettingSlider : public Setting {
private:
    T _value;
    T _min, _max;
    std::string _key;
    const char *_label, *_format;
protected:
    SettingSlider(SettingCategory settingCategory, T min, T max, T defaultValue, std::string key, const char *label, const char *format) : _min(min), _max(max), _key(key), _label(label), _format(format) {
        instance = (I *) this;
        category = settingCategory;
        std::string categoryName = CategoryToString(settingCategory);
        if(!settingsIni[categoryName].has(key)) {
            settingsIni[categoryName][key] = std::to_string(defaultValue);
        }
        if constexpr (std::is_same_v<T, int>) {
            _value = std::stoi(settingsIni[categoryName][key]);
        } else if constexpr (std::is_same_v<T, float>) {
            _value = std::stof(settingsIni[categoryName][key]);
        }
    }
    virtual void OnValueChanged(T value) {};
    virtual void OnLoaded(T value) {};
public:
    static I *instance;
    void OnLoaded();
    void OnGUI();
    inline T GetValue() {
        return _value;
    }
};

template<typename I, typename T> requires std::same_as<T, int> || std::same_as<T, float>
I *SettingSlider<I, T>::instance;

template<typename I>
class SettingCheckbox : public Setting {
private:
    bool _value;
    std::string _key;
    const char *_label;
protected:
    SettingCheckbox(SettingCategory settingCategory, std::string key, bool defaultValue, const char *label) : _key(key), _label(label) {
        instance = (I *) this;
        category = settingCategory;
        std::string categoryName = CategoryToString(settingCategory);
        if(!settingsIni[categoryName].has(key)) {
            settingsIni[categoryName][key] = std::to_string(defaultValue);
        }
        _value = std::stoi(settingsIni[categoryName][key]);
    }
    virtual void OnLoaded(bool value) {};
    virtual void OnValueChanged(bool value) {};
public:
    static I *instance;
    void OnLoaded();
    void OnGUI();
    inline bool GetValue() {
        return _value;
    }
};

template<typename I>
I *SettingCheckbox<I>::instance;

template<typename I, typename T> requires std::is_enum_v<T>
class SettingComboBox : public Setting {
private:
    T _value;
    std::string _key;
    const char *_label;
    const char **_items;
    std::size_t _items_count;
protected:
    template<std::size_t N>
    SettingComboBox(SettingCategory settingCategory, std::string key, T defaultValue, const char *label, std::array<const char *, N> *items) : _key(key), _label(label) {
        instance = (I *) this;
        category = settingCategory;
        _items = items->data();
        _items_count = N;
        std::string categoryName = CategoryToString(settingCategory);
        if(!settingsIni[categoryName].has(key)) {
            settingsIni[categoryName][key] = std::to_string(int(defaultValue));
        }
        _value = (T) std::stoi(settingsIni[categoryName][key]);
    }
    virtual void OnLoaded(T value) {};
    virtual void OnValueChanged(T value) {};
public:
    static I *instance;
    inline T GetValue() {
        return _value;
    }
    void OnLoaded();
    void OnGUI();
};

template<typename I, typename T> requires std::is_enum_v<T>
I *SettingComboBox<I, T>::instance;