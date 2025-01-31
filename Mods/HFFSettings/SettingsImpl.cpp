#include <SettingsImpl.hpp>

BNM::UnityEngine::Object *triggerMaterial;

std::vector<BNM::UnityEngine::Object *> CreateHitboxesInComponents(std::vector<void *> components, BNM::Structures::Unity::Color color) {
    using namespace UnityEngine;
    triggerMaterial = (BNM::UnityEngine::Object *) Material::clazz.CreateNewObjectParameters(Shader::Find(BNM::CreateMonoString("Standard")));
    Material::SetFloat[triggerMaterial](BNM::CreateMonoString("_Mode"), 3.0f);
    Material::SetInt[triggerMaterial](BNM::CreateMonoString("_SrcBlend"), 1);
    Material::SetInt[triggerMaterial](BNM::CreateMonoString("_DstBlend"), 10);
    Material::SetInt[triggerMaterial](BNM::CreateMonoString("_ZWrite"), 0);
    Material::DisableKeyword[triggerMaterial](BNM::CreateMonoString("_ALPHATEST_ON"));
    Material::DisableKeyword[triggerMaterial](BNM::CreateMonoString("_ALPHABLEND_ON"));
    Material::EnableKeyword[triggerMaterial](BNM::CreateMonoString("_ALPHAPREMULTIPLY_ON"));
    Material::renderQueue[triggerMaterial] = 3000;
    std::vector<BNM::UnityEngine::Object *> gameObjectList;
    for(void *component : components) {
        BNM::UnityEngine::Object *collider = GameObject::GetComponent[Component::gameObject[component]](BoxCollider::clazz.GetMonoType());
        if(!collider->Alive()) continue;
        BNM::UnityEngine::Object *hitbox = GameObject::CreatePrimitive(3);
        Object::Destroy(GameObject::GetComponent[hitbox](BoxCollider::clazz.GetMonoType()));
        BNM::UnityEngine::Object *renderer = GameObject::GetComponent[hitbox](Renderer::clazz.GetMonoType());
        Renderer::sharedMaterial[renderer] = triggerMaterial;
        Material::color[Renderer::material[renderer]] = color;
        Renderer::receiveShadows[renderer] = false;
        Renderer::shadowCastingMode[renderer] = 0;
        void *hitboxTransform = GameObject::transform[hitbox];
        void *triggerTransform = GameObject::transform[Component::gameObject[component]];
        Transform::rotation[hitboxTransform] = Transform::rotation[triggerTransform].Get();
        Transform::position[hitboxTransform] = Collider::bounds[collider].Get().m_Center;
        Transform::localScale[hitboxTransform] = BoxCollider::size[collider].Get();
        gameObjectList.push_back(hitbox);
    }
    return gameObjectList;
}

void LocalSaveSetting::ApplyLocalSave() {
    using namespace BNM;

    HOOK(Mobile::SaveGameSystem::PlayerPrefs::GetInt, ::UnityEngine::PlayerPrefs::GetInt.GetInfo()->methodPointer, nullptr);
    HOOK(Mobile::SaveGameSystem::PlayerPrefs::SetInt, ::UnityEngine::PlayerPrefs::SetInt.GetInfo()->methodPointer, nullptr);
    HOOK(Mobile::SaveGameSystem::PlayerPrefs::GetFloat, ::UnityEngine::PlayerPrefs::GetFloat.GetInfo()->methodPointer, nullptr);
    HOOK(Mobile::SaveGameSystem::PlayerPrefs::SetFloat, ::UnityEngine::PlayerPrefs::SetFloat.GetInfo()->methodPointer, nullptr);
    HOOK(Mobile::SaveGameSystem::PlayerPrefs::GetString, ::UnityEngine::PlayerPrefs::GetString.GetInfo()->methodPointer, nullptr);
    HOOK(Mobile::SaveGameSystem::PlayerPrefs::SetString, ::UnityEngine::PlayerPrefs::SetString.GetInfo()->methodPointer, nullptr);
}

void new_ScaleControls(BNM::UnityEngine::Object *) {}

void BiggerButtonSetting::OnLoaded(bool value) {
    if(value) HOOK(MobileControlScale::ScaleControls, &new_ScaleControls, nullptr);
}

void (*old_Ball_OnEnable)(BNM::UnityEngine::Object *);
void new_Ball_OnEnable(BNM::UnityEngine::Object *thiz) {
    old_Ball_OnEnable(thiz);
    VisibleBallSetting::instance->ballObject = UnityEngine::GameObject::Find(BNM::CreateMonoString("/Player(Clone)/Ball/Sphere"));
    UnityEngine::GameObject::SetActive[VisibleBallSetting::instance->ballObject](VisibleBallSetting::instance->GetValue());
}

