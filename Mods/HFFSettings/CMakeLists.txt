cmake_minimum_required(VERSION 3.8...3.26)

project(HFFSettings)
set(CMAKE_CXX_STANDARD 20)

get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)
get_property(IMGUI_INCLUDE_DIRECTORIES TARGET ImGui PROPERTY IMGUI_INCLUDE_DIRECTORIES)

add_library(
        HFFSettings

        SHARED

        Classes.cpp

        Settings.cpp

        SettingsImpl.cpp

        HFFSettings.cpp
)

target_include_directories(
        HFFSettings
        PUBLIC
        ${BNM_INCLUDE_DIRECTORIES}
        ${IMGUI_INCLUDE_DIRECTORIES}
        ../../BNMUtils
        ./include
        # ...
)
target_link_libraries(
        HFFSettings
        PUBLIC
        BNM
        ImGui
        mINI
        SharedData
)