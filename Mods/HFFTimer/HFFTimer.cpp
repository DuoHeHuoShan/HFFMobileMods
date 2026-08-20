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
#include <cstdio>
#include <imgui_manager.hpp>
#include <imgui.h>
#include "SubsplitsManager.hpp"
#include "imgui_internal.h"
#include <SharedData.hpp>
#include <algorithm>
#include <random>
#include <chrono>

using namespace mINI;

const char *modesStr[] = { "Any%", "CP%", "Randomize%" };
const char *randomizeCompletionStr[] = { "Aztec%", "Any%" };
const char *timerStylesStr[] = { "纯色", "渐变" };
const char *timerLayoutsStr[] = { "普通", "简洁" };
BNM::Field<AppState> appStateField; // For ImGui thread

INIFile *configFile;
INIStructure configIni;
bool dirty = false;
void WriteConfig();
BNM::Coroutine::IEnumerator Restart(int level);

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
    if(configIni["Custom"].has("timerSize")) timerSize = std::stof(configIni["Custom"]["timerSize"]);
    if(configIni["Custom"].has("timerColor")) timerColor = ReadImColor(configIni["Custom"]["timerColor"]);
    if(configIni["Custom"].has("timerColorGradient1")) timerColorGradient1 = ReadImColor(configIni["Custom"]["timerColorGradient1"]);
    if(configIni["Custom"].has("timerColorGradient2")) timerColorGradient2 = ReadImColor(configIni["Custom"]["timerColorGradient2"]);
    if(configIni["Custom"].has("timerLayout")) timerLayout = (TimerLayout) std::stoi(configIni["Custom"]["timerLayout"]);
    if(configIni["Custom"].has("subsplitsOnRight")) subsplitsOnRight = std::stoi(configIni["Custom"]["subsplitsOnRight"]);
    ImGuiManager::AddOnGuiCallback(std::bind(&HFFTimer::OnGUI, this));
}

std::string HFFTimer::FormatTime(float time) {
    int hours = int(time) / 3600;
    int minutes = int(time) % 3600 / 60;
    int seconds = int(time) % 60;
    int ms = int(fmod(time, 1.0) * 100);
    char buf[32];
    if(minutes == 0) snprintf(buf, sizeof(buf), "%d.%02d", seconds, ms);
    else if(hours == 0) snprintf(buf, sizeof(buf), "%d:%02d.%02d", minutes, seconds, ms);
    else snprintf(buf, sizeof(buf), "%d:%02d:%02d.%02d", hours, minutes, seconds, ms);
    return buf;
}

std::string HFFTimer::GetTimeText() {
    std::string s;
    s.reserve(96);
    if(timerLayout == TimerLayout::Common) {
        s += "总时间: "; s += FormatTime(gameTime); s += '\n';
        s += "单关: "; s += FormatTime(ssTime); s += '\n';
        if(displayRealtime) { s += "上关实时时间: "; s += FormatTime(prevRealtime); s += '\n'; }
        s += "上关总时间: "; s += FormatTime(prevGameTime); s += '\n';
        s += "上次: "; s += FormatTime(prevLevelGameTime); s += '\n';
    } else if(timerLayout == TimerLayout::Simple) {
        s += "GT: "; s += FormatTime(gameTime); s += '\n';
        s += "SS: "; s += FormatTime(ssTime); s += '\n';
        if(displayRealtime) { s += "Prev RT: "; s += FormatTime(prevRealtime); s += '\n'; }
        s += "Prev GT: "; s += FormatTime(prevGameTime); s += '\n';
        s += "Prev: "; s += FormatTime(prevLevelGameTime); s += '\n';
    }
    return s;
}

std::string HFFTimer::GetSpeedrunText() {
    std::string s;
    s.reserve(64);
    if(timerLayout == TimerLayout::Common) {
        s += "项目: "; s += modesStr[int(mode)];
        if(glitchless) s += " GL";
        s += '\n';
        if(mode == SpeedrunMode::Randomize && setSeed) s += "固定种子\n";
        if(mode == SpeedrunMode::Checkpoint) { s += "存档: "; s += std::to_string(Game::currentCheckpointNumber[Game::instance].Get()); s += '\n'; }
    } else if(timerLayout == TimerLayout::Simple) {
        s += "Mode: "; s += modesStr[int(mode)];
        if(glitchless) s += " GL";
        s += '\n';
        if(mode == SpeedrunMode::Randomize && setSeed) s += "Set Seed\n";
        if(mode == SpeedrunMode::Checkpoint) { s += "CP: "; s += std::to_string(Game::currentCheckpointNumber[Game::instance].Get()); s += '\n'; }
    }
    return s;
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
    gameTime = prevRealtime = prevGameTime = 0;
    SharedData::InvokeCallback<void()>("HFFTimer::OnReset");
}

