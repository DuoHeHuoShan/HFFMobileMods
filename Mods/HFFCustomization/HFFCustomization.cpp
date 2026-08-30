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

bool stopReleaseBaseTexture = false;
void *presetCache = nullptr;

void (*old_ReleaseBaseTexture)(void *);
void ReleaseBaseTexture(void *instance) {
    if (!stopReleaseBaseTexture) old_ReleaseBaseTexture(instance);
}

void LoadTextureFromPreset(void *ragdollTexture, void *preset) {
    old_ReleaseBaseTexture(ragdollTexture);
    std::string partStr = WorkshopItemType2String(RagdollTexture::part[ragdollTexture].Get());
    if (WorkshopItemMetadata::folder[preset].Get() != nullptr) RagdollTexture::savePath[ragdollTexture] = FileTools::Combine(WorkshopItemMetadata::folder[preset].Get(), BNM::CreateMonoString(
                partStr + ".png"));
    RagdollTexture::textureLoadSuppressed[ragdollTexture] = RagdollPresetPartMetadata::suppressCustomTexture[RagdollPresetMetadata::GetPart[preset](RagdollTexture::part[ragdollTexture].Get())].Get();
    if(!RagdollTexture::textureLoadSuppressed[ragdollTexture].Get()) {
        void *ragdollPresetPartMetadata = RagdollPresetMetadata::GetPart[preset](RagdollTexture::part[ragdollTexture].Get());
        if(ragdollPresetPartMetadata != nullptr && RagdollPresetPartMetadata::bytes[ragdollPresetPartMetadata].Get() != nullptr) {
            RagdollTexture::ChangeBaseTexture[ragdollTexture](FileTools::TextureFromBytes(BNM::CreateMonoString(partStr), RagdollPresetPartMetadata::bytes[ragdollPresetPartMetadata].Get()), false);
            if(RagdollTexture::baseTexture[ragdollTexture].Get()->Alive()) {
                UnityEngine::Texture2D::Compress[RagdollTexture::baseTexture[ragdollTexture].Get()](true);
                UnityEngine::Texture2D::Apply[RagdollTexture::baseTexture[ragdollTexture].Get()](true);
                UnityEngine::Object::Destroy(RagdollTexture::bakedTexture[ragdollTexture].Get());
                UnityEngine::Object::Destroy(RagdollTexture::renderTexture[ragdollTexture].Get());
                RagdollTexture::bakedTexture[ragdollTexture] = nullptr;
                RagdollTexture::renderTexture[ragdollTexture] = nullptr;
            }
        } else if(!RagdollTexture::savePath[ragdollTexture].Get()->IsNullOrEmpty() && FileTools::TestExists(RagdollTexture::savePath[ragdollTexture].Get())) {
            bool isAsset;
            void *newRes = FileTools::ReadTexture(RagdollTexture::savePath[ragdollTexture].Get(), &isAsset);
            RagdollTexture::ChangeBaseTexture[ragdollTexture](newRes, isAsset);
            if(RagdollTexture::baseTexture[ragdollTexture].Get()->Alive()) {
                UnityEngine::Texture2D::Compress[RagdollTexture::baseTexture[ragdollTexture].Get()](true);
                UnityEngine::Texture2D::Apply[RagdollTexture::baseTexture[ragdollTexture].Get()](true);
                UnityEngine::Object::Destroy(RagdollTexture::bakedTexture[ragdollTexture].Get());
                UnityEngine::Object::Destroy(RagdollTexture::renderTexture[ragdollTexture].Get());
                RagdollTexture::bakedTexture[ragdollTexture] = nullptr;
                RagdollTexture::renderTexture[ragdollTexture] = nullptr;
            }
        }
    }
}

void (*old_ApplyPresetColors)(void *, void *, bool, bool);
void ApplyPresetColors(void *instance, void *preset, bool bake, bool compress) {
    presetCache = preset;
    old_ApplyPresetColors(instance, preset, bake, compress);
    presetCache = nullptr;
}

void (*old_LoadFromPreset)(void *, void *);
void LoadFromPreset(void *instance, BNM::IL2CPP::Il2CppObject *preset) {
    bool textureLoaded = true;
    if (BNM::Class(preset).GetClass() == RagdollPresetMetadata::clazz.GetClass()) {
        LoadTextureFromPreset(instance, preset);
    } else if (presetCache) {
        LoadTextureFromPreset(instance, presetCache);
    } else {
        textureLoaded = false;
    }
    if (textureLoaded) stopReleaseBaseTexture = true;
    old_LoadFromPreset(instance, preset);
    if (textureLoaded) stopReleaseBaseTexture = false;
}

void OnLoaded() {
    using namespace BNM;
    VirtualHook(CustomizationEditMenu::clazz, CustomizationEditMenu::OnGotFocus,
                new_CustomizationEditMenu_OnGotFocus, old_CustomizationEditMenu_OnGotFocus);
    HOOK(RagdollTexture::LoadFromPreset, LoadFromPreset, old_LoadFromPreset);
    HOOK(RagdollTexture::ApplyPresetColors, ApplyPresetColors, old_ApplyPresetColors);
    HOOK(RagdollTexture::ReleaseBaseTexture, ReleaseBaseTexture, old_ReleaseBaseTexture);
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