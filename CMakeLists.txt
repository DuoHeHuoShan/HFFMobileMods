cmake_minimum_required(VERSION 3.8...3.26)

project(NativeModLoader)

add_subdirectory(ByNameModding EXCLUDE_FROM_ALL)
add_subdirectory(ImGui EXCLUDE_FROM_ALL)
add_subdirectory(Mods/HFFSkipSplashScreen)
add_subdirectory(Mods/HFFLevelSelect)
add_subdirectory(Mods/HFFSettings)
add_subdirectory(Mods/HFFTimer)