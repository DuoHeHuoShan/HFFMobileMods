#include <BNMUtils.hpp>
#include <BNM/Property.hpp>
#include <BNM/Method.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/Field.hpp>

namespace UnityEngine {
    enum class TouchPhase
    {
        Began = 0,
        Moved = 1,
        Stationary = 2,
        Ended = 3,
        Canceled = 4
    };
    enum class TouchType
    {
        Direct,
        Indirect,
        Stylus
    };
    struct Touch {
        int fingerId;
        BNM::Structures::Unity::Vector2 position;
        BNM::Structures::Unity::Vector2 rawPosition;
        BNM::Structures::Unity::Vector2 positionDelta;
        float timeDelta;
        int tapCount;
        TouchPhase phase;
        TouchType type;
        float pressure;
        float maximumPossiblePressure;
        float radius;
        float radiusVariance;
        float altitudeAngle;
        float azimuthAngle;
    };
}

BNMU_BeginDefineClass(UnityEngine::Application, "UnityEngine", Application)
    BNMU_DefineProperty(BNM::Property<int>, targetFrameRate)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(HFFResources, "", HFFResources)
    BNMU_DefineMethod(BNM::MethodBase, Awake, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(HumanControls, "", HumanControls)
    BNMU_DefineMethod(BNM::MethodBase, ReadInput, -1)
    BNMU_DefineMethod(BNM::MethodBase, get_calc_joyWalk, -1)
    BNMU_DefineField(BNM::Field<float>, lookHScale)
    BNMU_DefineField(BNM::Field<float>, lookVScale)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Options, "", Options)
    BNMU_DefineMethod(BNM::MethodBase, Load, -1)
    BNMU_DefineMethod(BNM::Method<void>, set_advancedVideoClouds, -1)
    BNMU_DefineMethod(BNM::Method<void>, set_cameraFov, -1)
    BNMU_DefineMethod(BNM::Method<void>, set_cameraSmoothing, -1)
    BNMU_DefineMethod(BNM::Method<void>, ApplyAdvancedVideo, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Ball, "", Ball)
    BNMU_DefineMethod(BNM::MethodBase, OnEnable, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Component, "UnityEngine", Component)
    BNMU_DefineProperty(BNM::Property<BNM::UnityEngine::Object *>, gameObject)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::GameObject, "UnityEngine", GameObject)
    BNMU_DefineMethod(BNM::Method<BNM::UnityEngine::Object *>, Find, -1)
    BNMU_DefineMethod(BNM::Method<void>, SetActive, -1)
    BNMU_DefineMethod(BNM::Method<void>, AddComponent, 1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Mobile::SaveGameSystem::PlayerPrefs, "Mobile.SaveGameSystem", PlayerPrefs)
    BNMU_DefineMethod(BNM::MethodBase, GetInt, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetInt, -1)
    BNMU_DefineMethod(BNM::MethodBase, GetFloat, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetFloat, -1)
    BNMU_DefineMethod(BNM::MethodBase, GetString, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetString, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::PlayerPrefs, "UnityEngine", PlayerPrefs)
    BNMU_DefineMethod(BNM::MethodBase, GetInt, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetInt, -1)
    BNMU_DefineMethod(BNM::MethodBase, GetFloat, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetFloat, -1)
    BNMU_DefineMethod(BNM::MethodBase, GetString, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetString, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::QualitySettings, "UnityEngine", QualitySettings)
    BNMU_DefineProperty(BNM::Property<float>, shadowDistance)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(MobileControlScale, "", MobileControlScale)
    BNMU_DefineMethod(BNM::MethodBase, Start, -1)
    BNMU_DefineMethod(BNM::MethodBase, ScaleControls, -1)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, TouchStick)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(MobileControlSchemeManager, "", MobileControlSchemeManager)
    BNMU_DefineMethod(BNM::MethodBase, get_OverridesBodyPitchControls, -1)
    BNMU_DefineProperty(BNM::Property<float>, LeftArmExtendValue)
    BNMU_DefineProperty(BNM::Property<float>, RightArmExtendValue)
BNMU_EndDefineClass()

enum SnapAngles {
    None = 0,
    Four = 4,
    Eight = 8,
    Sixteen = 16
};

BNMU_BeginDefineClass(InControl::TouchStickControl, "InControl", TouchStickControl)
    BNMU_DefineField(BNM::Field<SnapAngles>, snapAngles)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(TriggerOnQuickSwipe, "", TriggerOnQuickSwipe)
    BNMU_DefineMethod(BNM::MethodBase, GetTriggerState, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Screen, "UnityEngine", Screen)
    BNMU_DefineProperty(BNM::Property<int>, width)
    BNMU_DefineProperty(BNM::Property<int>, height)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Input, "UnityEngine", Input)
    BNMU_DefineProperty(BNM::Property<int>, touchCount)
    BNMU_DefineMethod(BNM::Method<UnityEngine::Touch>, GetTouch, 1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Time, "UnityEngine", Time)
    BNMU_DefineProperty(BNM::Property<float>, unscaledDeltaTime)
BNMU_EndDefineClass()