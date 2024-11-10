cmake_minimum_required(VERSION 3.8...3.26)

project(ImGui)
set(CMAKE_CXX_STANDARD 20)

# Dobby Hook
add_library(DOBBY_LIB STATIC IMPORTED)
set_target_properties(DOBBY_LIB PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Dobby/libs/${CMAKE_ANDROID_ARCH_ABI}/libdobby.a)

add_library(
        ImGui
        SHARED
        ./imgui/imgui.cpp
        ./imgui/imgui_demo.cpp
        ./imgui/imgui_draw.cpp
        ./imgui/imgui_tables.cpp
        ./imgui/imgui_widgets.cpp
        ./imgui/backends/imgui_impl_android.cpp
        ./imgui/backends/imgui_impl_opengl3.cpp
        ./imgui_manager.cpp
)

set(
        INCLUDE_DIRS
        ./imgui
        ./include
        ${CMAKE_SOURCE_DIR}/Dobby/include
)

target_include_directories(
        ImGui
        PUBLIC
        ${INCLUDE_DIRS}
)

set_property(TARGET ImGui PROPERTY IMGUI_INCLUDE_DIRECTORIES
        ./imgui
        ./include
)
target_compile_definitions(
        ImGui
        PRIVATE
        IMGUI_IMPL_OPENGL_ES3
)

target_link_libraries(
        ImGui
        PUBLIC
        android
        EGL
        GLESv3
        log
        DOBBY_LIB
)