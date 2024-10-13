cmake_minimum_required(VERSION 3.8...3.26)

project(HFFLevelSelect)
set(CMAKE_CXX_STANDARD 20)

get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)

add_library(
        HFFLevelSelect

        SHARED

        HFFLevelSelect.cpp
)

target_include_directories(
        HFFLevelSelect
        PUBLIC
        ${BNM_INCLUDE_DIRECTORIES}
        # ...
)
target_link_libraries(
        HFFLevelSelect
        PUBLIC
        BNM

)