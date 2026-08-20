#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Method.hpp"
#include "BNM/Utils.hpp"
#include "BNM/Field.hpp"
#include "BNM/ClassesManagement.hpp"
#include "Classes.hpp"
#include "imgui_manager.hpp"

bool isMenuOpen = false;

void OnGUI();

struct HFFRecorder : public BNM::UnityEngine::MonoBehaviour {
    BNM_CustomClass(HFFRecorder, BNM::CompileTimeClassBuilder("", "HFFRecorder").Build(), BNM::CompileTimeClassBuilder("UnityEngine", "MonoBehaviour", "UnityEngine.CoreModule").Build(), {});
    void Constructor() {
        BNM::UnityEngine::MonoBehaviour tmp = *this;
        *this = HFFRecorder();
        *((BNM::UnityEngine::MonoBehaviour *)this) = tmp;

        ImGuiManager::AddOnGuiCallback(OnGUI);
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
    void Update() {
        if(ShouldToggleMenu()) {
            isMenuOpen = !isMenuOpen;
        }
    }

    BNM_CustomMethod(Update, false, BNM::Defaults::Get<void>(), "Update");
    BNM_CustomMethod(Constructor, false, BNM::Defaults::Get<void>(), ".ctor");
};

void OnGUI() {
    if (!isMenuOpen) return;
    static bool p_close = true;
    if (!p_close) return;
    ImGui::SetNextWindowSize(ImVec2(450, 300), ImGuiCond_Once);
    if(ImGui::Begin("HFF录制插件v0.0.7", &p_close)) {
        ImGui::End();
    }
}

void (*_HFFResources$Awake)(BNM::UnityEngine::Object *);
void HFFResources$Awake(BNM::UnityEngine::Object *thiz) {
    using namespace UnityEngine;
    _HFFResources$Awake(thiz);
    UnityEngine::GameObject::AddComponent[UnityEngine::Component::gameObject[thiz]](BNM::Class(HFFRecorder::BNMCustomClass.myClass).GetMonoType());
}

void OnLoaded() {
    using namespace BNM;
    InvokeHook(HFFResources::Awake, HFFResources$Awake, _HFFResources$Awake);
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