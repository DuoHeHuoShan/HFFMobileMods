#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Method.hpp"
#include "BNM/Utils.hpp"
#include "BNM/ClassesManagement.hpp"
#include "BNM/UnityStructures.hpp"
#include "Classes.hpp"
#include <BNM/Coroutine.hpp>
#include <sstream>
#include <iomanip>
#include <imgui_manager.hpp>
#include <imgui.h>
#include <imgui_demo.cpp>

static void RestartLevel(BNM::UnityEngine::Object *instance, bool reset = true);

struct HFFTimer : public BNM::UnityEngine::MonoBehaviour {
    BNM_CustomClass(HFFTimer, BNM::CompileTimeClassBuilder(nullptr, "HFFTimer").Build(),
                    BNM::CompileTimeClassBuilder("UnityEngine", "MonoBehaviour", "UnityEngine.CoreModule").Build(),
                    {});
    static HFFTimer *instance;
    bool timerWindowOpened = false;
    bool enableTimer = false;
    bool enableRestart = false;
    int restartLevel = 0;

    ImColor timerColor = ImColor(255, 0, 255);
    GameState prevGameState = GameState::Inactive;
    float gameTime = 0;
    float prevGameTime = 0;
    float prevRealTime = 0;
    float startRealTime = 0;
    float prevLevelGameTime = 0;
    float prevLevelRealTime = 0;

    void Constructor() {
        BNM::UnityEngine::MonoBehaviour tmp = *this;
        *this = HFFTimer();
        *((BNM::UnityEngine::MonoBehaviour *)this) = tmp;

        using namespace BNM::Structures::Unity;
        using namespace UnityEngine;
        instance = this;
        ImGuiManager::AddOnGuiCallback(std::bind(&HFFTimer::OnGUI, this));
    }

    static std::string FormatTime(float time) {
        int hours = int(time) / 3600;
        int minutes = int(time) % 3600 / 60;
        int seconds = int(time) % 60;
        int ms = int(fmod(time, 1.0) * 1000);
        std::stringstream stringStream;
        stringStream << std::setfill('0');
        if(minutes == 0) stringStream << seconds << '.' << std::setw(3) << ms;
        else if(hours == 0) stringStream << minutes << ':' << std::setw(2) << seconds << '.' << std::setw(3) << ms;
        else stringStream << hours << ':' << std::setw(2) << minutes << ':' << std::setw(2) << seconds << '.' << std::setw(3) << ms;
        return stringStream.str();
    }

    std::string GetTimeText() {
        std::stringstream stringStream;
        stringStream << "游戏时间: " << FormatTime(gameTime) << std::endl;
        stringStream << "现实时间: " << FormatTime(UnityEngine::Time::realtimeSinceStartup - startRealTime) << std::endl;
        stringStream << "上次游戏时间: " << FormatTime(prevGameTime) << std::endl;
        stringStream << "上次现实时间: " << FormatTime(prevRealTime - startRealTime) << std::endl;
        stringStream << "上关游戏时间: " << FormatTime(prevLevelGameTime) << std::endl;
        stringStream << "上关现实时间: " << FormatTime(prevLevelRealTime) << std::endl;
        return stringStream.str();
    }

    bool ShouldToggleMenu() {
        using namespace UnityEngine;
        if(Input::touchCount == 3)
        {
            for(int i = 0; i < 3; i++)
            {
                Touch touch = Input::GetTouch(i);
                if(touch.phase != TouchPhase::Began || touch.position.x > float(Screen::width) / 2.0f) return false;
            }
            return true;
        }
        return false;
    }

