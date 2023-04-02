#ifndef VULKAN_PC_HELPERS_H
#define VULKAN_PC_HELPERS_H

#include <vulkan/vulkan_core.h>
#include <iostream>
#include <cstdio>

#define LOGE(...) ((void) std::printf(__VA_ARGS__) )

template<typename TYPE>
void printBuffer(char* log, VkDevice device, VkDeviceMemory bufferMemory,uint32_t numElements, TYPE type){
	void* inBufferPtr;
	vkMapMemory(device, bufferMemory, 0, numElements*sizeof(TYPE) ,0, &inBufferPtr);
	std::cout << log << ": ";
	for (uint32_t i = 0; i < numElements; ++i)
	{
		std::cout << ((TYPE*) inBufferPtr)[i] << " ";
	}
	std::cout << std::endl;
	vkUnmapMemory(device,bufferMemory);
}

void printBufferFloat(char* log, VkDevice device, VkDeviceMemory bufferMemory,uint32_t numElements){
	void* inBufferPtr;
	vkMapMemory(device, bufferMemory, 0, numElements*sizeof(float) ,0, &inBufferPtr);
	std::cout << log << ": ";
	for (uint32_t i = 0; i < numElements; ++i)
	{
		std::cout << round(((float*) inBufferPtr)[i]*100.)/100. << " ";
	}
	std::cout << std::endl;
	vkUnmapMemory(device,bufferMemory);
	
}

#endif  // VULKAN_PC_HELPERS_H