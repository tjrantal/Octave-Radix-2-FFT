# Octave-Radix-2-FFT
Octave port of [renderscipt FFT implementation](https://github.com/nesl/renderScriptFFT).
The above renderscript chokes on Android SDKs 18 to 23 (because the foreach call does not allow more than 3 parameters).
I'm trying to figure out how to implement it with a different foreach call -> confirming the maths ought to work


UPDATE 2020/01/05
Added a [Kompute](https://github.com/KomputeProject/kompute)-enabled [Vulkan](https://www.vulkan.org/) C++ Radix-2 FFT implementation copied and ported from [here](https://github.com/miracle2121/hpc12-fp-rl1609). The implementation is much slower than FFTW3 so might not be a good way to go. I will use it in my Android apps to replace Renderscript to avoid writing new code.

- Octave (Matlab) script testFFT.m demonstrates that octave fft and the radix-2 produce the same results.
- c++ implementation - use [cmake](https://cmake.org/) to compile
	- mkdir build
	- cd build
	- cmake ..
	- cmake --build . -j 4 --config Release
	- Release\komputeFFT.exe
	- Depends on Vulkan SDK being installed and added to environment variables as VK_SDK_PATH=C:\VulkanSDK\1.2.198.1 or VULKAN_SDK=C:\VulkanSDK\1.2.198.1. Cannot remember which one worked with cmake
	- Depends on [fftw3](https://www.fftw.org/) being installed. I downloaded the pre-compiled version. Added FFTWDIR=C:\timo\software\fftw3 and then FFTWDIR to PATH, and ran
		- "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.30.30705\bin\Hostx64\x64\lib.exe" /machine:x64 /def:libfftw3-3.def
		- "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.30.30705\bin\Hostx64\x64\lib.exe" /machine:x64 /def:libfftw3f-3.def
		- "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.30.30705\bin\Hostx64\x64\lib.exe" /machine:x64 /def:libfftw3l-3.def
	-FindFFTW.cmake copied from [here](https://github.com/egpbos/findFFTW)
	- FFTW and the Vulkan Radix-2 ought produce the same results in the minimal test case on my computer. There may be other dependencies I cannot remember - have a look at the complaints you get from cmake...

UPDATE 2023/04/05
Added a pure [Vulkan](https://www.vulkan.org/) implementation of the above Kompute example in the app directory. The steps to execute the PC version are the same as described above just in the app directory (see compileTest.bat for details). The same c++ code works on Android as well, execute gradle installDebug from this folder (i.e., the folder with this text file) for testing. Keep an eye on logcat for the output once you hit the compute button of the app. The Android app worked on my Galaxy S21 with Android 13 on it. Note that I have modified the Kompute kernel shader for this Vulkan implementation as I was not constricted to float push constants with Vulkan. The implementation is a smidge faster than with Kompute, which is also what I was hoping for.

[GPL version 3](https://www.gnu.org/licenses/gpl-3.0.en.html) or the licensing indicated in the code I have included from others applies. Written by Timo Rantalainen tjrantal at gmail dot com
	
	