void HFFTimer::Update() {
    using namespace UnityEngine;
    using namespace Multiplayer;
    if(ShouldToggleMenu()) timerWindowOpened = !timerWindowOpened;
    if(!Game::instance.Get()->Alive()) return;
    if(timeOnPause && Game::state[Game::instance] == GameState::Paused) {
        prevGameState = GameState::Paused;
        gameTime += Time::unscaledDeltaTime;
        ssTime = gameTime - prevGameTime;
    }
    if(SharedData::GetData<bool>("HFFSettings::isCheated")) invalidText = "无效: 作弊";
    if(dirty) {
        WriteConfig();
        dirty = false;
    }
}

void HFFTimer::FixedUpdate() {
    using namespace UnityEngine;
    using namespace Multiplayer;
    static int oldCpNumber = 0;
    if(!Game::instance.Get()->Alive()) return;
    if(autoReset && ((prevGameState == GameState::Paused && App::state == AppState::Menu) || (prevAppState == AppState::ServerLoadLobby && App::state == AppState::ServerLobby) || (prevAppState == AppState::ClientLoadLobby && App::state == AppState::ClientLobby))) {
        restarting = true;
        Reset();
    }
    if(prevGameState == GameState::PlayingLevel && App::state != AppState::ClientWaitServerLoad) {
        gameTime += Time::fixedDeltaTime;
        ssTime = gameTime - prevGameTime;
    }
    if(prevGameState == GameState::PlayingLevel && Game::state[Game::instance] == GameState::LoadingLevel) {
        prevLevelGameTime = gameTime - prevGameTime;
        prevGameTime = gameTime;
        prevRealtime = Time::realtimeSinceStartup - startRealtime;
    }
    if((prevGameState == GameState::LoadingLevel || prevGameState == GameState::Inactive) && Game::state[Game::instance] == GameState::PlayingLevel) {
        if(restarting) {
            IncrementAttempts(Game::currentLevelNumber[Game::instance]);
            startRealtime = Time::realtimeSinceStartup;
        }
        restarting = false;
        oldCpNumber = 0;
    } else if(oldCpNumber != Game::currentCheckpointNumber[Game::instance]) {
        int currentCheckpointNumber = Game::currentCheckpointNumber[Game::instance];
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
    if(glitchless) {
        auto leftHand = HumanSegment::sensor[Ragdoll::partLeftHand[Human::ragdoll[Human::Localplayer]]].Get();
        auto rightHand = HumanSegment::sensor[Ragdoll::partRightHand[Human::ragdoll[Human::Localplayer]]].Get();
        if(Human::onGround[Human::Localplayer] && !CollisionSensor::grabJoint[leftHand].Get()->Alive() && !CollisionSensor::grabJoint[rightHand].Get()->Alive() && (CollisionSensor::grabObject[leftHand].Get()->Alive() || CollisionSensor::grabObject[rightHand].Get()->Alive())) invalidText = "无效: 半身";
    }
    SubsplitsManager::Update();
    prevGameState = Game::state[Game::instance];
    prevAppState = App::state;
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
        if(ImGui::GetWindowPos().x != restartButtonPos.x || ImGui::GetWindowPos().y != restartButtonPos.y) {
            restartButtonPos = ImGui::GetWindowPos();
            configIni["Speedrun"]["restartButtonPos"] = WriteImVec2(restartButtonPos);
            dirty = true;
        }
        ImGui::PushFont(nullptr, ImGui::GetStyle().FontSizeBase * 2.0f);
        ImGui::BeginChild("RestartButton", ImVec2(80, 80), ImGuiChildFlags_None, restartButtonDraggable ? ImGuiWindowFlags_NoInputs : ImGuiWindowFlags_None);
        if(ImGui::Button("R", ImVec2(80, 80)) && appStateField != AppState::LoadLevel) {
            if(BNM::AttachIl2Cpp()) {
                UnityEngine::MonoBehaviour::StartCoroutine[Game::instance](Restart(HFFTimer::instance->restartLevel).get());
                BNM::DetachIl2Cpp();
            }
        }
        ImGui::EndChild();
        ImGui::PopFont();
        ImGui::End();
    }
    if(enableTimer) {
        ImGuiWindowFlags timer_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;
        ImGui::PushFont(nullptr, timerSize);
        float timerRatio = timerSize / 20.0f;
        float timerHeight = 120.0f * timerRatio;
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
            ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetWindowDrawList(), vtxStart, vtxEnd, ImVec2(10, 0), ImVec2(190 * timerRatio + 10, timerHeight), timerColorGradient1, timerColorGradient2);
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(190 * timerRatio + 10, 0));
        ImGui::Begin("SpeedrunText", nullptr, timer_flags);
        if(timerStyle == TimerStyle::Solid) {
            ImGui::TextColored(timerColor, "%s", GetSpeedrunText().c_str());
        }
        if(timerStyle == TimerStyle::Gradient) {
            auto drawList = ImGui::GetWindowDrawList();
            int vtxStart = drawList->VtxBuffer.size();
            ImGui::TextColored(ImColor(255, 255, 255), "%s", GetSpeedrunText().c_str());
            int vtxEnd = drawList->VtxBuffer.size();
            ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetWindowDrawList(), vtxStart, vtxEnd, ImVec2(190 * timerRatio + 10, 0), ImVec2(380 * timerRatio + 10, timerHeight), timerColorGradient1, timerColorGradient2);
        }
        if(displayFootside && !glitchless && Game::currentLevelNumber[Game::instance] == 6 && Game::passedLevel[Game::instance]) ImGui::TextColored(ImColor(0, 255, 0), "%s", "已踩点");
        ImGui::TextColored(ImColor(255, 0, 0), "%s", invalidText.c_str());
        ImGui::End();

        if(displaySubsplits) {
            ImGui::SetNextWindowPos(ImVec2(subsplitsOnRight ? io.DisplaySize.x - 190 * timerRatio : 380 * timerRatio + 10, 0));
            ImGui::Begin("SubsplitsText", nullptr, timer_flags);
            if(timerStyle == TimerStyle::Solid) {
                ImGui::TextColored(timerColor, "%s", SubsplitsManager::GetSubsplitsText().c_str());
            }
            if(timerStyle == TimerStyle::Gradient) {
                auto drawList = ImGui::GetWindowDrawList();
                int vtxStart = drawList->VtxBuffer.size();
                ImGui::TextColored(ImColor(255, 255, 255), "%s", SubsplitsManager::GetSubsplitsText().c_str());
                int vtxEnd = drawList->VtxBuffer.size();
                ImGui::ShadeVertsLinearColorGradientKeepAlpha(ImGui::GetWindowDrawList(), vtxStart, vtxEnd, ImVec2(subsplitsOnRight ? io.DisplaySize.x - 190 * timerRatio : 380 * timerRatio + 10, 0), ImVec2(subsplitsOnRight ? io.DisplaySize.x : 570 * timerRatio + 10, timerHeight), timerColorGradient1, timerColorGradient2);
            }
            ImGui::End();
        }

        ImGui::PopFont();

        if (displayBobCup) {
            ImGui::PushFont(nullptr, 24.0f);
            ImGui::SetNextWindowPos(
                    ImVec2(io.DisplaySize.x - 120 * 1.5f, io.DisplaySize.y - 161.0f));
            ImGui::SetNextWindowSize(ImVec2(120 * 1.5f, 161.0f));
            ImGui::Begin("BobText", nullptr, timer_flags);

            std::time_t now = std::time(nullptr);
            std::tm tm = {};
            localtime_r(&now, &tm);

            auto bobColor = ImColor(255, 69, 0);
            ImGui::TextColored(bobColor, "%s", "手游 Bob杯");
            ImGui::TextColored(bobColor, "%d/%d/%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
            ImGui::TextColored(bobColor, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);

            ImGui::TextColored(bobColor, "%s", "踩点显示:");
            ImGui::SameLine();
            ImGui::TextColored(displayFootside ? ImColor(0, 255, 0) : ImColor(255, 0, 0), "%s",
                               displayFootside ? "开" : "关");

            ImGui::TextColored(bobColor, "%s", "新存档点:");
            ImGui::SameLine();
            ImGui::TextColored(enableNewCheckpoints ? ImColor(0, 255, 0) : ImColor(255, 0, 0), "%s",
                               enableNewCheckpoints ? "开" : "关");

            ImGui::End();
            ImGui::PopFont();
        }
    }
    if(!timerWindowOpened) return;
    static bool p_close = true;
    if(!p_close) return;
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
    if(ImGui::Begin("HFF手游计时器v0.0.7", &p_close)) {
        if(ImGui::BeginTabBar("TimerTabBar")) {
            if(ImGui::BeginTabItem("计时")) {
                ImGui::Checkbox("启用计时器", &enableTimer);
                ImGui::Checkbox("自动重置", &autoReset);
                ImGui::Checkbox("暂停时计时", &timeOnPause);
                ImGui::Checkbox("显示分段", &displaySubsplits);
                ImGui::Checkbox("显示实时时间", &displayRealtime);
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("速通")) {
                ImGui::Combo("项目", (int *) &mode, modesStr, IM_ARRAYSIZE(modesStr));
                if(mode == SpeedrunMode::Randomize) {
                    ImGui::Combo("完成度", (int *) &randomizeCompletion, randomizeCompletionStr,
                                 IM_ARRAYSIZE(randomizeCompletionStr));
                    ImGui::Checkbox("固定种子", &setSeed);
                    if(setSeed) ImGui::InputText("种子", randomSeed, 100);
                }
                ImGui::Checkbox("Glitchless", &glitchless);
                ImGui::Checkbox("覆盖重新开始", &overriveRestartLevel);
                if(mode != SpeedrunMode::Randomize) {
                    ImGui::InputInt("重开关卡", &restartLevel);
                    if (ImGui::Button("设为当前关卡")) {
                        restartLevel = Game::currentLevelNumber[Game::instance];
                    }
                }
                ImGui::Checkbox("启用重开按钮", &enableRestartButton);
                ImGui::Checkbox("移动重开按钮位置", &restartButtonDraggable);
                ImGui::Checkbox("显示重开数", &displayAttempts);
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("定制")) {
                if(ImGui::Combo("计时器布局", (int *) &timerLayout, timerLayoutsStr, IM_ARRAYSIZE(timerLayoutsStr))) {
                    configIni["Custom"]["timerLayout"] = std::to_string(int(timerLayout));
                    dirty = true;
                }
                if(ImGui::Combo("计时器样式", (int *) &timerStyle, timerStylesStr, IM_ARRAYSIZE(timerStylesStr))) {
                    configIni["Custom"]["timerStyle"] = std::to_string(int(timerStyle));
                    dirty = true;
                }
                if(ImGui::Checkbox("右侧显示分段", &subsplitsOnRight)) {
                    configIni["Custom"]["subsplitsOnRight"] = std::to_string(subsplitsOnRight);
                    dirty = true;
                }
                if(ImGui::SliderFloat("计时器大小", &timerSize, 10.0f, 60.0f, "%.0f")) {
                    configIni["Custom"]["timerSize"] = std::to_string(timerSize);
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
            if(ImGui::BeginTabItem("其它")) {
                ImGui::Checkbox("赛事计时器", &displayBobCup);
                ImGui::Checkbox("水踩点显示 (仅比赛使用)", &displayFootside);
                ImGui::Checkbox("启用新版存档点 (仅比赛使用)", &enableNewCheckpoints);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

HFFTimer *HFFTimer::instance;
std::mutex HFFTimer::attemptsMutex;

int HFFTimer::GetAttempts(unsigned long long level) {
    std::lock_guard<std::mutex> lock(attemptsMutex);
    auto it = attempts.find(level);
    return it != attempts.end() ? it->second : 0;
}

void HFFTimer::IncrementAttempts(unsigned long long level) {
    std::lock_guard<std::mutex> lock(attemptsMutex);
    ++attempts[level];
}

void GenerateRandomLevels() {
    HFFTimer::instance->nextLevels.clear();
    int levelCount = HFFTimer::instance->randomizeCompletion == RandomizeCompletion::Aztec ? 9 : 12;
    std::vector<int> levels;
    levels.reserve(levelCount);
    for(int i = 0; i < levelCount; ++i) {
        levels.push_back(i);
    }
    unsigned long seed;
    if(HFFTimer::instance->setSeed) {
        char *endPtr;
        errno = 0;
        seed = strtoul(HFFTimer::instance->randomSeed, &endPtr, 0);
        if(*endPtr != '\0' || errno == ERANGE) {
            std::string str = HFFTimer::instance->randomSeed;
            std::hash<std::string> hashStr;
            seed = hashStr(str);
        }
    } else seed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::mt19937 g(seed);
    std::shuffle(levels.begin(), levels.end(), g);
    HFFTimer::instance->firstLevel = levels[0];
    for(int i = 0; i < levelCount - 1; ++i) {
        HFFTimer::instance->nextLevels[levels[i]] = levels[i + 1];
    }
    HFFTimer::instance->nextLevels[levels[levelCount - 1]] = Game::levelCount[Game::instance];
}

int GetNextLevelRand(int currentLevel) {
    if(HFFTimer::instance->nextLevels.contains(currentLevel)) {
        return HFFTimer::instance->nextLevels[currentLevel];
    }
    return currentLevel + 1;
}

void CheckpointClass::Awake() {
    using namespace UnityEngine;
    if (!HFFTimer::instance->enableNewCheckpoints) return;
    if (Scene::GetNameInternal(SceneManager::GetActiveScene())->str().starts_with("River")) // 水
    {
        if (Checkpoint::number[(void *) this].Get() == 0) // 出生点 cp0
        {
            Transform::position[Component::transform[this]] = Vector3(-84.56f, 2.65f, 81.15f);
        }
        if (Checkpoint::number[(void *) this].Get() == 3) // 大船 cp3
        {
            Transform::position[Component::transform[this]] = Vector3(46.05f, 1.0f, -122.45f);
            void *boxCollider = Component::GetComponent[this](BoxCollider::clazz.GetMonoType());
            BoxCollider::center[boxCollider] = Vector3(9.49f, 0.0f, 5.52f);
        }
    }
}

bool (*old_Raycast)(void *ray, void *hitInfo, float maxDistance, int layerMask, UnityEngine::QueryTriggerInteraction queryTriggerInteraction);
bool Raycast(void *ray, void *hitInfo, float maxDistance, int layerMask, UnityEngine::QueryTriggerInteraction queryTriggerInteraction) {
    if (HFFTimer::instance && HFFTimer::instance->enableNewCheckpoints) return old_Raycast(ray, hitInfo, maxDistance, layerMask, UnityEngine::QueryTriggerInteraction::Collide);
    return old_Raycast(ray, hitInfo, maxDistance, layerMask, queryTriggerInteraction);
}

void *(*old_PassLevel)(void *);
void *PassLevel(void *instance) {
    if(HFFTimer::instance->mode == SpeedrunMode::Randomize) Game::currentLevelNumber[Game::instance] = GetNextLevelRand(Game::currentLevelNumber[Game::instance].Get()) - 1;
    return old_PassLevel(instance);
}

BNM::Coroutine::IEnumerator Restart(int level) {
    using namespace Multiplayer;
    if(HFFTimer::instance->mode == SpeedrunMode::Randomize) {
        GenerateRandomLevels();
        level = HFFTimer::instance->firstLevel;
    }
    if(Game::state[Game::instance] == GameState::Paused) Game::Resume[Game::instance]();
    if(NetGame::isLocal) {
        HFFTimer::instance->restarting = true;
        Game::state[Game::instance] = GameState::Paused;
        co_yield BNM::Coroutine::WaitForFixedUpdate();
        App::PauseLeave[App::instance](false);
        co_yield BNM::Coroutine::WaitForFixedUpdate();
        App::LaunchSinglePlayer[Multiplayer::App::instance](
                (unsigned long long) level, 0, 0);
        HFFTimer::instance->Reset();
    } else if(NetGame::isServer) {
        HFFTimer::instance->restarting = true;
        Game::state[Game::instance] = GameState::Paused;
        co_yield BNM::Coroutine::WaitForFixedUpdate();
        Game::state[Game::instance] = GameState::Inactive;
        if(Game::currentLevelNumber[Game::instance] == level) {
            HFFTimer::instance->restarting = false;
            HFFTimer::instance->startRealtime = UnityEngine::Time::realtimeSinceStartup;
            HFFTimer::instance->IncrementAttempts(level);
        }
        App::NextLevelServer[App::instance](level, 0);
        HFFTimer::instance->Reset();
    }
    co_return;
}

static void (*old_RestartLevel)(BNM::UnityEngine::Object *, bool);
static void RestartLevel(BNM::UnityEngine::Object *instance, bool reset) {
    if(HFFTimer::instance->overriveRestartLevel) {
        UnityEngine::MonoBehaviour::StartCoroutine[instance](Restart(HFFTimer::instance->restartLevel).get());
        return;
    } else if(HFFTimer::instance->mode == SpeedrunMode::Checkpoint) {
        HFFTimer::instance->invalidText = "";
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
    appStateField = Multiplayer::App::clazz.GetField("_state");
    SubsplitsManager::Init();
    HOOK((BNM::MethodBase) Game::RestartLevel, RestartLevel, old_RestartLevel);
    HOOK(Game::PassLevel, PassLevel, old_PassLevel);
    InvokeHook(HFFResources::Awake, HFFResources$Awake, _HFFResources$Awake);
    if (Physics::Raycast.Initialized()) HOOK(Physics::Raycast, Raycast, old_Raycast);
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