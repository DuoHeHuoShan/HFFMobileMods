#pragma once
#include <functional>

namespace ImGuiManager {
    void TryInitImGui();
    void AddOnGuiCallback(std::function<void()> func);
}
