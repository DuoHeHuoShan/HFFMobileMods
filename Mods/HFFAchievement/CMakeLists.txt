cmake_minimum_required(VERSION 3.8...3.26)

project(HFFAchievement)
set(CMAKE_CXX_STANDARD 20)

get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)

add_library(
        HFFAchievement

        SHARED

        HFFAchievement.cpp
)

target_include_directories(
        HFFAchievement
        PUBLIC
        ${BNM_INCLUDE_DIRECTORIES}
        ../../BNMUtils
        # ...
)
target_link_libraries(
        HFFAchievement
        PUBLIC
        BNM
        ImGui
        SharedData
)