bool (*old_GetTriggerState)(void *thiz, BNM::Structures::Unity::Vector3);
bool new_GetTriggerState(void *thiz, BNM::Structures::Unity::Vector3 newStickMovement) {
    if(DontJumpOnQuickSwipeSetting::instance->GetValue()) return false;
    return old_GetTriggerState(thiz, newStickMovement);
}

void DontJumpOnQuickSwipeSetting::OnLoaded(bool value) {
    HOOK(TriggerOnQuickSwipe::GetTriggerState, new_GetTriggerState, old_GetTriggerState);
}

void VisibleBallSetting::OnLoaded(bool value) {
    InvokeHook(Ball::OnEnable, new_Ball_OnEnable, old_Ball_OnEnable);
}

BNM::Structures::Unity::Vector3 (*old_HumanControls_get_calc_joyWalk)(BNM::UnityEngine::Object *);
BNM::Structures::Unity::Vector3 new_HumanControls_get_calc_joyWalk(BNM::UnityEngine::Object *thiz) {
    BNM::Structures::Unity::Vector3 ret = old_HumanControls_get_calc_joyWalk(thiz);
    if(!PseudoPCSetting::instance->GetValue()) return ret;
    ret.x = round(ret.x);
    ret.z = round(ret.z);
    return ret;
}

void (*old_MobileControlScale_Start)(BNM::UnityEngine::Object *);
void new_MobileControlScale_Start(BNM::UnityEngine::Object *thiz) {
    old_MobileControlScale_Start(thiz);
    BNM::UnityEngine::Object *touchStick = MobileControlScale::TouchStick[thiz];
    if(touchStick) {
        PseudoPCSetting::instance->touchStick = touchStick;
        if(PseudoPCSetting::instance->GetValue()) InControl::TouchStickControl::snapAngles[touchStick] = SnapAngles::Eight;
    }
}

bool (*old_OverridesBodyPitchControls)(BNM::UnityEngine::Object *);
bool new_OverridesBodyPitchControls(BNM::UnityEngine::Object *thiz) {
    if(!PseudoPCSetting::instance->GetValue()) return old_OverridesBodyPitchControls(thiz);
    return MobileControlSchemeManager::LeftArmExtendValue[thiz] != 0 || MobileControlSchemeManager::RightArmExtendValue[thiz] != 0;
}

void PseudoPCSetting::OnLoaded(bool value) {
    InvokeHook(MobileControlScale::Start, new_MobileControlScale_Start, old_MobileControlScale_Start);
    HOOK(HumanControls::get_calc_joyWalk, new_HumanControls_get_calc_joyWalk,
         old_HumanControls_get_calc_joyWalk);
    HOOK(MobileControlSchemeManager::get_OverridesBodyPitchControls, new_OverridesBodyPitchControls,
         old_OverridesBodyPitchControls);
}

void PseudoPCSetting::OnValueChanged(bool value) {
    if(value) {
        InControl::TouchStickControl::snapAngles[touchStick] = SnapAngles::Eight;
    } else {
        InControl::TouchStickControl::snapAngles[touchStick] = SnapAngles::None;
    }
}

void (*old_ReadInput)(BNM::UnityEngine::Object *, void *);
void new_ReadInput(BNM::UnityEngine::Object *thiz, void *outInputState) {
    HumanControls::lookHScale[thiz] = float(LookHScaleSetting::instance->GetValue());
    HumanControls::lookVScale[thiz] = float(LookVScaleSetting::instance->GetValue());
    old_ReadInput(thiz, outInputState);
}

void SharedSetting::OnLoaded() {
    HOOK(HumanControls::ReadInput, new_ReadInput, old_ReadInput);
}

void SharedSetting::OnUpdate() {
    static int oldLevelNumber = -1;
    if(!Game::instance.Get()->Alive()) return;
    if(oldLevelNumber != Game::currentLevelNumber[Game::instance]) {
        oldLevelNumber = Game::currentLevelNumber[Game::instance];
        DisplayLevelPassTriggersSetting::instance->OnLevelChanged(oldLevelNumber);
        DisplayFallTriggersSetting::instance->OnLevelChanged(oldLevelNumber);
        DisplayCheckpointsSetting::instance->OnLevelChanged(oldLevelNumber);
    }
}