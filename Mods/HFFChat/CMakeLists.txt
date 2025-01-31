cmake_minimum_required(VERSION 3.8...3.26)

project(HFFChat)
set(CMAKE_CXX_STANDARD 20)

get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)

add_library(
        HFFChat

        SHARED

        HFFChat.cpp
)

target_include_directories(
        HFFChat
        PUBLIC
        ${BNM_INCLUDE_DIRECTORIES}
        ../../BNMUtils
        # ...
)
target_link_libraries(
        HFFChat
        PUBLIC
        BNM
        ImGui
)