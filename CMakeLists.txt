#configure project and make it crossplatform (Windows, Linux, Web)
cmake_minimum_required(VERSION 3.21)

include(FetchContent)


FetchContent_Declare(
  glfw3
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        7482de6071d21db77a7236155da44c172a7f6c9e # release-3.3.8
  OVERRIDE_FIND_PACKAGE
)

include_directories(include)

project(glBenchy)

enable_language(C)
enable_language(CXX)

# CMake Settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SUPPRESS_REGENERATION true)
set(DCMAKE_GENERATOR_PLATFORM "x64")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
set(BENCHY_OS AUTO CACHE STRING "Select target OS API: WIN32, LINUX, WASM")
set_property(
    CACHE
    BENCHY_OS PROPERTY
    STRINGS AUTO WIN32 LINUX WASM
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)


#Common has to be added after executable statement - otherwise there's no target to attach sources to.
add_subdirectory(src)

target_link_libraries(glBenchy glfw ${OPENGL_LIBRARY})
