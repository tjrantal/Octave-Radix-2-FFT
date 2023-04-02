cmake_minimum_required(VERSION 3.16)
add_compile_definitions(_USE_MATH_DEFINES)
project(VulkanFFT)

find_package(Vulkan REQUIRED)

#Compile GLSL to SPIR-V with CMake
add_custom_command(
	OUTPUT "${CMAKE_BINARY_DIR}/fft.spv"
	COMMAND $ENV{VK_SDK_PATH}/Bin/glslc src/main/cpp/shaders/fft.comp -o "${CMAKE_BINARY_DIR}/fft.spv"
	DEPENDS "src/main/cpp/shaders/fft.comp"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Buiding Shaders"
)
add_custom_target(ComputeShaderFFT DEPENDS "${CMAKE_BINARY_DIR}/fft.spv")


add_executable(VulkanFFT "src/main/cpp/main.cpp")
target_link_libraries(VulkanFFT PRIVATE Vulkan::Vulkan)
add_dependencies(VulkanFFT ComputeShaderFFT)

