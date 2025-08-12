#pragma once

#include <BNMUtils.hpp>
#include <BNM/Field.hpp>
#include <BNM/Property.hpp>
#include <BNM/UnityStructures.hpp>

using namespace BNM::Structures;

enum class WorkshopItemSource {
    BuiltIn = 0,
    LocalWorkshop = 1,
    Subscription = 2,
    BuiltInLobbies = 3,
    SubscriptionLobbies = 4
};

enum class WorkshopItemType {
    Level = 0,
    ModelFull = 1,
    ModelHead = 2,
    ModelUpperBody = 3,
    ModelLowerBody = 4,
    RagdollPreset = 5,
    LevelV1 = 6,
    Lobby = 7
};

BNMU_BeginDefineClass(String, "System", String)
    BNMU_DefineMethod(BNM::Method<Mono::String *>, Replace, { "oldValue", "newValue" })
BNMU_EndDefineClass()

BNMU_BeginDefineClass(SceneManager, "UnityEngine.SceneManagement", SceneManager)
    BNMU_DefineMethod(BNM::MethodBase, LoadSceneAsync, { "sceneName", "mode" })
BNMU_EndDefineClass()

BNMU_BeginDefineClass(FileTools, "", FileTools)
    BNMU_DefineMethod(BNM::Method<Mono::Array<Mono::String *> *>, ListDirectories, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(LevelRepository, "", LevelRepository)
    BNMU_DefineMethod(BNM::MethodBase, GetLevel, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Game, "", Game)
    BNMU_DefineField(BNM::Field<int>, kMaxBuiltInLevels)
    BNMU_DefineField(BNM::Field<int>, currentLevelNumber)
    BNMU_DefineField(BNM::Field<void *>, instance)
    BNMU_DefineField(BNM::Field<void *>, workshopLevel)
    BNMU_DefineField(BNM::Field<bool>, passedLevel)
    BNMU_DefineMethod(BNM::MethodBase, BeginLoadLevel, -1)
    BNMU_DefineMethod(BNM::MethodBase, Fall, -1)
    BNMU_DefineMethod(BNM::MethodBase, EnterCheckpoint, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(App, "Multiplayer", App)
    BNMU_DefineField(BNM::Field<void *>, instance)
    BNMU_DefineMethod(BNM::Method<void>, PauseLeave, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(PlayerManager, "", PlayerManager)
    BNMU_DefineMethod(BNM::Method<void>, SetSingle, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(NetGame, "Multiplayer", NetGame)
    BNMU_DefineField(BNM::Field<bool>, isClient)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(WorkshopRepository, "", WorkshopRepository)
    BNMU_DefineField(BNM::Field<void *>, instance)
    BNMU_DefineField(BNM::Field<void *>, levelRepo)
    BNMU_DefineMethod(BNM::MethodBase, LoadBuiltinLevels, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(WorkshopTypeRepository, "", WorkshopTypeRepository`1)
    BNM::Method<void> AddItem;
BNMU_EndDefineClass()

BNMU_BeginDefineClass(BuiltinLevelMetadata, "HumanAPI", BuiltinLevelMetadata)
    BNMU_DefineField(BNM::Field<Mono::String *>, _thumbPath)
    BNMU_DefineField(BNM::Field<Mono::String *>, internalName)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(WorkshopLevelMetadata, "HumanAPI", WorkshopLevelMetadata)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(WorkshopItemMetadata, "HumanAPI", WorkshopItemMetadata)
    BNMU_DefineMethod(BNM::Method<void *>, Load, 1)
    BNMU_DefineField(BNM::Field<Mono::String *>, folder)
    BNMU_DefineField(BNM::Field<unsigned long long>, workshopId)
//    BNMU_DefineField(BNM::Field<WorkshopItemSource>, levelType)
    BNMU_DefineProperty(BNM::Property<WorkshopItemType>, itemType)
    BNMU_DefineField(BNM::Field<Mono::String *>, title)
    BNMU_DefineProperty(BNM::Property<Mono::String *>, thumbPath)
BNMU_EndDefineClass()