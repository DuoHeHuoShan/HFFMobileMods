#pragma once

#include <BNMUtils.hpp>
#include <BNM/Property.hpp>
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
enum class GameState
{
    Inactive,
    Paused,
    LoadingLevel,
    PlayingLevel
};

enum class AppState
{
    Startup = 0,
    Menu = 1,
    Customize = 2,
    LoadLevel = 3,
    PlayLevel = 4,
    ServerHost = 5,
    ServerLoadLobby = 6,
    ServerLobby = 7,
    ServerLoadLevel = 8,
    ServerPlayLevel = 9,
    ClientJoin = 10,
    ClientLoadLobby = 11,
    ClientLobby = 12,
    ClientLoadLevel = 13,
    ClientWaitServerLoad = 14,
    ClientPlayLevel = 15
};

BNMU_BeginDefineClass(HFFResources, "", HFFResources)
    BNMU_DefineMethod(BNM::MethodBase, Awake, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(HumanAPI::Checkpoint, "HumanAPI", Checkpoint)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Game, "", Game)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, instance)
    BNMU_DefineField(BNM::Field<int>, currentLevelNumber)
    BNMU_DefineField(BNM::Field<int>, currentCheckpointNumber)
    BNMU_DefineMethod(BNM::MethodBase, RestartLevel, 1)
    BNMU_DefineField(BNM::Field<GameState>, state)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Multiplayer::App, "Multiplayer", App)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, instance)
    BNMU_DefineProperty(BNM::Property<AppState>, state)
    BNMU_DefineMethod(BNM::Method<void>, PauseLeave, 1)
    BNMU_DefineMethod(BNM::Method<void>, LaunchSinglePlayer, 3)
    BNMU_DefineMethod(BNM::Method<void>, NextLevelServer, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(NetGame, "Multiplayer", NetGame)
    BNMU_DefineField(BNM::Field<void *>, instance)
    BNMU_DefineField(BNM::Field<bool>, isLocal)
    BNMU_DefineField(BNM::Field<bool>, isServer)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Object, "UnityEngine", Object)
    BNMU_DefineMethod(BNM::MethodBase, FindObjectsOfType, 0)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Component, "UnityEngine", Component)
    BNMU_DefineProperty(BNM::Property<BNM::UnityEngine::Object *>, gameObject)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::GameObject, "UnityEngine", GameObject)
    BNMU_DefineMethod(BNM::Method<void>, AddComponent, 1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::MonoBehaviour, "UnityEngine", MonoBehaviour)
    BNMU_DefineMethod(BNM::Method<void *>, StartCoroutine, {"routine"})
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
    BNMU_DefineProperty(BNM::Property<float>, realtimeSinceStartup)
    BNMU_DefineProperty(BNM::Property<float>, unscaledDeltaTime)
    BNMU_DefineProperty(BNM::Property<float>, deltaTime)
BNMU_EndDefineClass()