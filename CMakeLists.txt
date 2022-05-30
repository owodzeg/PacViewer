cmake_minimum_required(VERSION 3.19)
project(PacViewer VERSION 3.0.11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
add_definitions(-DPACVIEWER_VERSION="${CMAKE_PROJECT_VERSION}")

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)

set(ENGINE_FILES
        Instruction.cpp
		Menu.cpp
		PacMessageBox.cpp
		PacView.cpp
		tinyfiledialogs.c)

add_executable(PacViewer ${ENGINE_FILES} main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Enabling Unity build for Release build")
    set_target_properties(PacViewer PROPERTIES
            UNITY_BUILD ON
            UNITY_BUILD_BATCH_SIZE 5)
endif()

if(MSVC)
    target_compile_options(PacViewer PRIVATE /W4)
    target_compile_definitions(PacViewer PRIVATE _USE_MATH_DEFINES=1)
else()
    target_compile_options(PacViewer PRIVATE -Wall -Wextra)
endif()

target_include_directories(PacViewer PRIVATE Engine/)

target_link_libraries(PacViewer PRIVATE
        sfml-graphics
        sfml-audio)

if (OS STREQUAL "Linux")
    find_package(X11 REQUIRED)
    target_link_libraries(PacViewer PRIVATE X11::X11)
endif (OS STREQUAL "Linux")
