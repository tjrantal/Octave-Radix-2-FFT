cmake_minimum_required(VERSION 3.20)
add_compile_definitions(_USE_MATH_DEFINES)
message("IN SRC")
message(${FFTW_INCLUDE_DIRS})
add_executable(komputeFFT main.cpp)
target_include_directories(komputeFFT PRIVATE ${FFTW_INCLUDE_DIRS})

target_link_libraries(komputeFFT PRIVATE shader kompute::kompute ${FFTW_LIBRARIES})
