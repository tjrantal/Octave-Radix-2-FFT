cmake_minimum_required(VERSION 3.20)
project(vulkanFFT)
set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(_USE_MATH_DEFINES) #M_PI

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#find_package(Vulkan REQUIRED)
find_library(libvulkan vulkan)
find_library(libandroid android)
find_library(log-lib log)

#COMPILE VULKAN SHADER MANUALLY #Compile GLSL to SPIR-V with CMake
#MESSAGE(STATUS "TIMO CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")
#MESSAGE(STATUS "TIMO CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")
#MESSAGE(STATUS "TIMO ENV{VK_SDK_PATH} $ENV{VK_SDK_PATH}")
add_custom_command(
	OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/../assets/shaders/fft.comp.spv"
	COMMAND $ENV{VK_SDK_PATH}/Bin/glslc shaders/fft.comp -o "${CMAKE_CURRENT_SOURCE_DIR}/../assets/shaders/fft.comp.spv"
	DEPENDS "shaders/fft.comp"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Buiding Shaders"
)
add_custom_target(compileShaderFFT DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../assets/shaders/fft.comp.spv")


#add_subdirectory(shader) #Shaders automatically compiled from main/shaders into app/src/main/shaders/

add_library(vulkanFFT SHARED android_helper.cpp 
							 vulkanFFTNative.cpp
                               vulkanHelpers.h
							   vulkanFFTNative.h
							   android_helpers.h)

target_link_libraries(vulkanFFT PRIVATE vulkan log android)
add_dependencies(vulkanFFT compileShaderFFT)

