#include "HFFTimer.hpp"
#include <jni.h>
#include <mini/ini.h>
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
#include "SubsplitsManager.hpp"
#include "imgui_internal.h"
#include <SharedData.hpp>

using namespace mINI;

const char *modesStr[] = { "Any%",  "No CP%", "CP%" };
const char *timerStylesStr[] = { "纯色", "渐变" };
BNM::Field<AppState> appStateField; // For ImGui thread

INIFile *configFile;
INIStructure configIni;
bool dirty = false;
void WriteConfig();

ImVec2 ReadImVec2(const std::string &format) {
    ImVec2 vec;
    sscanf(format.c_str(), "%f,%f", &vec.x, &vec.y);
    return vec;
}
std::string WriteImVec2(ImVec2 vec) {
    return std::to_string(vec.x) + ',' + std::to_string(vec.y);
}

ImColor ReadImColor(const std::string &format) {
    int i[4];
    const char *str = format.c_str();
    if(str[0] == '#') ++str;
    sscanf(str, "%02X%02X%02X%02X", &i[0], &i[1], &i[2], &i[3]);
    return { i[0], i[1], i[2], i[3] };
}

std::string WriteImColor(ImColor col) {
    char c[10];
    sprintf(c, "#%02X%02X%02X%02X", int(col.Value.x * 255), int(col.Value.y * 255), int(col.Value.z * 255), int(col.Value.w * 255));
    return c;
}

BNM::Method<BNM::Structures::Mono::Array<void *> *> FindCheckpoints;

static void RestartLevel(BNM::UnityEngine::Object *instance, bool reset = true);

void HFFTimer::Constructor() {
    BNM::UnityEngine::MonoBehaviour tmp = *this;
    *this = HFFTimer();
    *((BNM::UnityEngine::MonoBehaviour *)this) = tmp;

    using namespace BNM::Structures::Unity;
    using namespace UnityEngine;
    instance = this;
    if(configIni["Speedrun"].has("restartButtonPos")) restartButtonPos = ReadImVec2(configIni["Speedrun"]["restartButtonPos"]);
    if(configIni["Custom"].has("timerStyle")) timerStyle = (TimerStyle) std::stoi(configIni["Custom"]["timerStyle"]);
    if(configIni["Custom"].has("timerColor")) timerColor = ReadImColor(configIni["Custom"]["timerColor"]);
    if(configIni["Custom"].has("timerColorGradient1")) timerColorGradient1 = ReadImColor(configIni["Custom"]["timerColorGradient1"]);
    if(configIni["Custom"].has("timerColorGradient2")) timerColorGradient2 = ReadImColor(configIni["Custom"]["timerColorGradient2"]);
    ImGuiManager::AddOnGuiCallback(std::bind(&HFFTimer::OnGUI, this));
}

