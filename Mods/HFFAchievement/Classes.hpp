#pragma once

#include <BNMUtils.hpp>
#include <BNM/Field.hpp>
#include <BNM/Property.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/Delegates.hpp>
#include <BNM/ComplexMonoStructures.hpp>

using namespace BNM::Structures;

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

BNMU_BeginDefineClass(UnityEngine::Screen, "UnityEngine", Screen)
    BNMU_DefineProperty(BNM::Property<int>, width)
    BNMU_DefineProperty(BNM::Property<int>, height)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Input, "UnityEngine", Input)
    BNMU_DefineProperty(BNM::Property<int>, touchCount)
    BNMU_DefineMethod(BNM::Method<UnityEngine::Touch>, GetTouch, 1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(HFFResources, "", HFFResources)
    BNMU_DefineMethod(BNM::MethodBase, Awake, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Component, "UnityEngine", Component)
    BNMU_DefineProperty(BNM::Property<BNM::UnityEngine::Object *>, gameObject)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::GameObject, "UnityEngine", GameObject)
    BNMU_DefineMethod(BNM::Method<void>, AddComponent, 1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(CheatCodes, "", CheatCodes)
    BNMU_DefineField(BNM::Field<bool>, cheatMode)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Enum, "System", Enum)
    BNMU_DefineMethod(BNM::Method<Mono::String *>, ToString, 0)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Achievement, "", Achievement)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(StatsAndAchievements, "", StatsAndAchievements)
    BNMU_DefineMethod(BNM::MethodBase, UnlockAchievementInternal, -1)
    BNMU_DefineMethod(BNM::Method<void>, UnlockAchievement, -1)
    BNMU_DefineMethod(BNM::MethodBase, OnEnable, -1)
    BNMU_DefineMethod(BNM::Method<void>, Save, -1)
    BNMU_DefineMethod(BNM::Method<void>, SyncStats, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(PlayerPrefs, "UnityEngine", PlayerPrefs)
    BNMU_DefineMethod(BNM::Method<Mono::String *>, GetString, 2)
    BNMU_DefineMethod(BNM::Method<void>, SetString, 2)
    BNMU_DefineMethod(BNM::Method<void>, DeleteKey, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Game, "", Game)
    BNMU_DefineField(BNM::Field<int>, currentLevelNumber)
    BNMU_DefineField(BNM::Field<bool>, passedLevel)
    BNMU_DefineField(BNM::Field<bool>, singleRun)
    BNMU_DefineMethod(BNM::MethodBase, Fall, -1)
BNMU_EndDefineClass()