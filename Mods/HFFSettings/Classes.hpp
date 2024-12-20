#pragma once

#include <BNMUtils.hpp>
#include <BNM/Property.hpp>

BNMU_BeginDefineClass(UnityEngine::Application, "UnityEngine", Application)
    BNMU_DefineProperty(BNM::Property<int>, targetFrameRate)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(HumanControls, "", HumanControls)
    BNMU_DefineMethod(BNM::MethodBase, ReadInput, -1)
    BNMU_DefineMethod(BNM::MethodBase, get_calc_joyWalk, -1)
    BNMU_DefineField(BNM::Field<float>, lookHScale)
    BNMU_DefineField(BNM::Field<float>, lookVScale)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Options, "", Options)
    BNMU_DefineMethod(BNM::MethodBase, Load, -1)
    BNMU_DefineMethod(BNM::Method<void>, set_advancedVideoClouds, -1)
    BNMU_DefineMethod(BNM::Method<void>, set_cameraFov, -1)
    BNMU_DefineMethod(BNM::Method<void>, set_cameraSmoothing, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Ball, "", Ball)
    BNMU_DefineMethod(BNM::MethodBase, OnEnable, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::GameObject, "UnityEngine", GameObject)
    BNMU_DefineMethod(BNM::Method<BNM::UnityEngine::Object *>, Find, -1)
    BNMU_DefineMethod(BNM::Method<void>, SetActive, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(Mobile::SaveGameSystem::PlayerPrefs, "Mobile.SaveGameSystem", PlayerPrefs)
    BNMU_DefineMethod(BNM::MethodBase, GetInt, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetInt, -1)
    BNMU_DefineMethod(BNM::MethodBase, GetFloat, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetFloat, -1)
    BNMU_DefineMethod(BNM::MethodBase, GetString, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetString, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::PlayerPrefs, "UnityEngine", PlayerPrefs)
    BNMU_DefineMethod(BNM::MethodBase, GetInt, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetInt, -1)
    BNMU_DefineMethod(BNM::MethodBase, GetFloat, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetFloat, -1)
    BNMU_DefineMethod(BNM::MethodBase, GetString, -1)
    BNMU_DefineMethod(BNM::MethodBase, SetString, -1)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(UnityEngine::QualitySettings, "UnityEngine", QualitySettings)
    BNMU_DefineProperty(BNM::Property<float>, shadowDistance)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(MobileControlScale, "", MobileControlScale)
    BNMU_DefineMethod(BNM::MethodBase, Start, -1)
    BNMU_DefineMethod(BNM::MethodBase, ScaleControls, -1)
    BNMU_DefineField(BNM::Field<BNM::UnityEngine::Object *>, TouchStick)
BNMU_EndDefineClass()

BNMU_BeginDefineClass(MobileControlSchemeManager, "", MobileControlSchemeManager)
    BNMU_DefineMethod(BNM::MethodBase, get_OverridesBodyPitchControls, -1)
    BNMU_DefineProperty(BNM::Property<float>, LeftArmExtendValue)
    BNMU_DefineProperty(BNM::Property<float>, RightArmExtendValue)
BNMU_EndDefineClass()

enum SnapAngles {
    None = 0,
    Four = 4,
    Eight = 8,
    Sixteen = 16
};

BNMU_BeginDefineClass(InControl::TouchStickControl, "InControl", TouchStickControl)
    BNMU_DefineField(BNM::Field<SnapAngles>, snapAngles)
BNMU_EndDefineClass()