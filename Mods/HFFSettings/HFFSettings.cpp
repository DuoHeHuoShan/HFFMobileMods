#include <jni.h>
#include <imgui_manager.hpp>
#include <imgui.h>
#include <BNM/Loading.hpp>
#include <BNM/Class.hpp>
#include <BNM/MethodBase.hpp>
#include <BNM/Method.hpp>
#include <BNM/Utils.hpp>
#include <BNM/Field.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/ClassesManagement.hpp>
#include <Classes.hpp>
#include <Settings.hpp>
#include <mINI/ini.h>
#include <string>

using namespace mINI;

INIFile *settingsFile;
INIStructure settingsIni;

struct HFFSettings : public BNM::UnityEngine::MonoBehaviour {
    BNM_CustomClass(HFFSettings, BNM::CompileTimeClassBuilder("", "HFFSettings").Build(),
            BNM::CompileTimeClassBuilder("UnityEngine", "MonoBehaviour", "UnityEngine.CoreModule").Build(),
    {});
    void Awake() {
        BNM_CallCustomMethodOrigin(Awake, this);
        SettingsManager::instance->OnAwake();
    }
    void Update() {
        BNM_CallCustomMethodOrigin(Update, this);
        SettingsManager::instance->OnUpdate();
    }
    BNM_CustomMethod(Awake, false, BNM::Defaults::Get<void>(), "Awake");
    BNM_CustomMethodSkipTypeMatch(Awake);
    BNM_CustomMethodMarkAsInvokeHook(Awake);
    BNM_CustomMethod(Update, false, BNM::Defaults::Get<void>(), "Update");
    BNM_CustomMethodSkipTypeMatch(Update);
    BNM_CustomMethodMarkAsInvokeHook(Update);
};

void (*_HFFResources$Awake)(BNM::UnityEngine::Object *);
void HFFResources$Awake(BNM::UnityEngine::Object *thiz) {
    using namespace UnityEngine;
    _HFFResources$Awake(thiz);
    UnityEngine::GameObject::AddComponent[UnityEngine::Component::gameObject[thiz]](BNM::Class(HFFSettings::BNMCustomClass.myClass).GetMonoType());
}

void OnLoaded() {
    using namespace BNM;

    InvokeHook(HFFResources::Awake, HFFResources$Awake, _HFFResources$Awake);
    SettingsManager::Init();
    ImGuiManager::AddOnGuiCallback(std::bind(&SettingsManager::OnGUI, SettingsManager::instance));
}

std::string GetWorkDir() {
    Dl_info dlInfo;
    if(dladdr((void *) &GetWorkDir, &dlInfo) > 0) {
        std::string soDir = dlInfo.dli_fname;
        return soDir.substr(0, soDir.find_last_of('/'));
    }
    return "";
}

void WriteSettings() {
    if(!settingsFile)
        settingsFile = new INIFile(GetWorkDir() + "/HFFSettings.ini");
    settingsFile->write(settingsIni, true);
}

void ReadSettings() {
    if(!settingsFile)
        settingsFile = new INIFile(GetWorkDir() + "/HFFSettings.ini");
    settingsFile->read(settingsIni);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    // Load BNM by finding the path to libil2cpp.so
    ImGuiManager::TryInitImGui();
    BNM::Loading::TryLoadByJNI(env);

    // Or load using KittyMemory (as an example)
    // Example_07();

    BNM::Loading::AddOnLoadedEvent(BNMU_OnLoaded);
    BNM::Loading::AddOnLoadedEvent(OnLoaded);

    ReadSettings();

    return JNI_VERSION_1_6;
}