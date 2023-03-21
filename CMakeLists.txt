#configure project and make it crossplatform (Windows, Linux, Web)
cmake_minimum_required(VERSION 3.21)

include(FetchContent)


FetchContent_Declare(
  glfw3
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8 # release-3.3.8
  OVERRIDE_FIND_PACKAGE
)



FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.2.5 # Release 5.2.5
  OVERRIDE_FIND_PACKAGE
)
set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(assimp)

project(glBenchy)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include") 

enable_language(C)
enable_language(CXX)

# CMake Settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SUPPRESS_REGENERATION true)
set(DCMAKE_GENERATOR_PLATFORM "x64")
set(CMAKE_CXX_STANDARD 20)
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

if(WIN32)
    set(CMAKE_CXX_FLAGS "/W3 /EHsc")
endif()

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
  #target_include_directories(glBenchy PUBLIC glfw)
endif()

if(UNIX)
  target_link_libraries(glBenchy uuid)
endif()

find_package(OpenGL REQUIRED)



#Common has to be added after executable statement - otherwise there's no target to attach sources to.

set_target_properties(glBenchy PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}")
set_target_properties(glBenchy PROPERTIES DEBUG_OUTPUT_NAME "${CMAKE_PROJECT_NAME}_d")

set_target_properties(glBenchy PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out/bin")

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(glBenchy glfw assimp ${OPENGL_LIBRARY})

#copy assets to bin out and copy built executable to bin.
