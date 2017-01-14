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

# Find: Additional Frameworks (macOS)
if(APPLE)
  find_library(COCOA Cocoa)
  find_library(IOKIT IOKit)
  find_library(COREVIDEO CoreVideo)
endif()

# Version: x.y
set(AXAL_VERSION_MAJOR 0)
set(AXAL_VERSION_MINOR 0)

# Configuration
configure_file(
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include")

# Source files
FILE(GLOB AXAL_SOURCES src/*.cpp)

# Build
if(APPLE)
  set(MACOSX_BUNDLE_ICON_FILE launcher.icns)
  set(AXAL_ICON ${CMAKE_CURRENT_SOURCE_DIR}/res/launcher.icns)
  set_source_files_properties(${AXAL_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

  add_executable(Axal MACOSX_BUNDLE ${AXAL_ICON} ${AXAL_SOURCES})

  add_library(libaxal STATIC ${AXAL_SOURCES})
  set_property(TARGET libaxal PROPERTY OUTPUT_NAME axal)

  set_property(TARGET Axal libaxal PROPERTY CXX_STANDARD 14)

  target_compile_definitions(Axal PRIVATE AXAL_MAIN)

  target_link_libraries(Axal Qt5::Widgets ${OPENGL_LIBRARIES} ${COCOA} ${IOKIT} ${COREVIDEO})
  target_link_libraries(libaxal Qt5::Widgets ${OPENGL_LIBRARIES} ${COCOA} ${IOKIT} ${COREVIDEO})

  install(TARGETS libaxal DESTINATION ARCHIVE DESTINATION .)
else()
  find_package(Threads REQUIRED)

  add_executable(axal ${AXAL_ICON} ${AXAL_SOURCES})

  add_library(libaxal STATIC ${AXAL_SOURCES})
  set_property(TARGET libaxal PROPERTY OUTPUT_NAME axal)

  set_property(TARGET axal libaxal PROPERTY CXX_STANDARD 14)

  if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(PUBLIC axal "-pthread")
    target_compile_options(PUBLIC libaxal "-pthread")
  endif()

  if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(axal "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(libaxal "${CMAKE_THREAD_LIBS_INIT}")
  endif()

  target_compile_definitions(axal PRIVATE AXAL_MAIN)

  target_link_libraries(axal Qt5::Widgets ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})
  target_link_libraries(libaxal Qt5::Widgets ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})

  install(TARGETS libaxal DESTINATION ARCHIVE DESTINATION .)
endif()
