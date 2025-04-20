cmake_minimum_required(VERSION 3.8...3.26)

project(HFFCustomization)
set(CMAKE_CXX_STANDARD 20)

get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)

add_library(
        HFFCustomization

        SHARED

        HFFCustomization.cpp
)

target_include_directories(
        HFFCustomization
        PUBLIC
        ${BNM_INCLUDE_DIRECTORIES}
        ../../BNMUtils
        # ...
)
target_link_libraries(
        HFFCustomization
        PUBLIC
        BNM
)