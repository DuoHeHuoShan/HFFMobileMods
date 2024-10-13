cmake_minimum_required(VERSION 3.8...3.26)

project(HFFTimer)
set(CMAKE_CXX_STANDARD 20)

get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)

add_library(
        HFFTimer

        SHARED

        HFFTimer.cpp
)

target_include_directories(
        HFFTimer
        PUBLIC
        ${BNM_INCLUDE_DIRECTORIES}
        # ...
)
target_link_libraries(
        HFFTimer
        PUBLIC
        BNM

)