#configure project and make it crossplatform (Windows, Linux, Web)
cmake_minimum_required(VERSION 3.21)
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

if(BENCHY_OS STREQUAL "AUTO")
    if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
        set(BENCHY_OS "WIN32" CACHE STRING "Select target OS API: WIN32, LINUX, WASM" FORCE)
    #elseif (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
    #set(BENCHY_OS "COCOA" CACHE STRING "A more specific platform selector to choose from, choose the exact OS protocol to use, can be WIN32, UWP, COCOA, UIKIT, XCB, XLIB, MIR, WAYLAND, ANDROID, WASM, NOOP." FORCE)
    elseif (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
        set(BENCHY_OS "LINUX" CACHE STRING "Select target OS API: WIN32, LINUX, WASM" FORCE)
    endif()
endif()

option(CONSOLE_ENABLED "Enable console" ON)

if(CONSOLE_ENABLED)
    add_definitions("-DCONSOLE")
endif()


include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include") 

add_subdirectory(src/Common)

#Target executable with proper flags
if(BENCHY_OS STREQUAL "WIN32")
    add_subdirectory(src/Win32)
elseif(BENCHY_OS STREQUAL "WASM")
    add_subdirectory(src/Wasm)
endif()



#Add sources directory
#add_subdirectory(src)

#add_executable(glBenchy glbenchy.cxx)