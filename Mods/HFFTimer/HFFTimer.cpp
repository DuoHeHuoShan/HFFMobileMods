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

const char *modesStr[] = { "Any%",  "No CP%", "CP%" };
enum class SpeedrunMode {
    Any,
    NoCheckPoint,
    Checkpoint
};

BNM::Method<BNM::Structures::Mono::Array<void *> *> FindCheckpoints;

static void RestartLevel(BNM::UnityEngine::Object *instance, bool reset = true);

struct HFFTimer : public BNM::UnityEngine::MonoBehaviour {
    BNM_CustomClass(HFFTimer, BNM::CompileTimeClassBuilder(nullptr, "HFFTimer").Build(),
                    BNM::CompileTimeClassBuilder("UnityEngine", "MonoBehaviour", "UnityEngine.CoreModule").Build(),
                    {});
    static HFFTimer *instance;
    bool timerWindowOpened = false;
    bool enableTimer = false;
    bool autoReset = true;
    bool timeOnPause = true;
    bool enableRestart = false;
    int restartLevel = 0;
    SpeedrunMode mode = SpeedrunMode::Any;
    std::string invalidText;
    int cpMaxCount = 0;
    int cpCount = 0;

    ImColor timerColor = ImColor(255, 0, 255);
    GameState prevGameState = GameState::Inactive;
    AppState prevAppState = AppState::Startup;
    float gameTime = 0;
    float prevGameTime = 0;
    float prevLevelGameTime = 0;
    float ssTime = 0;

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
        int ms = int(fmod(time, 1.0) * 100);
        std::stringstream stringStream;
        stringStream << std::setfill('0');
        if(minutes == 0) stringStream << seconds << '.' << std::setw(2) << ms;
        else if(hours == 0) stringStream << minutes << ':' << std::setw(2) << seconds << '.' << std::setw(2) << ms;
        else stringStream << hours << ':' << std::setw(2) << minutes << ':' << std::setw(2) << seconds << '.' << std::setw(2) << ms;
        return stringStream.str();
    }

    std::string GetTimeText() {
        std::stringstream stringStream;
        stringStream << "总时间: " << FormatTime(gameTime) << std::endl;
        stringStream << "单关: " << FormatTime(ssTime) << std::endl;
        stringStream << "上关总时间: " << FormatTime(prevGameTime) << std::endl;
        stringStream << "上次: " << FormatTime(prevLevelGameTime) << std::endl;
        return stringStream.str();
    }

    std::string GetSpeedrunText() {
        std::stringstream stringStream;
        stringStream << "项目: " << modesStr[int(mode)] << std::endl;
        if(mode == SpeedrunMode::Checkpoint)
            stringStream << "存档: " << cpCount << "/" << cpMaxCount << std::endl;
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

    void Reset() {
        invalidText = "";
        gameTime = HFFTimer::instance->prevGameTime = 0;
    }

    void Update() {
        using namespace UnityEngine;
        using namespace Multiplayer;
        BNM_CallCustomMethodOrigin(Update, this);
        static int oldCpNumber = 0;
        if(ShouldToggleMenu()) timerWindowOpened = !timerWindowOpened;
        if(!Game::instance.Get()->Alive()) return;
        if(autoReset && ((prevAppState == AppState::PlayLevel && App::state == AppState::Menu) || (prevAppState == AppState::ServerLoadLobby && App::state == AppState::ServerLobby) || (prevAppState == AppState::ClientLoadLobby && App::state == AppState::ClientLobby))) Reset();
        if(Game::state[Game::instance] == GameState::PlayingLevel) {
            gameTime += Time::deltaTime;
            ssTime = gameTime - prevGameTime;
        }
        if(timeOnPause && Game::state[Game::instance] == GameState::Paused) {
            gameTime += Time::unscaledDeltaTime;
            ssTime = gameTime - prevGameTime;
        }
        if(prevGameState == GameState::PlayingLevel && Game::state[Game::instance] == GameState::LoadingLevel) {
            gameTime += Time::deltaTime; // 增加通关到前一帧的时间
            prevLevelGameTime = gameTime - prevGameTime;
            prevGameTime = gameTime;
        }
        if((prevGameState == GameState::LoadingLevel || prevGameState == GameState::Inactive) && Game::state[Game::instance] == GameState::PlayingLevel) {
            oldCpNumber = 0;
            cpCount = 0;
            if(Game::currentLevelNumber[Game::instance] == 9) {
                cpMaxCount = 15;
            } else if(Game::currentLevelNumber[Game::instance] == 21) {
                cpMaxCount = 10;
            } else {
                cpMaxCount = FindCheckpoints()->GetCapacity() - 1;
            }
        } else if(oldCpNumber != Game::currentCheckpointNumber[Game::instance]) {
            if(mode == SpeedrunMode::NoCheckPoint) {
                invalidText = "无效!";
            }
            cpCount++;
            oldCpNumber = Game::currentCheckpointNumber[Game::instance];
        }
        prevGameState = Game::state[Game::instance];
        prevAppState = App::state;
    }

    void OnGUI() {
        ImGuiIO &io = ImGui::GetIO();
        if(enableTimer) {
            ImGuiWindowFlags timer_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
            ImGui::SetNextWindowPos(ImVec2(10, 0));
            ImGui::Begin("TimerText", nullptr, timer_flags);
            ImGui::TextColored(timerColor, "%s", GetTimeText().c_str());
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(200, 0));
            ImGui::Begin("SpeedrunText", nullptr, timer_flags);
            ImGui::TextColored(timerColor, "%s", GetSpeedrunText().c_str());
            ImGui::TextColored(ImColor(255, 0, 0), "%s", invalidText.c_str());
            ImGui::End();
        }
        if(!timerWindowOpened) return;
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
        if(ImGui::Begin("HFF手游计时器v0.0.3")) {
            if(ImGui::BeginTabBar("TimerTabBar")) {
                if(ImGui::BeginTabItem("计时")) {
                    ImGui::Checkbox("启用计时器", &enableTimer);
                    ImGui::Checkbox("自动重置", &autoReset);
                    ImGui::Checkbox("暂停时计时", &timeOnPause);
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("速通")) {
                    ImGui::Combo("项目", (int *) &mode, modesStr, IM_ARRAYSIZE(modesStr));
                    ImGui::Checkbox("启用重开", &enableRestart);
                    ImGui::InputInt("重开关卡", &restartLevel);
                    if(ImGui::Button("设为当前关卡")) {
                        restartLevel = Game::currentLevelNumber[Game::instance];
                    }
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("定制")) {
                    ImGui::ColorPicker4("计时器颜色", (float *) &timerColor, ImGuiColorEditFlags_AlphaBar);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
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
    using namespace Multiplayer;
    if(NetGame::isLocal) {
        Game::state[Game::instance] = GameState::Paused;
        co_yield BNM::Coroutine::WaitForFixedUpdate();
        App::PauseLeave[App::instance](false);
        co_yield BNM::Coroutine::WaitForFixedUpdate();
        App::LaunchSinglePlayer[Multiplayer::App::instance](
                (unsigned long long) HFFTimer::instance->restartLevel, 0, 0);
        HFFTimer::instance->Reset();
    } else if(NetGame::isServer) {
        Game::state[Game::instance] = GameState::Paused;
        co_yield BNM::Coroutine::WaitForFixedUpdate();
        Game::state[Game::instance] = GameState::Inactive;
        App::NextLevelServer[App::instance](HFFTimer::instance->restartLevel, 0);
        HFFTimer::instance->Reset();
    }
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
    using namespace UnityEngine;
    using namespace BNM;
    FindCheckpoints = Object::FindObjectsOfType.GetGeneric({HumanAPI::Checkpoint::clazz});
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