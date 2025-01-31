#define STB_IMAGE_IMPLEMENTATION
#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Method.hpp"
#include "BNM/Utils.hpp"
#include <BNM/ClassesManagement.hpp>
#include "Classes.hpp"
#include <imgui_manager.hpp>
#include <queue>
#include <GLES3/gl3.h>
#include <stb_image.h>
#include <filesystem>
#include <fstream>
#include <sstream>
namespace fs = std::filesystem;

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, nullptr, 0);
    if (image_data == nullptr)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

// Open and read a file, then forward to LoadTextureFromMemory()
bool LoadTextureFromFile(const char* file_name, GLuint* out_texture, int* out_width, int* out_height)
{
    FILE* f = fopen(file_name, "rb");
    if (f == nullptr)
        return false;
    fseek(f, 0, SEEK_END);
    size_t file_size = (size_t)ftell(f);
    if (file_size == -1)
        return false;
    fseek(f, 0, SEEK_SET);
    void* file_data = IM_ALLOC(file_size);
    fread(file_data, 1, file_size, f);
    bool ret = LoadTextureFromMemory(file_data, file_size, out_texture, out_width, out_height);
    IM_FREE(file_data);
    return ret;
}

const int POPUP_WIDTH = 400;
const int POPUP_HEIGHT = 120;
const float POPUP_SHOW_TIME = 3;
const float POPUP_STAY_TIME = 2;
const float POPUP_APPEARED_TIME = POPUP_STAY_TIME + (POPUP_SHOW_TIME - POPUP_STAY_TIME) / 2;
const float POPUP_DISAPPEAR_TIME = (POPUP_SHOW_TIME - POPUP_STAY_TIME) / 2;

struct UnlockedAchievementData {
    std::string label;
    std::string description;
    ImTextureID texture = 0;
};

float popupTime = 0;
std::queue<UnlockedAchievementData> unlockAchievementQueue;
UnlockedAchievementData lastAchievementData;
bool achievementWindowOpened = false;

std::vector<int> unlockedAchievements;
std::vector<int> allAchievements;
std::map<std::string, ImTextureID> achievementImageMap;
std::map<std::string, std::string> locale;

void ClearAchievements() {
    unlockedAchievements.clear();
    PlayerPrefs::DeleteKey(BNM::CreateMonoString("UnlockedAchievements"));
}

void LoadUnlockedAchievements() {
    unlockedAchievements.clear();
    Mono::String *str = PlayerPrefs::GetString(BNM::CreateMonoString("UnlockedAchievements"), nullptr);
    if(str->IsNullOrEmpty()) return;
    std::stringstream achievementList(str->str());
    std::string achievementID;
    while(std::getline(achievementList, achievementID, ',')) {
        unlockedAchievements.push_back(std::stoi(achievementID));
    }
}

void SaveUnlockedAchievements() {
    std::string data {};
    auto len = unlockedAchievements.size();
    for(int i = 0; i < len; ++i) {
        data.append(std::to_string(unlockedAchievements[i]));
        if(i != len - 1) data.append(",");
    }
    PlayerPrefs::SetString(BNM::CreateMonoString("UnlockedAchievements"), BNM::CreateMonoString(data));
}

