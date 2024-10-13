#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Method.hpp"
#include "BNM/Utils.hpp"
#include "BNM/Field.hpp"

BNM::Class PlayMenu;
BNM::MethodBase PlayMenu_OnGotFocus;
BNM::Field<BNM::UnityEngine::Object *> PlayMenu_selectLevelButton;
BNM::Method<BNM::UnityEngine::Object *> GameObject_SetActive;
BNM::Method<BNM::UnityEngine::Object *> Component_get_gameObject;

void (*old_PlayMenu_OnGotFocus)(BNM::UnityEngine::Object *);
void new_PlayMenu_OnGotFocus(BNM::UnityEngine::Object *instance)
{
    old_PlayMenu_OnGotFocus(instance);
    GameObject_SetActive[Component_get_gameObject[PlayMenu_selectLevelButton[instance]]()](true);
}

void OnLoaded() {
    using namespace BNM;
    PlayMenu = Class("", "PlayMenu");
    Component_get_gameObject = Class("UnityEngine", "Component").GetMethod("get_gameObject");
    GameObject_SetActive = Class("UnityEngine", "GameObject").GetMethod("SetActive");
    PlayMenu_OnGotFocus = PlayMenu.GetMethod("OnGotFocus");
    PlayMenu_selectLevelButton = PlayMenu.GetField("selectLevelButton");
    VirtualHook(PlayMenu, PlayMenu_OnGotFocus, new_PlayMenu_OnGotFocus, old_PlayMenu_OnGotFocus);
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