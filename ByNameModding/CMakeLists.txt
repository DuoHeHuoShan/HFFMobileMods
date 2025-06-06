cmake_minimum_required(VERSION 3.8...3.26)


project(BNM)
# Dobby Hook
add_library(DOBBY_LIB STATIC IMPORTED)
set_target_properties(DOBBY_LIB PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Dobby/libs/${CMAKE_ANDROID_ARCH_ABI}/libdobby.a)

# Build BNM examples
set(BNM_BUILD_EXAMPLES OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PUBLIC_INCLUDE_DIRS
    include
    external/include
)

set(INCLUDE_DIRS
    include
    external/include
    external
    external/utf8
    src/private
    ../Dobby/include
)

add_library(
        BNM
        SHARED
        src/Class.cpp
        src/ClassesManagement.cpp
        src/Coroutine.cpp
        src/Defaults.cpp
        src/Delegates.cpp
        src/EventBase.cpp
        src/Exceptions.cpp
        src/FieldBase.cpp
        src/Hooks.cpp
        src/Image.cpp
        src/Internals.cpp
        src/Loading.cpp
        src/MethodBase.cpp
        src/MonoStructures.cpp
        src/PropertyBase.cpp
        src/UnityStructures.cpp
        src/Utils.cpp
)

target_include_directories(
        BNM
        PUBLIC
        ${INCLUDE_DIRS}
)


target_link_libraries(
        BNM
        PUBLIC
        DOBBY_LIB
        log
)


set_property(TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES ${PUBLIC_INCLUDE_DIRS})

if (${BNM_BUILD_EXAMPLES})
    add_library(BNM_Examples
            SHARED
            examples/Main.cpp
            examples/01_Basics.cpp
            examples/02_OtherStructures.cpp
            examples/03_Generic.cpp
            examples/04_CompileTimeClasses.cpp
            examples/05_ClassesManagement.cpp
            examples/06_Coroutine.cpp
            examples/07_CustomFinder.cpp
            examples/08_Exceptions.cpp
    )

    target_include_directories(
            BNM_Examples
            PUBLIC
            ${PUBLIC_INCLUDE_DIRS}
    )

    target_link_libraries(
            BNM_Examples
            PUBLIC
            BNM
            log
    )

endif ()

