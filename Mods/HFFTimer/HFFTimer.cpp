#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Method.hpp"
#include "BNM/Utils.hpp"
#include "BNM/ClassesManagement.hpp"
#include "BNM/UnityStructures.hpp"
#include "Classes.hpp"
#include <BNMGUI.hpp>

struct HFFTimer : BNM::UnityEngine::MonoBehaviour {
    BNM_CustomClass(HFFTimer, BNM::CompileTimeClassBuilder(nullptr, "HFFTimer").Build(),
                    BNM::CompileTimeClassBuilder("UnityEngine", "MonoBehaviour", "UnityEngine.CoreModule").Build(),
                    {});
    void OnGUI() {
        using namespace BNMGUI;
        using namespace BNM::Structures::Unity;
        BNM_CallCustomMethodOrigin(OnGUI, this);
        GUI::Label(Rect(0, 0, 20000, 20000), "OnGUI() called!");
    }
    BNM_CustomMethod(OnGUI, false, BNM::Defaults::Get<void>(), "OnGUI");
    BNM_CustomMethodSkipTypeMatch(OnGUI);
    BNM_CustomMethodMarkAsInvokeHook(OnGUI);
};

void (*_HFFResources$Awake)(BNM::UnityEngine::Object *);
void HFFResources$Awake(BNM::UnityEngine::Object *thiz) {
    _HFFResources$Awake(thiz);
    UnityEngine::GameObject::AddComponent[UnityEngine::Component::gameObject[thiz]](BNM::Class(HFFTimer::BNMCustomClass.myClass).GetMonoType());
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

    BNM::Loading::AddOnLoadedEvent(BNMU_OnLoaded);
    BNM::Loading::AddOnLoadedEvent(OnLoaded);

    return JNI_VERSION_1_6;
}