#include "vulkanFFTNative.cpp"

int main()
{
	//Call vulkanFFTNative stuff from here
	std::cout<<"Starting main"<<std::endl;
	VulkanFFT* vFFT = new VulkanFFT();
	std::cout<<"vFFT created"<<std::endl;
	//Read and set the shader contents. Read the shader .spv 
	if (std::ifstream shaderFile{ "fft.spv", std::ios::binary | std::ios::ate }){
		const size_t fileSize = shaderFile.tellg();
		shaderFile.seekg(0);
		vFFT->getShaderContentsVector()->resize(fileSize, '\0');
		shaderFile.read(vFFT->getShaderContentsVector()->data(), fileSize);
	}
	LOGE("vFFT->getShaderContentsVector()->size() %d\n",(int32_t) vFFT->getShaderContentsVector()->size());
	vFFT->prepSignal(1<<5);
	std::cout<<"signal prepped"<<std::endl;
	vFFT->compute();
	std::cout<<"compute called"<<std::endl;
	delete vFFT;
	std::cout<<"destructor called"<<std::endl;
	return 0;
}