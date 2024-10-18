#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Method.hpp"
#include "BNM/Utils.hpp"
#include "BNM/Field.hpp"
#include "BNM/UnityStructures.hpp"
#include "Classes.hpp"
#include <map>
#include <string>
#include <fstream>

std::map<std::string, std::string> HFFSettings;

float lookHScale = 12;
float lookVScale = 5;
bool visibleBall = false;
bool localSave = false;
bool disableShadows = false;
bool joystickFix = false;
int cameraFov = 5;
int cameraSmoothing = 10;
int advancedVideoClouds = 2;

void (*old_ReadInput)(BNM::UnityEngine::Object *, void *);
void new_ReadInput(BNM::UnityEngine::Object *thiz, void *outInputState) {
    HumanControls::lookHScale[thiz].Set(lookHScale);
    HumanControls::lookVScale[thiz].Set(lookVScale);
    old_ReadInput(thiz, outInputState);
}

void (*old_Ball_OnEnable)(BNM::UnityEngine::Object *);
void new_Ball_OnEnable(BNM::UnityEngine::Object *thiz) {
    old_Ball_OnEnable(thiz);
    if(visibleBall) UnityEngine::GameObject::SetActive[UnityEngine::GameObject::Find(BNM::CreateMonoString("/Player(Clone)/Ball/Sphere"))](true);
}

BNM::Structures::Unity::Vector3 (*old_HumanControls_get_calc_joyWalk)(BNM::UnityEngine::Object *);
BNM::Structures::Unity::Vector3 new_HumanControls_get_calc_joyWalk(BNM::UnityEngine::Object *thiz) {
    BNM::Structures::Unity::Vector3 ret = old_HumanControls_get_calc_joyWalk(thiz);
    ret.x = round(ret.x);
    ret.z = round(ret.z);
    return ret;
}

void (*old_Options_Load)();
void new_Options_Load() {
    Options::set_cameraFov(cameraFov);
    Options::set_cameraSmoothing(cameraSmoothing);
    Options::set_advancedVideoClouds(advancedVideoClouds);
    if(disableShadows) UnityEngine::QualitySettings::shadowDistance = 0;
    old_Options_Load();
}


void ApplyLocalSave() {
    using namespace BNM;

    HOOK(Mobile::SaveGameSystem::PlayerPrefs::GetInt, ::UnityEngine::PlayerPrefs::GetInt.GetInfo()->methodPointer, nullptr);
    HOOK(Mobile::SaveGameSystem::PlayerPrefs::SetInt, ::UnityEngine::PlayerPrefs::SetInt.GetInfo()->methodPointer, nullptr);
    HOOK(Mobile::SaveGameSystem::PlayerPrefs::GetFloat, ::UnityEngine::PlayerPrefs::GetFloat.GetInfo()->methodPointer, nullptr);
    HOOK(Mobile::SaveGameSystem::PlayerPrefs::SetFloat, ::UnityEngine::PlayerPrefs::SetFloat.GetInfo()->methodPointer, nullptr);
    HOOK(Mobile::SaveGameSystem::PlayerPrefs::GetString, ::UnityEngine::PlayerPrefs::GetString.GetInfo()->methodPointer, nullptr);
    HOOK(Mobile::SaveGameSystem::PlayerPrefs::SetString, ::UnityEngine::PlayerPrefs::SetString.GetInfo()->methodPointer, nullptr);
}

void OnLoaded() {
    using namespace BNM;

    if(localSave) ApplyLocalSave();
    InvokeHook(Ball::OnEnable, new_Ball_OnEnable, old_Ball_OnEnable);
    HOOK(HumanControls::ReadInput, new_ReadInput, old_ReadInput);
    if(joystickFix) HOOK(HumanControls::get_calc_joyWalk, new_HumanControls_get_calc_joyWalk, old_HumanControls_get_calc_joyWalk);
    HOOK(Options::Load, new_Options_Load, old_Options_Load);
}

bool stob(const std::string &str) {
    if(str == "true") return true;
    return false;
}

std::string GetWorkDir() {
    Dl_info dlInfo;
    if(dladdr((void *) &GetWorkDir, &dlInfo) > 0) {
        std::string soDir = dlInfo.dli_fname;
        return soDir.substr(0, soDir.find_last_of('/'));
    }
    return "";
}

void UseDefaultSettings() {
    HFFSettings["lookHScale"] = "12";
    HFFSettings["lookVScale"] = "5";
    HFFSettings["visibleBall"] = "false";
    HFFSettings["localSave"] = "false";
    HFFSettings["disableShadows"] = "false";
    HFFSettings["joystickFix"] = "false";
    HFFSettings["cameraFov"] = "5";
    HFFSettings["cameraSmoothing"] = "10";
    HFFSettings["advancedVideoClouds"] = "2";
}

void ReadSettings() {
    UseDefaultSettings();
    std::string workDir = GetWorkDir();
    std::fstream settingsFile;
    settingsFile.open(workDir + "/HFFSettings.txt", std::ios_base::in);
    if(!settingsFile) {
        BNM_LOG_ERR("%s", "Could not open settings file!");
        return;
    }
    std::string line;
    while(std::getline(settingsFile, line)) {
        if(line.empty()) continue;
        auto mid = line.find_first_of('=');
        std::string key = line.substr(0, mid);
        std::string value = line.substr(mid + 1, line.length() - mid - 1);
        HFFSettings[key] = value;
    }
    settingsFile.close();
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    // Load BNM by finding the path to libil2cpp.so
    BNM::Loading::TryLoadByJNI(env);

    // Or load using KittyMemory (as an example)
    // Example_07();

    BNM::Loading::AddOnLoadedEvent(BNMU_OnLoaded);
    BNM::Loading::AddOnLoadedEvent(OnLoaded);

    ReadSettings();
    lookHScale = std::stof(HFFSettings["lookHScale"]);
    lookVScale = std::stof(HFFSettings["lookVScale"]);
    visibleBall = stob(HFFSettings["visibleBall"]);
    localSave = stob(HFFSettings["localSave"]);
    disableShadows = stob(HFFSettings["disableShadows"]);
    joystickFix = stob(HFFSettings["joystickFix"]);
    cameraFov = std::stoi(HFFSettings["cameraFov"]);
    cameraSmoothing = std::stoi(HFFSettings["cameraSmoothing"]);
    advancedVideoClouds = std::stoi(HFFSettings["advancedVideoClouds"]);

    return JNI_VERSION_1_6;
}