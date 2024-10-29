#pragma once

#include <BNMUtils.hpp>
#include <BNM/Property.hpp>

namespace BNMGUI {
    enum class ImagePosition;
    enum class TextAnchor;
    enum class FontStyle;
    enum class TextClipping;
}

BNMU_BeginDefineClass(BNMGUI::Classes::UnityEngine::GUI, "UnityEngine", GUI)
    BNMU_DefineMethodName(BNM::Method<void>, Label2, Label, 2)
    BNMU_DefineMethodName(BNM::Method<void>, Label3, Label, { "position", "text", "style" })
BNMU_EndDefineClass()

BNMU_BeginDefineClass(BNMGUI::Classes::UnityEngine::GUIStyle, "UnityEngine", GUIStyle)
    BNMU_DefineProperty(BNM::Property<BNM::Structures::Mono::String *>, name)
    BNMU_DefineProperty(BNM::Property<BNM::IL2CPP::Il2CppObject *>, font)
    BNMU_DefineProperty(BNM::Property<BNMGUI::ImagePosition>, imagePosition)
    BNMU_DefineProperty(BNM::Property<BNMGUI::TextAnchor>, alignment)
    BNMU_DefineProperty(BNM::Property<bool>, wordWrap)
    BNMU_DefineProperty(BNM::Property<BNMGUI::TextClipping>, clipping)
    BNMU_DefineProperty(BNM::Property<float>, fixedWidth)
    BNMU_DefineProperty(BNM::Property<float>, fixedHeight)
    BNMU_DefineProperty(BNM::Property<float>, stretchWidth)
    BNMU_DefineProperty(BNM::Property<float>, stretchHeight)
    BNMU_DefineProperty(BNM::Property<int>, fontSize)
    BNMU_DefineProperty(BNM::Property<BNMGUI::FontStyle>, fontStyle)
    BNMU_DefineProperty(BNM::Property<bool>, richText)
BNMU_EndDefineClass()