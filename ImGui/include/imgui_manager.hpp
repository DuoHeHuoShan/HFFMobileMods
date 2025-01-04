#pragma once
#include <functional>

namespace ImGuiManager {
    void TryInitImGui();
    void AddOnGuiCallback(std::function<void()> func);
    void AddOnLoadedCallback(std::function<void()> func);
}
