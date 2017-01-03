cmake_minimum_required(VERSION 2.6)
project(Axal)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find: Qt5
find_package(Qt5Widgets)

# Find: OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

# Version: x.y
set(AXAL_VERSION_MAJOR 0)
set(AXAL_VERSION_MINOR 0)

# Configuration
configure_file(
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("${PROJECT_BINARY_DIR}")

# Mac OSX
set(MACOSX_BUNDLE_ICON_FILE launcher.icns)
set(AXAL_ICON ${CMAKE_CURRENT_SOURCE_DIR}/res/launcher.icns)
set_source_files_properties(${AXAL_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

# Executable
add_executable(Axal MACOSX_BUNDLE ${AXAL_ICON} src/main.cpp src/window.cpp src/viewport.cpp)

# Link: Qt 5
target_link_libraries(Axal Qt5::Widgets ${OPENGL_LIBRARIES})
