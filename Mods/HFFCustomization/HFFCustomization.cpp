#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Method.hpp"
#include "BNM/Utils.hpp"
#include "Classes.hpp"

using namespace BNM::Structures;

void (*old_CustomizationEditMenu_OnGotFocus)(void *);
void new_CustomizationEditMenu_OnGotFocus(void *instance) {
    old_CustomizationEditMenu_OnGotFocus(instance);
    UnityEngine::GameObject::SetActive[UnityEngine::Component::gameObject[CustomizationEditMenu::paintButton[instance]]](true);
}

static sigjmp_buf env;

void sigsegv_handler(int signum, siginfo_t* info, void* context) {
    (void)info;
    (void)context;
    siglongjmp(env, 1);
}

bool IsAllocated(void* ptr) {
    struct sigaction old_act {}, new_act {};
    new_act.sa_sigaction = sigsegv_handler;
    new_act.sa_flags = SA_SIGINFO;

    // 设置信号处理器
    if (sigaction(SIGSEGV, &new_act, &old_act) == -1) {
        return false;
    }

    bool accessible = false;
    if (sigsetjmp(env, 1) == 0) {
        // 尝试读取内存
        volatile char c = *reinterpret_cast<volatile char*>(ptr);
        (void)c; // 防止编译器优化
        accessible = true;
    }

    // 恢复原信号处理
    sigaction(SIGSEGV, &old_act, nullptr);
    return accessible;
}

bool CheckObj(void *obj) {
    if (obj && IsAllocated(obj)) return true;
    return false;
}

void (*old_LoadFromPreset)(void *, void *);
void new_LoadFromPreset(void *instance, void *preset) {
    old_LoadFromPreset(instance, preset);
    if(!CheckObj(preset) || RagdollPresetMetadata::folder[preset].Get() == nullptr || RagdollPresetMetadata::folder[preset].Get() >= (void *) 32000000000000000ull) return;
    std::string partStr = WorkshopItemType2String(RagdollTexture::part[instance].Get());
    RagdollTexture::savePath[instance] = FileTools::Combine(RagdollPresetMetadata::folder[preset].Get(), BNM::CreateMonoString(
            partStr + ".png"));
    RagdollTexture::textureLoadSuppressed[instance] = RagdollPresetPartMetadata::suppressCustomTexture[RagdollPresetMetadata::GetPart[preset](RagdollTexture::part[instance].Get())].Get();
    if(!RagdollTexture::textureLoadSuppressed[instance].Get() && FileTools::TestExists(RagdollTexture::savePath[instance].Get())) {
        void *ragdollPresetPartMetadata = RagdollPresetMetadata::GetPart[preset](RagdollTexture::part[instance].Get());
        if(ragdollPresetPartMetadata != nullptr && RagdollPresetPartMetadata::bytes[ragdollPresetPartMetadata].Get() != nullptr) {
            RagdollTexture::ChangeBaseTexture[instance](FileTools::TextureFromBytes(BNM::CreateMonoString(partStr), RagdollPresetPartMetadata::bytes[ragdollPresetPartMetadata].Get()), false);
        } else if(!RagdollTexture::savePath[instance].Get()->IsNullOrEmpty()) {
            bool isAsset;
            void *newRes = FileTools::ReadTexture(RagdollTexture::savePath[instance].Get(), &isAsset);
            RagdollTexture::ChangeBaseTexture[instance](newRes, isAsset);
        }
        if(RagdollTexture::baseTexture[instance].Get()->Alive()) {
            UnityEngine::Texture2D::Compress[RagdollTexture::baseTexture[instance].Get()](true);
            UnityEngine::Texture2D::Apply[RagdollTexture::baseTexture[instance].Get()](true);
        }
    }
}

void OnLoaded() {
    using namespace BNM;
    VirtualHook(CustomizationEditMenu::clazz, CustomizationEditMenu::OnGotFocus,
                new_CustomizationEditMenu_OnGotFocus, old_CustomizationEditMenu_OnGotFocus);
    HOOK(RagdollTexture::LoadFromPreset, new_LoadFromPreset, old_LoadFromPreset);
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