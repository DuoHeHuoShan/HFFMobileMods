#include <BNMUtils.hpp>
#include <BNM/Property.hpp>
#include <BNM/Field.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/Method.hpp>
#include <BNM/Coroutine.hpp>

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

enum class HumanState
{
    Idle = 0,
    Walk = 1,
    Climb = 2,
    Jump = 3,
    Slide = 4,
    Fall = 5,
    FreeFall = 6,
    Unconscious = 7,
    Dead = 8,
    Spawning = 9
};

BNMU_BeginDefineClass(HFFResources, "", HFFResources)
    BNMU_DefineMethod(BNM::MethodBase, Awake, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Game, "", Game)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, instance)
    BNMU_DefineField(BNM::Field<bool>, passedLevel)
    BNMU_DefineField(BNM::Field<int>, levelCount)
    BNMU_DefineField(BNM::Field<int>, currentLevelNumber)
    BNMU_DefineField(BNM::Field<int>, currentCheckpointNumber)
    BNMU_DefineMethod(BNM::Method<void>, RestartLevel, 1)
    BNMU_DefineMethod(BNM::Method<void>, Resume, -1)
    BNMU_DefineMethod(BNM::MethodBase, Fall, -1)
    BNMU_DefineMethod(BNM::MethodBase, PassLevel, -1)
    BNMU_DefineField(BNM::Field<GameState>, state)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(CollisionSensor, "", CollisionSensor)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, grabJoint)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, grabObject)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(HumanSegment, "", HumanSegment)
    BNMU_DefineField(BNM::Field<void *>, transform)
    BNMU_DefineField(BNM::Field<void *>, sensor)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Ragdoll, "", Ragdoll)
    BNMU_DefineField(BNM::Field<void *>, partLeftHand)
    BNMU_DefineField(BNM::Field<void *>, partRightHand)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Human, "", Human)
    BNMU_DefineProperty(BNM::Property<void *>, Localplayer)
    BNMU_DefineField(BNM::Field<void *>, grabManager)
    BNMU_DefineField(BNM::Field<void *>, ragdoll)
    BNMU_DefineField(BNM::Field<HumanState>, state)
    BNMU_DefineField(BNM::Field<bool>, jump)
    BNMU_DefineField(BNM::Field<bool>, onGround)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(GrabManager, "", GrabManager)
    BNMU_DefineField(BNM::Field<Mono::List<void *> *>, grabbedObjects)
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
    BNMU_DefineField(BNM::Field<bool>, isClient)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Object, "UnityEngine", Object)
    BNMU_DefineMethod(BNM::MethodBase, FindObjectsOfType, 0)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Component, "UnityEngine", Component)
    BNMU_DefineProperty(BNM::Property<BNM::UnityEngine::Object *>, gameObject)
    BNMU_DefineProperty(BNM::Property<BNM::UnityEngine::Object *>, transform)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Transform, "UnityEngine", Transform)
    BNMU_DefineProperty(BNM::Property<BNM::Structures::Unity::Vector3>, position)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::GameObject, "UnityEngine", GameObject)
    BNMU_DefineMethod(BNM::Method<void>, AddComponent, 1)
    BNMU_DefineMethod(BNM::Method<BNM::UnityEngine::Object *>, Find, 1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::MonoBehaviour, "UnityEngine", MonoBehaviour)
    BNMU_DefineMethod(BNM::Method<BNM::Coroutine::IEnumerator *>, StartCoroutine, {"routine"})
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
    BNMU_DefineProperty(BNM::Property<float>, fixedDeltaTime)
BNMU_EndDefineClass()