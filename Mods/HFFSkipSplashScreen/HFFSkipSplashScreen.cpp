#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Method.hpp"
#include "BNM/Utils.hpp"

BNM::Class StartupExperienceController;
BNM::MethodBase StartupExperienceController_PlayStartupExperience;
BNM::Method<void> StartupExperienceController_SkipStartupExperience;

void (*old_StartupExperienceController_PlayStartupExperience)(BNM::UnityEngine::Object *);
void new_StartupExperienceController_PlayStartupExperience(BNM::UnityEngine::Object *instance) {
    StartupExperienceController_SkipStartupExperience[instance](nullptr);
}

void OnLoaded() {
    using namespace BNM;
    StartupExperienceController = Class("", "StartupExperienceController");
    StartupExperienceController_PlayStartupExperience = StartupExperienceController.GetMethod("PlayStartupExperience");
    StartupExperienceController_SkipStartupExperience = StartupExperienceController.GetMethod("SkipStartupExperience");
    HOOK(StartupExperienceController_PlayStartupExperience, new_StartupExperienceController_PlayStartupExperience, old_StartupExperienceController_PlayStartupExperience);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);


    // Load BNM by finding the path to libil2cpp.so
    BNM::Loading::TryLoadByJNI(env);

    // Or load using KittyMemory (as an example)
    // Example_07();

    BNM::Loading::AddOnLoadedEvent(OnLoaded);

    return JNI_VERSION_1_6;
}