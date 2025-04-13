#pragma once

#include "imgui.h"
#include "Classes.hpp"
#include <BNM/ClassesManagement.hpp>

enum class SpeedrunMode {
    Any,
    NoCheckPoint,
    Checkpoint
};
enum class TimerStyle {
    Solid,
    Gradient
};

struct HFFTimer : public BNM::UnityEngine::MonoBehaviour {
    BNM_CustomClass(HFFTimer, BNM::CompileTimeClassBuilder(nullptr, "HFFTimer").Build(), BNM::CompileTimeClassBuilder("UnityEngine", "MonoBehaviour", "UnityEngine.CoreModule").Build(), {});
    static HFFTimer *instance;
    bool timerWindowOpened = false;
    bool enableTimer = false;
    bool autoReset = true;
    bool timeOnPause = true;
    bool enableRestart = false;
    int restartLevel = 0;
    bool enableRestartButton = false;
    bool restartButtonDraggable = false;
    ImVec2 restartButtonPos = {-1, -1};
    bool displaySubsplits = true;
    SpeedrunMode mode = SpeedrunMode::Any;
    std::string invalidText;

    TimerStyle timerStyle = TimerStyle::Solid;
    ImColor timerColor = ImColor(255, 0, 255);
    ImColor timerColorGradient1 = ImColor(255, 0, 255);
    ImColor timerColorGradient2 = ImColor(255, 0, 255);
    GameState prevGameState = GameState::Inactive;
    AppState prevAppState = AppState::Startup;
    float gameTime = 0;
    float prevGameTime = 0;
    float prevLevelGameTime = 0;
    float ssTime = 0;

    void Constructor();
    static std::string FormatTime(float time);
    std::string GetTimeText();
    std::string GetSpeedrunText();
    bool ShouldToggleMenu();
    void Reset();
    void Update();
    void OnGUI();

    BNM_CustomMethod(Update, false, BNM::Defaults::Get<void>(), "Update");
    BNM_CustomMethodSkipTypeMatch(Update);
    BNM_CustomMethodMarkAsInvokeHook(Update);
    BNM_CustomMethod(Constructor, false, BNM::Defaults::Get<void>(), ".ctor");
};