cmake_minimum_required(VERSION 3.8...3.26)

project(HFFTimer)
set(CMAKE_CXX_STANDARD 20)

get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)
get_property(IMGUI_INCLUDE_DIRECTORIES TARGET ImGui PROPERTY IMGUI_INCLUDE_DIRECTORIES)

add_library(
        HFFTimer

        SHARED

        Classes.cpp

        Subsplits.cpp

        SubsplitsManager.cpp

        HFFTimer.cpp
)

target_include_directories(
        HFFTimer
        PUBLIC
        ${BNM_INCLUDE_DIRECTORIES}
        ${IMGUI_INCLUDE_DIRECTORIES}
        ../../BNMUtils
)
target_link_libraries(
        HFFTimer
        PUBLIC
        BNM
        ImGui
        mINI
        SharedData
)