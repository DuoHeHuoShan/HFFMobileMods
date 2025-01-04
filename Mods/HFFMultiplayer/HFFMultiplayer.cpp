#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include <BNM/BasicMonoStructures.hpp>
#include <BNM/Field.hpp>
#include <fstream>
#include <ctime>

using namespace BNM::Structures;
BNM::Class PlayGamesLocalUser;
BNM::MethodBase get_userName;
BNM::MethodBase get_id;
BNM::MethodBase get_authenticated;
std::string userName;
std::string userId;

std::string GetWorkDir() {
    Dl_info dlInfo;
    if(dladdr((void *) &GetWorkDir, &dlInfo) > 0) {
        std::string soDir = dlInfo.dli_fname;
        return soDir.substr(0, soDir.find_last_of('/'));
    }
    return "";
}

Mono::String *new_get_userName(void *) {
    return BNM::CreateMonoString(userName);
}

Mono::String *new_get_id(void *) {
    return BNM::CreateMonoString(userId);
}

bool new_get_authenticated(void *) {
    return true;
}

void OnLoaded() {
    using namespace BNM;

    PlayGamesLocalUser = Class("GooglePlayGames", "PlayGamesLocalUser");
    get_userName = PlayGamesLocalUser.GetMethod("get_userName");
    get_id = PlayGamesLocalUser.GetMethod("get_id");
    get_authenticated = PlayGamesLocalUser.GetMethod("get_authenticated");
    HOOK(get_userName, &new_get_userName, nullptr);
    HOOK(get_id, &new_get_id, nullptr);
    HOOK(get_authenticated, &new_get_authenticated, nullptr);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    std::fstream userNameFile;
    userNameFile.open(GetWorkDir() + "/MultiplayerName.txt", std::ios_base::in);
    if(userNameFile.bad()) return JNI_VERSION_1_6;
    std::getline(userNameFile, userName);
    userNameFile.close();
    userId = "user" + std::to_string(time(nullptr));

    // Load BNM by finding the path to libil2cpp.so
    BNM::Loading::TryLoadByJNI(env);

    BNM::Loading::AddOnLoadedEvent(OnLoaded);

    return JNI_VERSION_1_6;
}