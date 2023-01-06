
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <chrono>

#include "fft.hpp"
#include <kompute/Kompute.hpp>
#include <fmt/core.h>	//Debugging with fmt. fmt is provided by Kompute

#include <fftw3.h>	//Compare against fftw3 for accuracy and speed

struct PushConstants{
	uint32_t p;
	uint32_t aIndex;
	uint32_t numElements;
	uint32_t t;
};

//Main
int main(){
	
	//Create test signal and pop that into tensorInA
	uint32_t dLength = 1<<6;
	uint32_t N = dLength/2;
	float sRate = 20;
	std::vector<float> dataIn(dLength); //Create empty tensorInB, auto-initialised to 0
	std::vector<float> empty(dLength); //Create empty tensorInB, auto-initialised to 0
	std::vector<int> indices(N);
	
	fftw_complex *in, *out;
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
		
	for (int i = 0;i<N;i++){
		if (i<(((int) sRate)+1)){
			dataIn.at(2*i) = std::sin(2.*M_PI*2./sRate* (float) (i));
			in[i][0] =  dataIn[2*i];
		}else{
			in[i][0] = 0;
		}
		indices.at(i) = 2*i;
		in[i][1] = 0;
		//Redundant, vectors ought to be initialised to all 0
		out[i][0] = 0;
		out[i][1] = 0;
	}
	std::cout << "data size: " << dLength << std::endl;;
	std::cout << "INPUT: {  ";
	for (const float& elem : dataIn) {
		std::cout << elem << "  ";
	}
	std::cout << "}" << std::endl;
	
	std::cout << "FFTW Input: {  ";
	for (int i = 0; i<N; ++i){
		std::cout << in[i][0] << "  ";
	}
	std::cout << "}" << std::endl;
	
	//FFT algorithm parameters
	uint32_t numElements= indices.size();
	uint32_t t = numElements / 2;
    uint32_t aIndex = 1; // I/O initially placed in index 0 after mod operation
	uint32_t p = 1;
	
	
	
	
    kp::Manager mgr;

    std::shared_ptr<kp::TensorT<float>> tensorInA = mgr.tensor(dataIn);	//Data into FFT
    std::shared_ptr<kp::TensorT<float>> tensorInB = mgr.tensor(std::vector<float>(dataIn.size()));	//Create an empty tensor for the FFT
	std::shared_ptr<kp::TensorT<int>> tensorI = mgr.tensorT<int>(indices);
    std::vector<float> pushConstants({ 0, 0, (float) t});	//FFT algorithm parameters

    const std::vector<std::shared_ptr<kp::Tensor>> params = { tensorInA,
                                                              tensorInB,
																tensorI
															};

    const std::vector<uint32_t> shader = std::vector<uint32_t>(
      shader::FFT_COMP_SPV.begin(), shader::FFT_COMP_SPV.end());
    std::shared_ptr<kp::Algorithm> algo = mgr.algorithm(params, shader,kp::Workgroup{t, 1, 1},{},pushConstants);
	//mgr.sequence()->record<kp::OpTensorSyncDevice>(params)->eval();	//Load data to device
	//mgr.sequence()->clear();	//Remove the sync call
	//Iterate through the FFT here
	auto start = std::chrono::high_resolution_clock::now();
	 std::cout << "INTO LOOP" << std::endl;
     for(p = 1; p < numElements; p*=2){
	 //for(p = 1; p < 1<<4; p*=2){
		  	aIndex = (aIndex + 1) % 2;
			pushConstants[0] = p;	//Update algorithm parameters
			pushConstants[1] = aIndex;	//Update algorithm parameters
			std::cout << "IN LOOP into eval" << p << " aIndex " << aIndex << std::endl;
			//mgr.sequence()->record<kp::OpAlgoDispatch>(algo, pushConstants)->eval();	//Update algorithm parameters and evaluate		
			//mgr.sequence()->clear();	//Remove the queue
			mgr.sequence()->record<kp::OpTensorSyncDevice>(params)->record<kp::OpAlgoDispatch>(algo, pushConstants)->record<kp::OpTensorSyncLocal>({tensorInA,tensorInB})->eval();	//Update algorithm 
			std::cout << "PAST eval " << p << std::endl;
     }
	 std::cout << "PAST loop aIndex " << aIndex << std::endl;
	 
	 //mgr.sequence()->clear();	//Clear the sequence
	 //mgr.sequence()->record<kp::OpTensorSyncLocal>({tensorInA,tensorInB})->eval(); 
	 //mgr.sequence()->clear();	//Clear the sequence
	 std::shared_ptr<kp::TensorT<float>> output = aIndex == 1 ? tensorInA : tensorInB;
	auto stop = std::chrono::high_resolution_clock::now();
	//if (aIndex == 0){
	//	output = tensorInA;
	//}else{
	//	output = tensorInB;
	//}
	
	//std::cout << "Output A: {  ";
	std::string oPut = "Output A: {  ";
	for (int i = 0; i<tensorInA->vector().size()/2; ++i){
		//std::cout << tensorInA->vector()[2*i] << "  "  << tensorInA->vector()[2*i+1] << "  ";
		oPut.append(fmt::format("{:.2f} {:.2f} ",tensorInA->vector()[2*i],tensorInA->vector()[2*i+1]));
		//std::cout << sqrt(pow(output->vector()[2*i],2)+pow(output->vector()[2*i+1],2))/(double) (indices.size()/2) << "  ";
	}
	std::cout << oPut << "}" << std::endl;
	
	oPut = "Output B: {  ";
	
	//std::cout << "Output B: {  ";
	for (int i = 0; i<tensorInB->vector().size()/2; ++i){
		//std::cout << tensorInB->vector()[2*i] << "  "  << tensorInB->vector()[2*i+1] << "  ";
		oPut.append(fmt::format("{:.2f} {:.2f} ",tensorInB->vector()[2*i],tensorInB->vector()[2*i+1]));
		//std::cout << sqrt(pow(output->vector()[2*i],2)+pow(output->vector()[2*i+1],2))/(double) (indices.size()/2) << "  ";
	}
	
	std::cout << oPut << "}" << std::endl;
	oPut =  "Output res: {  ";
	for (int i = 0; i<indices.size()/2+1; ++i){
		//std::cout << out[i][0] << "  "  << out[i][1] << "  ";
		oPut.append(fmt::format("{:.4f} ", sqrt(pow(output->vector()[2*i],2)+pow(output->vector()[2*i+1],2))/(double) (N/2)));
	}
	std::cout << oPut << "}" << std::endl;
	

	
	
	//Check result with FFTW
    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	auto fftwstart = std::chrono::high_resolution_clock::now();
    fftw_execute(plan);
	auto fftwstop = std::chrono::high_resolution_clock::now();
	//Print out the result'

	oPut =  "FFTW Output: {  ";
	for (int i = 0; i<N/2+1; ++i){
		//std::cout << out[i][0] << "  "  << out[i][1] << "  ";
		oPut.append(fmt::format("{:.4f} ", sqrt(pow(out[i][0],2)+pow(out[i][1],2))/(double) (N/2)));
	}
	std::cout << oPut << "}" << std::endl;

    fftw_destroy_plan(plan);
    fftw_free(in); fftw_free(out);
	
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Vulkan uSec "<< duration.count() << std::endl;
	auto fftwduration = std::chrono::duration_cast<std::chrono::microseconds>(fftwstop - fftwstart);
	std::cout << "FFTW3 uSec "<< fftwduration.count() << std::endl;
	
	
}
