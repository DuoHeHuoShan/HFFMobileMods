#pragma once

#include <BNMUtils.hpp>
#include <BNM/Field.hpp>
#include <BNM/Property.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/Delegates.hpp>

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

BNMU_BeginDefineClass(Game, "", Game)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, instance)
    BNMU_DefineField(BNM::Field<int>, currentLevelNumber)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Multiplayer::App, "Multiplayer", App)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, instance)
    BNMU_DefineMethod(BNM::Method<void>, StartNextLevel, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(NetGame, "Multiplayer", NetGame)
    BNMU_DefineField(BNM::Field<bool>, isServer)
    BNMU_DefineField(BNM::Field<bool>, isClient)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(NetChat, "Multiplayer", NetChat)
    BNMU_DefineField(BNM::Field<void *>, instance)
    BNMU_DefineField(BNM::Field<void *>, serverCommands)
    BNMU_DefineField(BNM::Field<void *>, clientCommands)
    BNMU_DefineField(BNM::Field<Mono::String *>, contents)
    BNMU_DefineMethod(BNM::Method<void>, Send, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Shell, "", Shell)
    BNMU_DefineField(BNM::Field<Mono::String *>, contents)
    BNMU_DefineField(BNM::Field<void *>, commands)
    BNMU_DefineMethod(BNM::Method<void>, Print, -1)
    BNM::Method<void> RegisterCommand;
    BNM::Method<void> RegisterCommandStr;
BNMU_EndDefineClass()

BNMU_BeginDefineClass(CommandRegistry, "", CommandRegistry)
    BNMU_DefineMethod(BNM::Method<void>, Execute, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(CheatCodes, "", CheatCodes)
    BNMU_DefineMethod(BNM::Method<void>, SetCheckpoint, -1)
BNMU_EndDefineClass()