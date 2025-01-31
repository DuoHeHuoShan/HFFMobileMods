#include <imgui_manager.hpp>
#include <imgui.h>
#include "imgui/backends/imgui_impl_android.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <dobby.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <utility>
#include <vector>
#include <dlfcn.h>
#include <string>
#include <android/log.h>
#include <filesystem>
#include <mutex>
#include <android/input.h>
#include <map>
#include <imstb_textedit.h>

static std::vector<std::function<void()>> onGuiFunctions;
static std::vector<std::function<void()>> onLoadedFunctions;
static int screenWidth, screenHeight;
static int screenWidthReal, screenHeightReal;
static double g_Time = 0.0;
static std::map<ImGuiStyleVar, float> styleVarsFloat;
static std::map<ImGuiStyleVar, ImVec2> styleVarsVec2;
std::mutex mtx;

JavaVM *jvm;
jclass UnityPlayer;
jfieldID UnityPlayer_CurrentActivity;
jmethodID UnityPlayer_showSoftInput;
jmethodID UnityPlayer_hideSoftInput;
jobject currentActivity;
jclass UnityPlayerActivity;
jfieldID UnityPlayerActivity_mUnityPlayer;
jobject currentUnityPlayer;

std::string lastInputString;

std::string GetWorkDir() {
    Dl_info dlInfo;
    if(dladdr((void *) &GetWorkDir, &dlInfo) > 0) {
        std::string soDir = dlInfo.dli_fname;
        return soDir.substr(0, soDir.find_last_of('/'));
    }
    return "";
}

static void (*old_InputMessage)(AInputEvent *, void *, void *);
static void new_InputMessage(AInputEvent *input_event, void *arg1, void *arg2) {
    old_InputMessage(input_event, arg1, arg2);
    if(ImGui::GetCurrentContext() == nullptr || onGuiFunctions.empty()) return;
    mtx.lock();
    ImGui_ImplAndroid_HandleInputEvent(input_event);
    mtx.unlock();
}

static int32_t (*old_consume)(void *thiz, void *, bool, long, uint32_t *, AInputEvent **);
static int32_t new_consume(void *thiz, void *arg1, bool arg2, long arg3, uint32_t *arg4, AInputEvent **input_event) {
    auto result = old_consume(thiz, arg1, arg2, arg3, arg4, input_event);
    if(ImGui::GetCurrentContext() == nullptr || onGuiFunctions.empty()) return result;
    if(result != 0 || *input_event == nullptr) return result;
    mtx.lock();
    ImGui_ImplAndroid_HandleInputEvent(*input_event);
    mtx.unlock();
    return result;
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
    ImGui::GetStyle().ScaleAllSizes(1.5f);
    for(const auto &func : onLoadedFunctions) {
        func();
    }
}

static void GetUnityPlayer(JNIEnv *env) {
    currentActivity = env->NewGlobalRef(env->GetStaticObjectField(UnityPlayer, UnityPlayer_CurrentActivity));
    currentUnityPlayer = env->NewGlobalRef(env->GetObjectField(currentActivity, UnityPlayerActivity_mUnityPlayer));
}

static void displayKeyboard(bool display) {
    JNIEnv *env;
    jvm->AttachCurrentThread(&env, nullptr);
    if(!currentUnityPlayer) GetUnityPlayer(env);
    if(currentUnityPlayer) {
        if(display) {
            auto &inputTextState = ImGui::GetCurrentContext()->InputTextState;
            inputTextState.ClearSelection();
            env->CallVoidMethod(currentUnityPlayer, UnityPlayer_showSoftInput, env->NewStringUTF(inputTextState.InitialTextA.Data), 0, false, false, false, false, env->NewStringUTF(""), 0, false, false);
        } else {
            env->CallVoidMethod(currentUnityPlayer, UnityPlayer_hideSoftInput);
        }
    }
    jvm->DetachCurrentThread();
}