std::string HFFTimer::FormatTime(float time) {
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

std::string HFFTimer::GetTimeText() {
    std::stringstream stringStream;
    stringStream << "总时间: " << FormatTime(gameTime) << std::endl;
    stringStream << "单关: " << FormatTime(ssTime) << std::endl;
    stringStream << "上关总时间: " << FormatTime(prevGameTime) << std::endl;
    stringStream << "上次: " << FormatTime(prevLevelGameTime) << std::endl;
    return stringStream.str();
}

std::string HFFTimer::GetSpeedrunText() {
    std::stringstream stringStream;
    stringStream << "项目: " << modesStr[int(mode)] << std::endl;
    if(mode == SpeedrunMode::Checkpoint)
        stringStream << "存档: " << Game::currentCheckpointNumber[Game::instance].Get() << std::endl;
    return stringStream.str();
}

bool HFFTimer::ShouldToggleMenu() {
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

void HFFTimer::Reset() {
    invalidText = "";
    gameTime = HFFTimer::instance->prevGameTime = 0;
    SharedData::InvokeCallback<void()>("HFFTimer::OnReset");
}

void HFFTimer::Update() {
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
        ssTime = gameTime - prevGameTime;
        prevLevelGameTime = gameTime - prevGameTime;
        prevGameTime = gameTime;
    }
    if((prevGameState == GameState::LoadingLevel || prevGameState == GameState::Inactive) && Game::state[Game::instance] == GameState::PlayingLevel) {
        oldCpNumber = 0;
    } else if(oldCpNumber != Game::currentCheckpointNumber[Game::instance]) {
        int currentCheckpointNumber = Game::currentCheckpointNumber[Game::instance];
        if(mode == SpeedrunMode::NoCheckPoint) {
            invalidText = "无效: 存档";
        }
        if(mode == SpeedrunMode::Checkpoint && currentCheckpointNumber - oldCpNumber > 1) {
            switch (Game::currentLevelNumber[Game::instance]) {
                case 9:
                    if((oldCpNumber != 6 || currentCheckpointNumber != 11) && (oldCpNumber != 17 || currentCheckpointNumber < 19 || currentCheckpointNumber > 23) && (oldCpNumber < 18 || oldCpNumber > 20 || currentCheckpointNumber < 21 || currentCheckpointNumber > 24) && (oldCpNumber < 21 || oldCpNumber > 22 || currentCheckpointNumber != 24)) {
                        invalidText = "无效: 漏点";
                    }
                    break;
                case 21:
                    if((oldCpNumber != 7 || currentCheckpointNumber != 9) && (oldCpNumber != 8 || currentCheckpointNumber != 10)) {
                        invalidText = "无效: 漏点";
                    }
                    break;
                default:
                    invalidText = "无效: 漏点";
                    break;
            }
        }
        oldCpNumber = Game::currentCheckpointNumber[Game::instance];
    }
    prevGameState = Game::state[Game::instance];
    prevAppState = App::state;
    SubsplitsManager::Update();
    if(SharedData::GetData<bool>("HFFSettings::isCheated")) invalidText = "无效: 作弊";
    if(dirty) {
        WriteConfig();
        dirty = false;
    }
}

void HFFTimer::OnGUI() {
    using namespace Multiplayer;
    ImGuiIO &io = ImGui::GetIO();
    if(enableRestartButton) {
        ImGuiWindowFlags button_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground;
        if(!restartButtonDraggable) {
            button_flags |= ImGuiWindowFlags_NoMove;
        }
        if(restartButtonPos.x != -1 && restartButtonPos.y != -1) ImGui::SetNextWindowPos(restartButtonPos, ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(80, 80), ImGuiCond_Once);
        ImGui::Begin("RestartButtonWindow", nullptr, button_flags);
        ImGui::SetWindowFontScale(2.0f);
        if(ImGui::GetWindowPos().x != restartButtonPos.x || ImGui::GetWindowPos().y != restartButtonPos.y) {
            restartButtonPos = ImGui::GetWindowPos();
            configIni["Speedrun"]["restartButtonPos"] = WriteImVec2(restartButtonPos);
            dirty = true;
        }
        ImGui::BeginChild("RestartButton", ImVec2(80, 80), ImGuiChildFlags_None, restartButtonDraggable ? ImGuiWindowFlags_NoInputs : ImGuiWindowFlags_None);
        if(ImGui::Button("R", ImVec2(80, 80)) && appStateField != AppState::LoadLevel && (enableRestart || Game::state[Game::instance] == GameState::PlayingLevel)) {
            if(BNM::AttachIl2Cpp()) {
                Game::RestartLevel[Game::instance](true);
                BNM::DetachIl2Cpp();
            }
        }
        ImGui::EndChild();
        ImGui::End();
    }
    if(enableTimer) {
        ImGuiWindowFlags timer_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;
        ImGui::SetNextWindowPos(ImVec2(10, 0));
        ImGui::Begin("TimerText", nullptr, timer_flags);
        if(timerStyle == TimerStyle::Solid) {
            ImGui::TextColored(timerColor, "%s", GetTimeText().c_str());
        }
        if(timerStyle == TimerStyle::Gradient) {
            auto drawList = ImGui::GetWindowDrawList();
            int vtxStart = drawList->VtxBuffer.size();
            ImGui::TextColored(ImColor(255, 255, 255), "%s", GetTimeText().c_str());
            int vtxEnd = drawList->VtxBuffer.size();
            ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetWindowDrawList(), vtxStart, vtxEnd, ImVec2(10, 0), ImVec2(200, 120), timerColorGradient1, timerColorGradient2);
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(200, 0));
        ImGui::Begin("SpeedrunText", nullptr, timer_flags);
        if(timerStyle == TimerStyle::Solid) {
            ImGui::TextColored(timerColor, "%s", GetSpeedrunText().c_str());
        }
        if(timerStyle == TimerStyle::Gradient) {
            auto drawList = ImGui::GetWindowDrawList();
            int vtxStart = drawList->VtxBuffer.size();
            ImGui::TextColored(ImColor(255, 255, 255), "%s", GetSpeedrunText().c_str());
            int vtxEnd = drawList->VtxBuffer.size();
            ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetWindowDrawList(), vtxStart, vtxEnd, ImVec2(200, 0), ImVec2(390, 120), timerColorGradient1, timerColorGradient2);
        }
        ImGui::TextColored(ImColor(255, 0, 0), "%s", invalidText.c_str());
        ImGui::End();

        if(displaySubsplits) {
            ImGui::SetNextWindowPos(ImVec2(390, 0));
            ImGui::Begin("SubsplitsText", nullptr, timer_flags);
            if(timerStyle == TimerStyle::Solid) {
                ImGui::TextColored(timerColor, "%s", SubsplitsManager::GetSubsplitsText().c_str());
            }
            if(timerStyle == TimerStyle::Gradient) {
                auto drawList = ImGui::GetWindowDrawList();
                int vtxStart = drawList->VtxBuffer.size();
                ImGui::TextColored(ImColor(255, 255, 255), "%s", SubsplitsManager::GetSubsplitsText().c_str());
                int vtxEnd = drawList->VtxBuffer.size();
                ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetWindowDrawList(), vtxStart, vtxEnd, ImVec2(390, 0), ImVec2(580, 120), timerColorGradient1, timerColorGradient2);
            }
            ImGui::End();
        }
    }
    if(!timerWindowOpened) return;
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
    if(ImGui::Begin("HFF手游计时器v0.0.5")) {
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
                ImGui::Checkbox("启用重开按钮", &enableRestartButton);
                ImGui::Checkbox("移动重开按钮位置", &restartButtonDraggable);
                ImGui::Checkbox("显示分段", &displaySubsplits);
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("定制")) {
                if(ImGui::Combo("计时器样式", (int *) &timerStyle, timerStylesStr, IM_ARRAYSIZE(timerStylesStr))) {
                    configIni["Custom"]["timerStyle"] = std::to_string(int(timerStyle));
                    dirty = true;
                }
                if(timerStyle == TimerStyle::Solid) {
                    if(ImGui::ColorPicker4("计时器颜色", (float *) &timerColor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex)) {
                        configIni["Custom"]["timerColor"] = WriteImColor(timerColor);
                        dirty = true;
                    }
                }
                if(timerStyle == TimerStyle::Gradient) {
                    if(ImGui::ColorPicker4("渐变色1", (float *) &timerColorGradient1, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex)) {
                        configIni["Custom"]["timerColorGradient1"] = WriteImColor(timerColorGradient1);
                        dirty = true;
                    }
                    if(ImGui::ColorPicker4("渐变色2", (float *) &timerColorGradient2, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex)) {
                        configIni["Custom"]["timerColorGradient2"] = WriteImColor(timerColorGradient2);
                        dirty = true;
                    }
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

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
    appStateField = Multiplayer::App::clazz.GetField("_state");
    SubsplitsManager::Init();
    HOOK((BNM::MethodBase) Game::RestartLevel, RestartLevel, old_RestartLevel);
    InvokeHook(HFFResources::Awake, HFFResources$Awake, _HFFResources$Awake);
}

std::string GetWorkDir() {
    Dl_info dlInfo;
    if(dladdr((void *) &GetWorkDir, &dlInfo) > 0) {
        std::string soDir = dlInfo.dli_fname;
        return soDir.substr(0, soDir.find_last_of('/'));
    }
    return "";
}

void WriteConfig() {
    if(!configFile)
        configFile = new INIFile(GetWorkDir() + "/HFFTimer.ini");
    configFile->write(configIni, true);
}

void ReadConfig() {
    if(!configFile)
        configFile = new INIFile(GetWorkDir() + "/HFFTimer.ini");
    configFile->read(configIni);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);


    // Load BNM by finding the path to libil2cpp.so
    ImGuiManager::TryInitImGui(vm);
    BNM::Loading::TryLoadByJNI(env);

    // Or load using KittyMemory (as an example)
    // Example_07();

    BNM::Loading::AddOnLoadedEvent(BNMU_OnLoaded);
    BNM::Loading::AddOnLoadedEvent(OnLoaded);

    ReadConfig();

    return JNI_VERSION_1_6;
}