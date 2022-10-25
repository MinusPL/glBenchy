#configure project and make it crossplatform (Windows, Linux, Web)
cmake_minimum_required(VERSION 3.21)

include(FetchContent)


FetchContent_Declare(
  glfw3
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8 # release-3.3.8
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
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

# Options
set(BENCHY_OS AUTO CACHE STRING "Select target OS API: WIN32, LINUX, WASM")
set_property(
    CACHE
    BENCHY_OS PROPERTY
    STRINGS AUTO WIN32 LINUX WASM
)

add_subdirectory(src)

if (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DEMSCRIPTEN=1)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s USE_GLFW=3 -s USE_WEBGL2=1 -s FULL_ES3=1")
    add_definitions(-DWASM_BUILD)
endif ()

if(NOT EMSCRIPTEN)
  find_package(glfw3 REQUIRED)
  target_link_libraries(glBenchy glfw)
endif()

find_package(OpenGL REQUIRED)



#Common has to be added after executable statement - otherwise there's no target to attach sources to.


target_link_libraries(glBenchy glfw ${OPENGL_LIBRARY})