void OnGUI() {
    ImGuiIO &io = ImGui::GetIO();
    if(!unlockAchievementQueue.empty() && popupTime <= 0) {
        popupTime = POPUP_SHOW_TIME;
        lastAchievementData = unlockAchievementQueue.front();
        unlockAchievementQueue.pop();
    }
    if(popupTime > 0 && popupTime <= POPUP_SHOW_TIME) {
        ImGui::SetNextWindowSize(ImVec2(POPUP_WIDTH, POPUP_HEIGHT), ImGuiCond_Once);
        if(popupTime > POPUP_APPEARED_TIME) {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 20, POPUP_HEIGHT * ((POPUP_SHOW_TIME - popupTime) / (POPUP_SHOW_TIME - POPUP_APPEARED_TIME))), ImGuiCond_None, ImVec2(1, 1));
        } else if(popupTime < POPUP_DISAPPEAR_TIME) {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 20, -POPUP_HEIGHT * ((POPUP_DISAPPEAR_TIME - popupTime) / POPUP_DISAPPEAR_TIME)), ImGuiCond_None, ImVec2(1, 0));
        } else {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 20, 0), ImGuiCond_None, ImVec2(1, 0));
        }
        popupTime -= io.DeltaTime;
        const auto window_flag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing;
        ImGui::Begin("解锁成就", nullptr, window_flag);
        if(lastAchievementData.texture != 0) {
            ImGui::BeginChild("AchievementImage", ImVec2(80, -1), ImGuiChildFlags_None, window_flag);
            ImGui::Image(lastAchievementData.texture, ImVec2(64, 64));
            ImGui::EndChild();
            ImGui::SameLine();
        }
        ImGui::BeginChild("AchievementContent", ImVec2(0, 0), ImGuiChildFlags_None, window_flag);
        ImGui::PushTextWrapPos(0.0f);
        ImGui::SetWindowFontScale(1.0f);

        ImGui::Text("%s", lastAchievementData.label.c_str());
        ImGui::Spacing();
        ImGui::SetWindowFontScale(0.8f);
        ImVec4 textColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
        ImGui::TextColored(ImColor(textColor.x, textColor.y, textColor.z, 100.0f / 255.0f), "%s", lastAchievementData.description.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndChild();
        ImGui::End();
    }
    if(!achievementWindowOpened) return;
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);
    if(ImGui::Begin("HFF手游成就插件v0.0.4", nullptr, ImGuiWindowFlags_NoResize)) {
        static int displayMode = 0; // 0 已解锁 1 未解锁 2 全部
        static int current_achievement = -1;
        static std::string currentAchievementName {};
        ImGui::BeginListBox("##AchievementListBox", ImVec2(250, ImGui::GetContentRegionAvail().y));
        std::vector<int> &achievementList = displayMode == 0 ? unlockedAchievements : allAchievements;
        for(int achievementId : achievementList) {
            if(displayMode == 1 &&
                    std::find(unlockedAchievements.begin(), unlockedAchievements.end(), achievementId) != unlockedAchievements.end())
                continue;
            std::string achievementName = Enum::ToString[Achievement::clazz.BoxObject(&achievementId)]()->str();
            std::string achievementLabel = achievementName + "_LABEL";
            if(locale.contains(achievementLabel)) achievementLabel = locale[achievementLabel];
            if(ImGui::Selectable(achievementLabel.c_str(), current_achievement == achievementId)) {
                current_achievement = achievementId;
                currentAchievementName = achievementName;
            }
        }
        ImGui::EndListBox();
        if(current_achievement != -1) {
            ImGui::SameLine();
            ImGui::BeginChild("##AchievementInfo", ImVec2(0, 0), ImGuiChildFlags_FrameStyle);
            ImGui::PushTextWrapPos(0.0f);
            std::string achievementLabel = currentAchievementName + "_LABEL";
            std::string achievementDescription = currentAchievementName + "_DESC";
            if(locale.contains(achievementLabel)) achievementLabel = locale[achievementLabel];
            if(locale.contains(achievementDescription)) achievementDescription = locale[achievementDescription];
            if(achievementImageMap.contains(currentAchievementName)) {
                ImGui::Image(achievementImageMap[currentAchievementName], ImVec2(64, 64));
            }
            ImGui::Text("%s", achievementLabel.c_str());
            ImGui::SetWindowFontScale(0.8f);
            ImVec4 textColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
            ImGui::TextColored(ImColor(textColor.x, textColor.y, textColor.z, 100.0f / 255.0f), "%s", achievementDescription.c_str());
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopTextWrapPos();
            ImGui::Text("状态:");
            ImGui::SameLine();
            if(std::find(unlockedAchievements.begin(), unlockedAchievements.end(), current_achievement) != unlockedAchievements.end()) {
                ImGui::TextColored(ImColor(0, 255, 0), "已达成");
            } else {
                ImGui::TextColored(ImColor(255, 0, 0), "未达成");
            }
            ImGui::EndChild();
        }
        ImGui::Combo("显示范围", &displayMode, "已解锁\0未解锁\0全部\0");
        if(ImGui::Button("清空成就")) {
            if(BNM::AttachIl2Cpp()) {
                ClearAchievements();
                BNM::DetachIl2Cpp();
            }
        }
    }
    ImGui::End();
}

struct HFFAchievement : public BNM::UnityEngine::MonoBehaviour {
    BNM_CustomClass(HFFAchievement, BNM::CompileTimeClassBuilder("", "HFFAchievement").Build(), BNM::CompileTimeClassBuilder("UnityEngine", "MonoBehaviour", "UnityEngine.CoreModule").Build(), {});
    void Constructor() {
        BNM::UnityEngine::MonoBehaviour tmp = *this;
        *this = HFFAchievement();
        *((BNM::UnityEngine::MonoBehaviour *)this) = tmp;

        ImGuiManager::AddOnGuiCallback(OnGUI);
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
        if(ShouldToggleMenu()) {
            achievementWindowOpened = !achievementWindowOpened;
        }
    }

    BNM_CustomMethod(Update, false, BNM::Defaults::Get<void>(), "Update");
    BNM_CustomMethodSkipTypeMatch(Update);
    BNM_CustomMethodMarkAsInvokeHook(Update);
    BNM_CustomMethod(Constructor, false, BNM::Defaults::Get<void>(), ".ctor");
};

void (*_HFFResources$Awake)(BNM::UnityEngine::Object *);
void HFFResources$Awake(BNM::UnityEngine::Object *thiz) {
    using namespace UnityEngine;
    _HFFResources$Awake(thiz);
    UnityEngine::GameObject::AddComponent[UnityEngine::Component::gameObject[thiz]](BNM::Class(HFFAchievement::BNMCustomClass.myClass).GetMonoType());
}

