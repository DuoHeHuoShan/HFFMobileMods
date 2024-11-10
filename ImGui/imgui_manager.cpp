#include <imgui_manager.hpp>
#include <imgui.h>
#include "imgui/backends/imgui_impl_android.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <dobby.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <utility>
#include <vector>
#include <dlfcn.h>
#include <string>
#include <android/log.h>
#include <filesystem>

static std::vector<std::function<void()>> onGuiFunctions;
static int screenWidth, screenHeight;
static int screenWidthReal, screenHeightReal;

std::string GetWorkDir() {
    Dl_info dlInfo;
    if(dladdr((void *) &GetWorkDir, &dlInfo) > 0) {
        std::string soDir = dlInfo.dli_fname;
        return soDir.substr(0, soDir.find_last_of('/'));
    }
    return "";
}

static void (*old_InputMessage)(AInputEvent *, void *, void *);
static void new_InputMessage(AInputEvent *thiz, void *arg1, void *arg2) {
    old_InputMessage(thiz, arg1, arg2);
    ImGui_ImplAndroid_HandleInputEvent(thiz);
}

static void TrySetupImgui() {
    static bool hasInited = false;
    if(hasInited) return;
    hasInited = true;
    screenWidthReal = screenWidth;
    screenHeightReal = screenHeight;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    std::string workDir = GetWorkDir();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = (workDir + "/imgui.ini").c_str();
    if(std::filesystem::exists(workDir + "/GuiFont.ttf"))
        io.Fonts->AddFontFromFileTTF((workDir + "/GuiFont.ttf").c_str(), 20, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    ImGui_ImplOpenGL3_Init();
    void *sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE"));
    if (nullptr != sym_input) {
        DobbyHook(sym_input,(void *) &new_InputMessage,(void **) &old_InputMessage);
    }
}

static void DrawImGui() {
    if(onGuiFunctions.empty()) return;
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float) screenWidth, (float) screenHeight);
    ImGui::GetStyle() = ImGuiStyle();
    ImGui::GetStyle().ScaleAllSizes(1.5f);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    for(const auto &func : onGuiFunctions) {
        func();
    }
    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static EGLBoolean (*old_eglSwapBuffers)(EGLDisplay, EGLSurface);
static EGLBoolean new_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &screenWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &screenHeight);
    touchRatioX = float(screenWidth) / float(screenWidthReal);
    touchRatioY = float(screenHeight) / float(screenHeightReal);
    TrySetupImgui();
    DrawImGui();
    return old_eglSwapBuffers(dpy, surface);
}

void ImGuiManager::TryInitImGui() {
    static bool hasInited = false;
    if(hasInited) return;
    hasInited = true;
    DobbyHook((void *) &eglSwapBuffers, (void *) &new_eglSwapBuffers, (void **) &old_eglSwapBuffers);
}

void ImGuiManager::AddOnGuiCallback(std::function<void()> func) {
    onGuiFunctions.emplace_back(func);
}