%RADIX-2 FFT implementation ported to Octave from
%https://github.com/nesl/renderScriptFFT
%The above renderscript chokes on apis 18 to 23. I'm trying to figure
%out how to implement it with a different foreach call...
close all;
fclose all;
clear all;
clc;

addpath('functions');
sFreq = 20; %1000
N = 2^5;    %2^12
t = ([1:N]-1)./sFreq;
signal = sin(2*pi*2*t); %+0.5*sin(2*pi*200*t)+1.5*sin(2*pi*312.4*t);
signal(22:end) = 0; %Match C++ code
tic
fftSignal = abs(fft(signal));
toc;
amp = fftSignal/(length(fftSignal)/2);
amp = amp(1:((length(amp)/2)+1));
freq = linspace(0,sFreq/2,length(amp));
fh = figure;
plot(freq,amp,'k-','linewidth',3);

hold on;

tic
r2FFT = radix2fft(signal);
toc
%keyboard;
r2FFT = abs(r2FFT);
ramp = r2FFT/(length(r2FFT)/2);
ramp = ramp(1:((length(ramp)/2)+1));

%keyboard;
plot(freq,ramp,'b-.','linewidth',3);

if 1
	tic
	naiveFFT = naive_fft(signal);
	toc
	naiveFFT = abs(naiveFFT);
	namp = naiveFFT/(length(naiveFFT)/2);
	namp = namp(1:((length(namp)/2)+1));

	plot(freq,amp,'r--','linewidth',3);
end

waitfor(fh);
