#ifndef VULKAN_ANDROID_HELPERS_H
#define VULKAN_ANDROID_HELPERS_H

#include <vulkan/vulkan_core.h>
#include <jni.h>
#include <android/log.h>	//Android DEBUGGING

#include <iostream>
#include <cstdio>

// Macros for logging
#define LOG_TAG "timo.home.VULKANFFT_ANDROID_HELPERS"
#define LOG(severity, ...) ((void)__android_log_print(ANDROID_LOG_##severity, LOG_TAG, __VA_ARGS__))
#define LOGE(...) LOG(ERROR, __VA_ARGS__)


template<typename TYPE>
void printBuffer(char* log, VkDevice device, VkDeviceMemory bufferMemory,uint32_t numElements, TYPE type){
	void* inBufferPtr;
	vkMapMemory(device, bufferMemory, 0, numElements*sizeof(TYPE) ,0, &inBufferPtr);
	std::string oPut = log;
	oPut.append(":{");
	for (uint32_t i = 0; i < numElements; ++i){
		oPut.append(" "+std::to_string(((TYPE*) inBufferPtr)[i]));
	}
	oPut.append("}");
	vkUnmapMemory(device,bufferMemory);
}

void printBufferFloat(char* log, VkDevice device, VkDeviceMemory bufferMemory,uint32_t numElements){
	void* inBufferPtr;
	vkMapMemory(device, bufferMemory, 0, numElements*sizeof(float) ,0, &inBufferPtr);
	
	std::string oPut = std::string(log);
	oPut.append(":{");
	for (uint32_t i = 0; i < numElements; ++i){
		oPut.append(" "+std::to_string(round(((float*) inBufferPtr)[i]*100.)/100.));
	}
	oPut.append("}");
	LOGE("%s",oPut.c_str());
	vkUnmapMemory(device,bufferMemory);
}

#endif  // VULKAN_ANDROID_HELPERS_H