    void Update() {
        using namespace UnityEngine;
        BNM_CallCustomMethodOrigin(Update, this);
        if(ShouldToggleMenu()) timerWindowOpened = !timerWindowOpened;
        if(!Game::instance.Get()->Alive()) return;
        if(Game::state[Game::instance] == GameState::PlayingLevel)
            gameTime += UnityEngine::Time::deltaTime;
        if(Game::state[Game::instance] == GameState::Paused)
            gameTime += UnityEngine::Time::unscaledDeltaTime;
        if(prevGameState == GameState::PlayingLevel && Game::state[Game::instance] == GameState::LoadingLevel) {
            prevLevelGameTime = gameTime - prevGameTime;
            prevLevelRealTime = Time::realtimeSinceStartup - prevRealTime;
            prevGameTime = gameTime;
            prevRealTime = Time::realtimeSinceStartup;
        }
        prevGameState = Game::state[Game::instance];
    }

    void OnGUI() {
        ImGuiIO &io = ImGui::GetIO();
        if(enableTimer) {
            ImGuiWindowFlags timer_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::Begin("TimerText", nullptr, timer_flags);
            ImGui::TextColored(timerColor, GetTimeText().c_str());
            ImGui::End();
        }
        if(!timerWindowOpened) return;
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
        if(ImGui::Begin("HFF手游计时器v0.0.2")) {
            ImGui::Checkbox("启用计时器", &enableTimer);
            ImGui::Checkbox("启用重开", &enableRestart);
            ImGui::InputInt("重开关卡", &restartLevel);
            if(ImGui::Button("设为当前关卡")) {
                restartLevel = Game::currentLevelNumber[Game::instance];
            }
        }
        ImGui::End();
    }

    BNM_CustomMethod(Update, false, BNM::Defaults::Get<void>(), "Update");
    BNM_CustomMethodSkipTypeMatch(Update);
    BNM_CustomMethodMarkAsInvokeHook(Update);
    BNM_CustomMethod(Constructor, false, BNM::Defaults::Get<void>(), ".ctor");
};

HFFTimer *HFFTimer::instance;

BNM::Coroutine::IEnumerator Restart() {
    Game::state[Game::instance] = GameState::Paused;
    co_yield BNM::Coroutine::WaitForFixedUpdate();
    Multiplayer::App::PauseLeave[Multiplayer::App::instance](false);
    co_yield BNM::Coroutine::WaitForFixedUpdate();
    Multiplayer::App::LaunchSinglePlayer[Multiplayer::App::instance](HFFTimer::instance->restartLevel, 0, 0);
    HFFTimer::instance->gameTime = HFFTimer::instance->prevGameTime = 0;
    co_yield BNM::Coroutine::WaitUntil([]() -> bool {
        return Game::state[Game::instance] == GameState::PlayingLevel;
    });
    HFFTimer::instance->startRealTime = HFFTimer::instance->prevRealTime = UnityEngine::Time::realtimeSinceStartup;
    co_return;
}

static void (*old_RestartLevel)(BNM::UnityEngine::Object *, bool);
static void RestartLevel(BNM::UnityEngine::Object *instance, bool reset) {
    if(HFFTimer::instance->enableRestart) {
        UnityEngine::MonoBehaviour::StartCoroutine[instance](Restart().get());
        return;
    }
    old_RestartLevel(instance, reset);
}

void (*_HFFResources$Awake)(BNM::UnityEngine::Object *);
void HFFResources$Awake(BNM::UnityEngine::Object *thiz) {
    using namespace UnityEngine;
    _HFFResources$Awake(thiz);
    UnityEngine::GameObject::AddComponent[UnityEngine::Component::gameObject[thiz]](BNM::Class(HFFTimer::BNMCustomClass.myClass).GetMonoType());
}

void OnLoaded() {
    using namespace BNM;
    HOOK(Game::RestartLevel, RestartLevel, old_RestartLevel);
    InvokeHook(HFFResources::Awake, HFFResources$Awake, _HFFResources$Awake);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);


    // Load BNM by finding the path to libil2cpp.so
    ImGuiManager::TryInitImGui();
    BNM::Loading::TryLoadByJNI(env);

    // Or load using KittyMemory (as an example)
    // Example_07();

    BNM::Loading::AddOnLoadedEvent(BNMU_OnLoaded);
    BNM::Loading::AddOnLoadedEvent(OnLoaded);

    return JNI_VERSION_1_6;
}