#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Method.hpp"
#include "BNM/Utils.hpp"
#include <BNM/ClassesManagement.hpp>
#include "Classes.hpp"
#include <imgui_manager.hpp>
#include <regex>

bool chatMenuOpened = false;

enum class ChatBoxMode {
    Console,
    Chat
} chatBoxMode = ChatBoxMode::Console;

BNM::Class Action;
BNM::Class ActionStr;

void SendChatText(const std::string &text) {
    if(chatBoxMode == ChatBoxMode::Chat) {
        NetChat::Send[NetChat::instance](BNM::CreateMonoString(text));
    } else if(chatBoxMode == ChatBoxMode::Console) {
        Shell::Print(BNM::CreateMonoString("> " + text));
        CommandRegistry::Execute[Shell::commands](BNM::CreateMonoString(text));
    }
}

std::string GetChatText() {
    std::string result;
    if(chatBoxMode == ChatBoxMode::Chat) {
        result = NetChat::contents.Get()->str();
    } else if(chatBoxMode == ChatBoxMode::Console) {
        result = Shell::contents.Get()->str();
    }
    result = std::regex_replace(result, std::regex("<#......>|<color=#......>|</color>"), "");
    return result;
}

void OnGUI() {
    if(!chatMenuOpened) return;
    ImGui::SetNextWindowSize(ImVec2(450, 300), ImGuiCond_Once);
    if(ImGui::Begin("HFF手游聊天框插件v0.0.6", nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::BeginChild("chatContentFrame", ImVec2(430, 210), 0,
                          ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Text("%s", GetChatText().c_str());
        ImGui::EndChild();
        if (ImGui::Button(chatBoxMode == ChatBoxMode::Chat ? "聊天" : "终端")) {
            if (chatBoxMode == ChatBoxMode::Chat) chatBoxMode = ChatBoxMode::Console;
            else if (chatBoxMode == ChatBoxMode::Console) chatBoxMode = ChatBoxMode::Chat;
        }
        ImGui::SameLine();
        static char chatInput[100] = "";
        ImGui::InputText("##chatInput", chatInput, 100);
        ImGui::SameLine();
        if (ImGui::Button("发送") && chatInput[0] != '\0') {
            if (BNM::AttachIl2Cpp()) {
                if (chatBoxMode == ChatBoxMode::Chat && chatInput[0] == '/') {
                    if (NetGame::isServer) {
                        CommandRegistry::Execute[NetChat::serverCommands[NetChat::instance]](
                                BNM::CreateMonoString(&chatInput[1]));
                    } else {
                        CommandRegistry::Execute[NetChat::clientCommands[NetChat::instance]](
                                BNM::CreateMonoString(&chatInput[1]));
                    }
                } else {
                    SendChatText(chatInput);
                }
                BNM::DetachIl2Cpp();
            }
            chatInput[0] = '\0';
        }
    }
    ImGui::End();
}

struct HFFChat : public BNM::UnityEngine::MonoBehaviour {
    BNM_CustomClass(HFFChat, BNM::CompileTimeClassBuilder("", "HFFChat").Build(), BNM::CompileTimeClassBuilder("UnityEngine", "MonoBehaviour", "UnityEngine.CoreModule").Build(), {});
    static void LevelChange(Mono::String *str) {
        if(NetGame::isClient) return;
        if(NetGame::isServer) {
            Shell::Print(BNM::CreateMonoString("Cannot change the level in multiplayer"));
            return;
        }
        try {
            int lvl = std::stoi(str->str());
            if(lvl == Game::currentLevelNumber[Game::instance]) {
                CheatCodes::SetCheckpoint(0);
            } else {
                Multiplayer::App::StartNextLevel[Multiplayer::App::instance](lvl, 0);
            }
        } catch (std::exception &) {}
    }
    static void CheckpointChange(Mono::String *str) {
        if(NetGame::isClient) return;
        try {
            int cp = std::stoi(str->str());
            CheatCodes::SetCheckpoint(cp);
            Shell::Print(BNM::CreateMonoString("loading checkpoint " + std::to_string(cp)));
        } catch (std::exception &) {}
    }
    void Constructor() {
        BNM::UnityEngine::MonoBehaviour tmp = *this;
        *this = HFFChat();
        *((BNM::UnityEngine::MonoBehaviour *)this) = tmp;

        ImGuiManager::AddOnGuiCallback(OnGUI);
        Shell::RegisterCommandStr(BNM::CreateMonoString("l"), ActionStr.CreateNewObjectParameters(
                nullptr, BNMCustomMethod_LevelChange.myInfo), nullptr);
        Shell::RegisterCommandStr(BNM::CreateMonoString("level"), ActionStr.CreateNewObjectParameters(
                nullptr, BNMCustomMethod_LevelChange.myInfo), BNM::CreateMonoString("level <level> <checkpoint>\r\nLoad checkpoint at level\r\n\t<level> - 0 based level number, e.g. 0-Intro, 1-Train\r\n\t<checkpoint> - 0 based checkpoint number"));
        Shell::RegisterCommandStr(BNM::CreateMonoString("c"), ActionStr.CreateNewObjectParameters(
                nullptr, BNMCustomMethod_CheckpointChange.myInfo), nullptr);
        Shell::RegisterCommandStr(BNM::CreateMonoString("cp"), ActionStr.CreateNewObjectParameters(
                nullptr, BNMCustomMethod_CheckpointChange.myInfo), BNM::CreateMonoString("cp <checkpoint>\r\nLoad checkpoint\r\n\t<checkpoint> - 0 based checkpoint number"));
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
            chatMenuOpened = !chatMenuOpened;
        }
    }

    BNM_CustomMethod(Update, false, BNM::Defaults::Get<void>(), "Update");
    BNM_CustomMethodSkipTypeMatch(Update);
    BNM_CustomMethodMarkAsInvokeHook(Update);
    BNM_CustomMethod(Constructor, false, BNM::Defaults::Get<void>(), ".ctor");
    BNM_CustomMethod(LevelChange, true, BNM::Defaults::Get<void>(), "LevelChange", {BNM::Defaults::Get<Mono::String *>()});
    BNM_CustomMethod(CheckpointChange, true, BNM::Defaults::Get<void>(), "CheckpointChange", {BNM::Defaults::Get<Mono::String *>()});
};

void (*old_Update)(void *);
void new_Update(void *instance) {
    bool visible = NetChat::visible;
    NetChat::visible = false;
    old_Update(instance);
    NetChat::visible = visible;
    if(!NetGame::isNetStarted) {
        if(NetChat::visible) NetChat::Show[instance](false, false);
        return;
    }
    if(!NetChat::typing && visible && NetChat::dismissIn[instance] > 0.0f) {
        NetChat::dismissIn[instance] = NetChat::dismissIn[instance] - Time::deltaTime;
        if(NetChat::dismissIn[instance] <= 0.0f) NetChat::Show[instance](false, false);
    }
}

void (*_HFFResources$Awake)(BNM::UnityEngine::Object *);
void HFFResources$Awake(BNM::UnityEngine::Object *thiz) {
    using namespace UnityEngine;
    _HFFResources$Awake(thiz);
    UnityEngine::GameObject::AddComponent[UnityEngine::Component::gameObject[thiz]](BNM::Class(HFFChat::BNMCustomClass.myClass).GetMonoType());
}

void OnLoaded() {
    using namespace BNM;
    InvokeHook(NetChat::Update, new_Update, old_Update);
    InvokeHook(HFFResources::Awake, HFFResources$Awake, _HFFResources$Awake);
    Shell::RegisterCommand = Shell::clazz.GetMethod("RegisterCommand", {BNM::Defaults::Get<Mono::String *>(), BNM::CompileTimeClassBuilder("System", "Action").Build(), BNM::Defaults::Get<Mono::String *>()});
    Shell::RegisterCommandStr = Shell::clazz.GetMethod("RegisterCommand", {BNM::Defaults::Get<Mono::String *>(), BNM::CompileTimeClassBuilder("System", "Action`1").Generic({BNM::Defaults::Get<Mono::String *>()}).Build(), BNM::Defaults::Get<Mono::String *>()});
    Action = BNM::CompileTimeClassBuilder("System", "Action").Build();
    ActionStr = BNM::CompileTimeClassBuilder("System", "Action`1").Generic({BNM::Defaults::Get<Mono::String *>()}).Build();
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

    return JNI_VERSION_1_6;
}