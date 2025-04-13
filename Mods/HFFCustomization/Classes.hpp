#pragma once

#include <BNMUtils.hpp>
#include <BNM/Field.hpp>
#include <BNM/Property.hpp>
#include <BNM/UnityStructures.hpp>

using namespace BNM::Structures;

enum class WorkshopItemType
{
    Level = 0,
    ModelFull = 1,
    ModelHead = 2,
    ModelUpperBody = 3,
    ModelLowerBody = 4,
    RagdollPreset = 5,
    Levels = 6,
    Lobbies = 7
};

std::string WorkshopItemType2String(WorkshopItemType workshopItemType) {
    switch (workshopItemType) {
        case WorkshopItemType::Level:
            return "Level";
        case WorkshopItemType::ModelFull:
            return "ModelFull";
        case WorkshopItemType::ModelHead:
            return "ModelHead";
        case WorkshopItemType::ModelUpperBody:
            return "ModelUpperBody";
        case WorkshopItemType::ModelLowerBody:
            return "ModelLowerBody";
        case WorkshopItemType::RagdollPreset:
            return "RagdollPreset";
        case WorkshopItemType::Levels:
            return "Levels";
        case WorkshopItemType::Lobbies:
            return "Lobbies";
    }
}

BNMU_BeginDefineClass(CustomizationEditMenu, "", CustomizationEditMenu)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, paintButton)
    BNMU_DefineMethod(BNM::MethodBase, OnGotFocus, 0)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(FileTools, "", FileTools)
    BNMU_DefineMethod(BNM::Method<Mono::String *>, Combine, -1)
    BNMU_DefineMethod(BNM::Method<bool>, TestExists, -1)
    BNMU_DefineMethod(BNM::Method<void *>, TextureFromBytes, -1)
    BNMU_DefineMethod(BNM::Method<void *>, ReadTexture, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(RagdollPresetPartMetadata, "HumanAPI", RagdollPresetPartMetadata)
    BNMU_DefineField(BNM::Field<bool>, suppressCustomTexture)
    BNMU_DefineField(BNM::Field<Mono::Array<std::byte> *>, bytes)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(RagdollPresetMetadata, "HumanAPI", RagdollPresetMetadata)
    BNMU_DefineField(BNM::Field<Mono::String *>, folder)
    BNMU_DefineMethod(BNM::Method<void *>, GetPart, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(RagdollTexture, "", RagdollTexture)
    BNMU_DefineField(BNM::Field<Mono::String *>, savePath)
    BNMU_DefineField(BNM::Field<bool>, textureLoadSuppressed)
    BNMU_DefineField(BNM::Field<WorkshopItemType>, part)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, baseTexture)
    BNMU_DefineMethod(BNM::MethodBase, LoadFromPreset, -1)
    BNMU_DefineMethod(BNM::Method<void>, ChangeBaseTexture, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Component, "UnityEngine", Component)
    BNMU_DefineProperty(BNM::Property<BNM::UnityEngine::Object *>, gameObject)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::GameObject, "UnityEngine", GameObject)
    BNMU_DefineMethod(BNM::Method<bool>, SetActive, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::Texture2D, "UnityEngine", Texture2D)
    BNMU_DefineMethod(BNM::Method<void>, Compress, 1)
    BNMU_DefineMethod(BNM::Method<void>, Apply, 1)
BNMU_EndDefineClass()