cmake_minimum_required(VERSION 3.20)
project(komputeFFT)

set(CMAKE_CXX_STANDARD 14)

# Set a default build type if none was specified
# Based on: https://github.com/openchemistry/tomviz/blob/master/cmake/BuildType.cmake
set(DEFAULT_BUILD_TYPE "Release")
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)

    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(FetchContent)
FetchContent_Declare(kompute GIT_REPOSITORY https://github.com/KomputeProject/kompute.git
    GIT_TAG 1344ece4ac278f9b3be3b4555ffaace7a032b91f) # v0.8.1 Replace with the latest from: https://github.com/KomputeProject/kompute/releases
FetchContent_MakeAvailable(kompute)
include_directories(${kompute_SOURCE_DIR}/src/include)

#Look for fftw3
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "C:/timo/programming/cpp/komputeFFT/findFFTW")
find_package(FFTW)
message(${FFTW_LIBRARIES})
message(${FFTW_INCLUDE_DIRS})

# Add to the list, so CMake can later find the code to compile shaders to header files
list(APPEND CMAKE_PREFIX_PATH "${kompute_SOURCE_DIR}/cmake")

add_subdirectory(shader)
add_subdirectory(src)