void new_UnlockAchievementInternal(int achievement, bool incAchievement, int) {
    if(!CheatCodes::cheatMode && std::find(unlockedAchievements.begin(),
                                            unlockedAchievements.end(), achievement) == unlockedAchievements.end()) {
        unlockedAchievements.push_back(achievement);
        std::string achievementName = Enum::ToString[Achievement::clazz.BoxObject(&achievement)]()->str();
        std::string unlockedAchievementLabel = achievementName + "_LABEL";
        std::string unlockedAchievementDescription = achievementName + "_DESC";
        ImTextureID unlockedAchievementTexture = 0;
        if(locale.contains(unlockedAchievementLabel)) unlockedAchievementLabel = locale[unlockedAchievementLabel];
        if(locale.contains(unlockedAchievementDescription)) unlockedAchievementDescription = locale[unlockedAchievementDescription];
        if(achievementImageMap.contains(achievementName)) unlockedAchievementTexture = achievementImageMap[achievementName];
        unlockAchievementQueue.push({unlockedAchievementLabel, unlockedAchievementDescription, unlockedAchievementTexture});
        if(!incAchievement) {
            StatsAndAchievements::Save(true);
        }
    }
}

void new_Save(bool triggerSave) {
    StatsAndAchievements::SyncStats(true);
    if(triggerSave)
        SaveUnlockedAchievements();
}

void (*old_Load)(void *);
void new_Load(void *instance) {
    old_Load(instance);
    LoadUnlockedAchievements();
    StatsAndAchievements::SyncStats(false);
}

void (*old_Fall)(void *, void *, bool, bool);
void new_Fall(void *instance, void *humanBase, bool drown, bool fallAchievement) {
    if(Game::passedLevel[instance] && Game::currentLevelNumber[instance] == 11 && Game::singleRun[instance]) {
        StatsAndAchievements::UnlockAchievement(10, false, -1);
    }
    old_Fall(instance, humanBase, drown, fallAchievement);
}

void OnLoaded() {
    using namespace BNM;
    InvokeHook(HFFResources::Awake, HFFResources$Awake, _HFFResources$Awake);
    HOOK(StatsAndAchievements::UnlockAchievementInternal, &new_UnlockAchievementInternal, nullptr);
    HOOK((BNM::MethodBase) StatsAndAchievements::Save, &new_Save, nullptr);
    InvokeHook(StatsAndAchievements::OnEnable, new_Load, old_Load);
    VirtualHook(Game::clazz, Game::Fall, new_Fall, old_Fall);
}

std::string GetWorkDir() {
    Dl_info dlInfo;
    if(dladdr((void *) &GetWorkDir, &dlInfo) > 0) {
        std::string soDir = dlInfo.dli_fname;
        return soDir.substr(0, soDir.find_last_of('/'));
    }
    return "";
}

void ReadAchNames(const std::string& path) {
    std::fstream achInfoFile;
    achInfoFile.open(path, std::ios::in);
    if(!achInfoFile) {
        BNM_LOG_ERR("%s", "Could not open achInfoFile!");
        return;
    }
    std::string line;
    while(std::getline(achInfoFile, line)) {
        if(line.empty()) continue;
        auto mid = line.find_first_of(':');
        auto mid2 = line.find_last_of(':');
        if(mid == std::string::npos || mid2 == std::string::npos) return;
        std::string name = line.substr(0, mid);
        std::string label = line.substr(mid + 1, mid2 - mid - 1);
        std::string description = line.substr(mid2 + 1, line.length() - mid2 - 1);
        locale[name + "_LABEL"] = label;
        locale[name + "_DESC"] = description;
    }
    achInfoFile.close();
}

void OnImGuiLoaded() {
    std::string imagesPath = GetWorkDir() + "/AchievementResources";
    if(!fs::exists(imagesPath) || !fs::is_directory(imagesPath)) return;
    for (const auto& entry : fs::directory_iterator(imagesPath)) {
        fs::path imagePath = entry.path().relative_path();
        std::string imageFileName = imagePath.filename().string();
        if(imageFileName == "ach_names.txt") {
            continue;
        }
        std::string imageName = imageFileName.substr(0, imageFileName.find('.'));
        int width, height;
        GLuint texture;
        LoadTextureFromFile(imagePath.c_str(), &texture, &width, &height);
        achievementImageMap[imageName] = (ImTextureID)(intptr_t) texture;
    }
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);


    // Load BNM by finding the path to libil2cpp.so
    ImGuiManager::TryInitImGui(vm);
    ImGuiManager::AddOnLoadedCallback(OnImGuiLoaded);
    BNM::Loading::TryLoadByJNI(env);

    // Or load using KittyMemory (as an example)
    // Example_07();

    BNM::Loading::AddOnLoadedEvent(BNMU_OnLoaded);
    BNM::Loading::AddOnLoadedEvent(OnLoaded);

    ReadAchNames(GetWorkDir() + "/AchievementResources/ach_names.txt");

    for(int i = 0; i <= 54; ++i) {
        allAchievements.push_back(i);
    }

    return JNI_VERSION_1_6;
}