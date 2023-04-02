call mkdir build
call cd build
cmake ../
cmake --build . -j 4 --config Release
call Release\VulkanFFT.exe
call cd ..
pause