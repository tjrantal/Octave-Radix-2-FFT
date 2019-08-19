# Octave-Radix-2-FFT
Octave port of [renderscipt FFT implementation](https://github.com/nesl/renderScriptFFT).
The above renderscript chokes on Android SDKs 18 to 23 (because the foreach call does not allow more than 3 parameters).
I'm trying to figure out how to implement it with a different foreach call -> confirming the maths ought to work
