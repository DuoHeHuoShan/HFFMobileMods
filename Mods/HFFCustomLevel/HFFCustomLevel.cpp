#include <jni.h>
#include <BNM/Loading.hpp>
#include "BNM/Class.hpp"
#include "BNM/MethodBase.hpp"
#include "BNM/Method.hpp"
#include "BNM/Utils.hpp"
#include <BNM/Delegates.hpp>
#include "Classes.hpp"
#include <map>

using namespace BNM::Structures;

std::map<uint64_t, std::string> levelNumber2Id;
int originMaxBuiltInLevels = 0;

void AddWorkshopLevel(void *instance, uint64_t levelID, void *workshopMetadata) {
    void *metadata = BuiltinLevelMetadata::clazz.CreateNewObjectParameters();
    WorkshopItemMetadata::folder[metadata] = WorkshopItemMetadata::folder[workshopMetadata].Get();
    WorkshopItemMetadata::workshopId[metadata] = levelID;
//    WorkshopItemMetadata::levelType[metadata] = WorkshopItemSource::BuiltIn;
    WorkshopItemMetadata::itemType[metadata] = WorkshopItemType::Level;
    BuiltinLevelMetadata::internalName[metadata] = WorkshopItemMetadata::title[workshopMetadata].Get();
    WorkshopItemMetadata::title[metadata] = WorkshopItemMetadata::title[workshopMetadata].Get();
    BuiltinLevelMetadata::_thumbPath[metadata] = WorkshopItemMetadata::thumbPath[workshopMetadata].Get();
    WorkshopTypeRepository::AddItem[WorkshopRepository::levelRepo[instance]](WorkshopItemSource::BuiltIn, metadata);
    levelNumber2Id[levelID] = WorkshopItemMetadata::folder[workshopMetadata].Get()->str();
}

void (*old_LoadBuiltinLevels)(void *instance, bool);
void new_LoadBuiltinLevels(void *instance, bool requestLobbies) {
    old_LoadBuiltinLevels(instance, requestLobbies);
    if(!originMaxBuiltInLevels) {
        originMaxBuiltInLevels = Game::kMaxBuiltInLevels;
    }
    levelNumber2Id.clear();
    int baseLevel = originMaxBuiltInLevels + 1;
    std::vector<Mono::String *> levels = FileTools::ListDirectories(BNM::CreateMonoString("lvl:"), false)->ToVector();
    for(int i = 0; i < levels.size(); ++i) {
        AddWorkshopLevel(instance, baseLevel + i, WorkshopItemMetadata::Load(levels[i]));
    }
    Game::kMaxBuiltInLevels = originMaxBuiltInLevels + int(levels.size());
}

void (*old_BeginLoadLevel)(void *, Mono::String *, unsigned long long, int, int, void *);
void new_BeginLoadLevel(void *instance, Mono::String *levelID, unsigned long long levelNumber, int a, int b, void *c) {
    if(levelNumber2Id.contains(levelNumber)) levelID = BNM::CreateMonoString(levelNumber2Id[levelNumber]);
    old_BeginLoadLevel(instance, levelID, levelNumber, a, b, c);
}

void (*old_PauseLeave)(void *, bool);
void new_PauseLeave(void *instance, bool instantLeave) {
    Game::workshopLevel[Game::instance] = nullptr;
    old_PauseLeave(instance, instantLeave);
}

void (*old_Fall)(void *, void *, bool, bool);
void new_Fall(void *instance, void *humanBase, bool drown, bool fallAchievement) {
    if(Game::currentLevelNumber[instance] <= originMaxBuiltInLevels || NetGame::isClient.Get() || !Game::passedLevel[instance].Get()) {
        old_Fall(instance, humanBase, drown, fallAchievement);
        return;
    }
    Game::passedLevel[instance] = false;
//    PlayerManager::SetSingle();
    App::PauseLeave[App::instance](false);
}

void (*old_EnterCheckpoint)(void *, int, int);
void new_EnterCheckpoint(void *instance, int checkpoint, int subObjectives) {
    Game::workshopLevel[instance] = nullptr;
    old_EnterCheckpoint(instance, checkpoint, subObjectives);
}

//For 2.3.0

void *(*old_LoadSceneAsync)(Mono::String *, int);
void *new_LoadSceneAsync(Mono::String *sceneName, int mode) {
    void *result = old_LoadSceneAsync(sceneName, mode);
    if(result) return result;
    return old_LoadSceneAsync(String::Replace[sceneName](BNM::CreateMonoString("_compressed"), BNM::CreateMonoString("")), mode);
}

void OnLoaded() {
    using namespace BNM;
    WorkshopTypeRepository::classDefinition.clazz = WorkshopTypeRepository::clazz.GetGeneric({WorkshopLevelMetadata::clazz});
    WorkshopTypeRepository::AddItem = WorkshopTypeRepository::clazz.GetMethod("AddItem");
    HOOK(WorkshopRepository::LoadBuiltinLevels, new_LoadBuiltinLevels, old_LoadBuiltinLevels);
    HOOK(Game::BeginLoadLevel, new_BeginLoadLevel, old_BeginLoadLevel);
    HOOK((BNM::MethodBase) App::PauseLeave, new_PauseLeave, old_PauseLeave);
    VirtualHook(Game::clazz, Game::Fall, new_Fall, old_Fall);
    VirtualHook(Game::clazz, Game::EnterCheckpoint, new_EnterCheckpoint, old_EnterCheckpoint);

    HOOK(SceneManager::LoadSceneAsync, new_LoadSceneAsync, old_LoadSceneAsync);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);


    // Load BNM by finding the path to libil2cpp.so
    BNM::Loading::TryLoadByJNI(env);

    // Or load using KittyMemory (as an example)
    // Example_07();

    BNM::Loading::AddOnLoadedEvent(BNMU_OnLoaded);
    BNM::Loading::AddOnLoadedEvent(OnLoaded);

    return JNI_VERSION_1_6;
}