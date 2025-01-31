#pragma once
#include <functional>
#include <imgui.h>
#include <jni.h>

namespace ImGuiManager {
    void TryInitImGui(JavaVM *vm);
    void AddOnGuiCallback(std::function<void()> func);
    void AddOnLoadedCallback(std::function<void()> func);
    void SetStyleVar(ImGuiStyleVar idx, float val);
    void SetStyleVar(ImGuiStyleVar idx, const ImVec2 &val);
}