static void DrawImGui() {
    if(onGuiFunctions.empty()) return;
    ImGuiIO &io = ImGui::GetIO();

    static bool WantTextInputLast = false;
    if(io.WantTextInput != WantTextInputLast)
        displayKeyboard(io.WantTextInput);
    WantTextInputLast = io.WantTextInput;
    io.DisplaySize = ImVec2((float) screenWidth, (float) screenHeight);
    ImGui_ImplOpenGL3_NewFrame();

    // Setup time step
    struct timespec current_timespec;
    clock_gettime(CLOCK_MONOTONIC, &current_timespec);
    double current_time = (double)(current_timespec.tv_sec) + (current_timespec.tv_nsec / 1000000000.0);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;

    mtx.lock();
    ImGui::NewFrame();
    mtx.unlock();
    for(auto &styleVar : styleVarsFloat) {
        ImGui::PushStyleVar(styleVar.first, styleVar.second);
    }
    for(auto &styleVar : styleVarsVec2) {
        ImGui::PushStyleVar(styleVar.first, styleVar.second);
    }
    int styleVarsCount = styleVarsFloat.size() + styleVarsVec2.size();
    for(const auto &func : onGuiFunctions) {
        func();
    }
    ImGui::PopStyleVar(styleVarsCount);
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

void ImGuiManager::AddOnGuiCallback(std::function<void()> func) {
    onGuiFunctions.emplace_back(func);
}

void ImGuiManager::AddOnLoadedCallback(std::function<void()> func) {
    onLoadedFunctions.emplace_back(func);
}

void ImGuiManager::SetStyleVar(ImGuiStyleVar idx, float val) {
    styleVarsFloat[idx] = val;
}

void ImGuiManager::SetStyleVar(ImGuiStyleVar idx, const ImVec2 &val) {
    styleVarsVec2[idx] = val;
}

void (*old_nativeSetInputString)(JNIEnv *, jobject, jstring);
void new_nativeSetInputString(JNIEnv *env, jobject instance, jstring str) {
    if(ImGui::GetCurrentContext() == nullptr || onGuiFunctions.empty()) {
        old_nativeSetInputString(env, instance, str);
        return;
    }
    lastInputString = env->GetStringUTFChars(str, nullptr);
    ImGuiIO &io = ImGui::GetIO();
    if(io.WantTextInput) {
        auto &inputTextState = ImGui::GetCurrentContext()->InputTextState;
        inputTextState.ClearSelection();
        inputTextState.TextA.resize(lastInputString.length() + 1);
        memcpy(inputTextState.TextA.Data, lastInputString.c_str(), lastInputString.length() + 1);
        inputTextState.CurLenA = lastInputString.length();
        ((STB_TexteditState *)inputTextState.Stb)->cursor = lastInputString.length();
    }
    old_nativeSetInputString(env, instance, str);
}

jint (*old_RegisterNatives)(JNIEnv*, jclass, JNINativeMethod*, jint);
jint new_RegisterNatives(JNIEnv* env, jclass destinationClass, JNINativeMethod* methods,
                          jint methodCount){
    for (int i = 0; i < methodCount; ++i )
    {
        if (!strcmp(methods[i].name, "nativeSetInputString") ) {
            DobbyHook(methods[i].fnPtr, (void *) &new_nativeSetInputString, (void **) &old_nativeSetInputString);
        }
    }
    return old_RegisterNatives(env, destinationClass, methods, methodCount);
}

void ImGuiManager::TryInitImGui(JavaVM *vm) {
    static bool hasInited = false;
    if(hasInited) return;
    hasInited = true;
    jvm = vm;
    JNIEnv *env;
    jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    UnityPlayer = env->FindClass("com/unity3d/player/UnityPlayer");
    UnityPlayer_CurrentActivity = env->GetStaticFieldID(UnityPlayer, "currentActivity","Landroid/app/Activity;");
    UnityPlayer_showSoftInput = env->GetMethodID(UnityPlayer, "showSoftInput", "(Ljava/lang/String;IZZZZLjava/lang/String;IZZ)V");
    UnityPlayer_hideSoftInput = env->GetMethodID(UnityPlayer, "hideSoftInput", "()V");
    UnityPlayerActivity = env->FindClass("com/unity3d/player/UnityPlayerActivity");
    UnityPlayerActivity_mUnityPlayer = env->GetFieldID(UnityPlayerActivity, "mUnityPlayer", "Lcom/unity3d/player/UnityPlayer;");
    DobbyHook((void *) env->functions->RegisterNatives, (void *) new_RegisterNatives, (void **) &old_RegisterNatives);
    DobbyHook((void *) &eglSwapBuffers, (void *) &new_eglSwapBuffers, (void **) &old_eglSwapBuffers);
    void *sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE"));
    if (nullptr != sym_input) {
        DobbyHook(sym_input, (void *) &new_InputMessage, (void **) &old_InputMessage);
    } else {
        sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer7consumeEPNS_26InputEventFactoryInterfaceEblPjPPNS_10InputEventE"));
        if(nullptr != sym_input) {
            DobbyHook(sym_input, (void *) &new_consume, (void **) &old_consume);
        }
    }
}