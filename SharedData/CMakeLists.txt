cmake_minimum_required(VERSION 3.8...3.26)

project(SharedData)
set(CMAKE_CXX_STANDARD 20)

add_library(
        SharedData
        SHARED
        SharedData.cpp
)

target_include_directories(
        SharedData
        PUBLIC
        ./include
)