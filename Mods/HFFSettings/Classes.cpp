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
    struct Bounds {
        BNM::Structures::Unity::Vector3 m_Center;
        BNM::Structures::Unity::Vector3 m_Extents;
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
    BNMU_DefineMethod(BNM::Method<BNM::Structures::Unity::Vector3>, get_calc_joyWalk, -1)
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

BNMU_BeginDefineClass(UnityEngine::Component, "UnityEngine", Component)
    BNMU_DefineProperty(BNM::Property<BNM::UnityEngine::Object *>, gameObject)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Transform, "UnityEngine", Transform)
    BNMU_DefineProperty(BNM::Property<BNM::Structures::Unity::Vector3>, position)
    BNMU_DefineProperty(BNM::Property<BNM::Structures::Unity::Vector3>, localScale)
    BNMU_DefineProperty(BNM::Property<BNM::Structures::Unity::Quaternion>, rotation)
    BNMU_DefineMethod(BNM::Method<void>, Translate, 1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::GameObject, "UnityEngine", GameObject)
    BNMU_DefineProperty(BNM::Property<void *>, transform)
    BNMU_DefineMethod(BNM::Method<BNM::UnityEngine::Object *>, Find, -1)
    BNMU_DefineMethod(BNM::Method<BNM::UnityEngine::Object *>, CreatePrimitive, -1)
    BNMU_DefineMethod(BNM::Method<void>, AddComponent, 1)
    BNMU_DefineMethod(BNM::Method<BNM::UnityEngine::Object *>, GetComponent, 1)
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
    BNMU_DefineMethod(BNM::MethodBase, ScaleControls, -1)
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
    BNMU_DefineMethod(BNM::MethodBase, GetKeyDown, { "key" })
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Time, "UnityEngine", Time)
    BNMU_DefineProperty(BNM::Property<float>, unscaledDeltaTime)
    BNMU_DefineProperty(BNM::Property<float>, deltaTime)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Material, "UnityEngine", Material)
    BNMU_DefineProperty(BNM::Property<int>, renderQueue)
    BNMU_DefineProperty(BNM::Property<BNM::Structures::Unity::Color>, color)
    BNMU_DefineMethod(BNM::Method<void>, SetFloat, { "name", "value" });
    BNMU_DefineMethod(BNM::Method<void>, SetInt, { "name", "value" })
    BNMU_DefineMethod(BNM::Method<void>, EnableKeyword, -1)
    BNMU_DefineMethod(BNM::Method<void>, DisableKeyword, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Shader, "UnityEngine", Shader)
    BNMU_DefineMethod(BNM::Method<void *>, Find, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Collider, "UnityEngine", Collider)
    BNMU_DefineProperty(BNM::Property<UnityEngine::Bounds>, bounds)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::BoxCollider, "UnityEngine", BoxCollider)
    BNMU_DefineProperty(BNM::Property<BNM::Structures::Unity::Vector3>, size)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Object, "UnityEngine", Object)
    BNMU_DefineMethod(BNM::Method<BNM::Structures::Mono::Array<void *> *>, FindObjectsOfType, 2)
    BNMU_DefineMethod(BNM::Method<void>, Destroy, 1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Renderer, "UnityEngine", Renderer)
    BNMU_DefineProperty(BNM::Property<void *>, material)
    BNMU_DefineProperty(BNM::Property<void *>, sharedMaterial)
    BNMU_DefineProperty(BNM::Property<bool>, receiveShadows)
    BNMU_DefineProperty(BNM::Property<int>, shadowCastingMode)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Game, "", Game)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, instance)
    BNMU_DefineField(BNM::Field<int>, currentLevelNumber)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(CollisionSensor, "", CollisionSensor)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, grabJoint)
    BNMU_DefineMethod(BNM::MethodBase, GrabCheck, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(FreeRoamCam, "", FreeRoamCam)
    BNMU_DefineMethod(BNM::MethodBase, OnEnable, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(AsyncOperation, "UnityEngine", AsyncOperation)
    BNMU_DefineMethod(BNM::MethodBase, get_isDone, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Resources, "UnityEngine", Resources)
    BNMU_DefineMethod(BNM::MethodBase, UnloadUnusedAssets, -1)
BNMU_EndDefineClass()