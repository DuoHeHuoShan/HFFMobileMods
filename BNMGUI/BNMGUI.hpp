#pragma once

#include <BNM/UnityStructures.hpp>
#include "BNMGUI_Classes.hpp"

namespace BNMGUI {
    using namespace Classes;
    enum class ImagePosition
    {
        ImageLeft,
        ImageAbove,
        ImageOnly,
        TextOnly
    };
    enum class TextAnchor
    {
        UpperLeft,
        UpperCenter,
        UpperRight,
        MiddleLeft,
        MiddleCenter,
        MiddleRight,
        LowerLeft,
        LowerCenter,
        LowerRight
    };
    enum class TextClipping
    {
        Overflow,
        Clip
    };
    enum class FontStyle
    {
        Normal,
        Bold,
        Italic,
        BoldAndItalic
    };
    class GUIStyle final {
    private:
        void InitProperties() {
#define INIT_PROPERTY(name) name = UnityEngine::GUIStyle::name[m_ptr]
            INIT_PROPERTY(name);
            INIT_PROPERTY(font);
            INIT_PROPERTY(imagePosition);
            INIT_PROPERTY(alignment);
            INIT_PROPERTY(wordWrap);
            INIT_PROPERTY(clipping);
            INIT_PROPERTY(fixedWidth);
            INIT_PROPERTY(fixedHeight);
            INIT_PROPERTY(stretchWidth);
            INIT_PROPERTY(stretchHeight);
            INIT_PROPERTY(fontSize);
            INIT_PROPERTY(fontStyle);
            INIT_PROPERTY(richText);
#undef INIT_PROPERTY
        }
    public:
        BNM::IL2CPP::Il2CppObject *m_ptr;

        BNM::Property<BNM::Structures::Mono::String *> name;

        BNM::Property<BNM::IL2CPP::Il2CppObject *> font;
        BNM::Property<ImagePosition> imagePosition;
        BNM::Property<TextAnchor> alignment;
        BNM::Property<bool> wordWrap;
        BNM::Property<TextClipping> clipping;
        BNM::Property<float> fixedWidth;
        BNM::Property<float> fixedHeight;
        BNM::Property<float> stretchWidth;
        BNM::Property<float> stretchHeight;
        BNM::Property<int> fontSize;
        BNM::Property<FontStyle> fontStyle;
        BNM::Property<bool> richText;
        GUIStyle() {
            m_ptr = UnityEngine::GUIStyle::classDefinition.clazz.CreateNewObjectParameters();
            InitProperties();
        }
        GUIStyle(BNM::IL2CPP::Il2CppObject *m_ptr) : m_ptr(m_ptr) {
            InitProperties();
        }
        GUIStyle(const GUIStyle &other) {
            m_ptr = UnityEngine::GUIStyle::classDefinition.clazz.CreateNewObjectParameters(other.m_ptr);
            InitProperties();
        }
    };
    class GUI final {
    public:
        static void Label(BNM::Structures::Unity::Rect position, std::string_view text) {
            UnityEngine::GUI::Label2(position, BNM::CreateMonoString(text));
        }
        static void Label(BNM::Structures::Unity::Rect position, std::string_view text, GUIStyle *style) {
            UnityEngine::GUI::Label3(position, BNM::CreateMonoString(text), style->m_ptr);
        }
    };
}