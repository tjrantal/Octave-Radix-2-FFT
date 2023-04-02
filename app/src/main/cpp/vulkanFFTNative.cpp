// Copyright 2016 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Includes the Jni utilities for Android to be able to create the
// relevant bindings for java, including JNIEXPORT, JNICALL , and
// other "j-variables".
//MODIFIED BY Timo Rantalainen tjrantal at gmail dot com

//Vulkan imports, Vulkan has to be installed on the computer VK_SDK_PATH and/or VULKAN_SDK environment vars
#include <vulkan/vulkan_core.h>
#include <cmath> //M_PI, requires cmake definition add_compile_definitions(_USE_MATH_DEFINES)
#include <vector> //std::vector
#include <iostream>	//Reading the spv
#include <fstream>

//Debugging
#include <cassert> 
#include <chrono>	//Debugging, execution duration
#include <string>	//Memset, std::string

//Helper functions
#include "vulkanHelpers.h"
#if defined(WIN32)
	#include "pc_helpers.h"			//Enable using the code for windows pc
#elif ANDROID
	#include "android_helpers.h"	//Enable using the code for android
	#define LOG_TAG "timo.home.VULKANFFT_CPP"
#endif
#include "vulkanFFTNative.h"

//Constructor, get Vulkan instance here
VulkanFFT::VulkanFFT(){
	//INIT VULKAN
	VkResult res;
	LOGE("In VulkanFFT constructor\n");
	//Get Vulkan physical device
	VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO,NULL,"vulkanFFTNative",1,NULL,0,VK_API_VERSION_1_0};
	#if defined(WIN32)
		const std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };	//Include validation layer in PC
		VkInstanceCreateInfo instanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,NULL,0,&appInfo,(uint32_t) layers.size(),layers.data(),0,NULL};	
	#else	//My phone cannot do validation layers..
		VkInstanceCreateInfo instanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,NULL,0,&appInfo,0,NULL,0,NULL};	
	#endif
	LOGE("Call createInstance\n");
	res = vkCreateInstance(&instanceCreateInfo, NULL, instance);
	LOGE("vkCreateInstance res = %d\n",res);
	uint32_t gpu_count = 1;
    res = vkEnumeratePhysicalDevices(*instance, &gpu_count, NULL);	// get GPU count
	gpuDevices = new std::vector<VkPhysicalDevice>(gpu_count);
    res = vkEnumeratePhysicalDevices(*instance, &gpu_count, gpuDevices->data());
	LOGE("vkEnumeratePhysicalDevices res = %d\n",res);
	*physicalDevice = gpuDevices->front();
	
	VkPhysicalDeviceProperties deviceProps;
	vkGetPhysicalDeviceProperties(*physicalDevice, &deviceProps);
	LOGE("Device Name    : %s\n", deviceProps.deviceName );
	const uint32_t ApiVersion = deviceProps.apiVersion;
	LOGE("Vulkan Version: %d.%d.%d\n", VK_VERSION_MAJOR(ApiVersion),VK_VERSION_MINOR(ApiVersion),VK_VERSION_PATCH(ApiVersion));
	VkPhysicalDeviceLimits deviceLimits = deviceProps.limits;
	LOGE("Max Compute Shared Memory Size: %d KB\n", deviceLimits.maxComputeSharedMemorySize / 1024);
	
	//Start prepping Vulkan
	
	uint32_t queueFamily_count;
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamily_count,NULL);
	std::vector<VkQueueFamilyProperties> queueFamilyProps = std::vector<VkQueueFamilyProperties>(queueFamily_count);	//Get number of queue families
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamily_count,queueFamilyProps.data());
	auto propIt = std::find_if(queueFamilyProps.begin(), queueFamilyProps.end(), [](const VkQueueFamilyProperties& prop)
	{
		return prop.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT;
	});
	computeQueueFamilyIndex = std::distance(queueFamilyProps.begin(), propIt);
	LOGE("Compute Queue Family Index: %d\n",computeQueueFamilyIndex);
	
	// Just to avoid a warning from the Vulkan Validation Layer
	const float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo deviceQueueCreateInfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL,	VkDeviceQueueCreateFlags(),	computeQueueFamilyIndex, 1, &queuePriority};


	VkDeviceCreateInfo deviceCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, NULL, VkDeviceCreateFlags(), 1, &deviceQueueCreateInfo,0, NULL, 0, NULL, NULL};  // Device Queue Create Info struct
			  
	//VkDevice device;
	res = vkCreateDevice(*physicalDevice,&deviceCreateInfo,NULL,device);
	LOGE("vkCreateDevice res = %d\n",res);
}

void VulkanFFT::prepSignal(int sampleLength){
	
	
	//Create sample signal
	
	uint32_t dLength = sampleLength*2;
	uint32_t N = sampleLength;
	float sRate = 20;
	dataIn = std::vector<float>(dLength); //Create empty tensorInB, auto-initialised to 0
	indices = std::vector<int>(N);
	int sigLength = N - (N % (uint32_t) sRate);
	
	for (int i = 0;i<N;i++){
		if (i<(sigLength+1)){
			dataIn.at(2*i) = std::sin(2.*M_PI*2./sRate* (float) (i));
		}
		indices.at(i) = 2*i;
	}
	
	numElements= indices.size();
	
	//RESERVE GPU memory
	VkBufferCreateInfo inInfo = getBufferCreateInfo(computeQueueFamilyIndex, dLength, (float) 0);
	VkResult res = vkCreateBuffer(*device, &inInfo,NULL,inBuffer);
	LOGE("vkCreateBuffer res = %d\n",res);	
	*inBufferMemory = getDevMem(*physicalDevice,*device,*inBuffer);
	res = vkBindBufferMemory(*device,*inBuffer,*inBufferMemory,0);
	LOGE("vkBindBufferMemory float res = %d\n",res);	
	
	VkBufferCreateInfo indInfo = getBufferCreateInfo(computeQueueFamilyIndex, numElements, (uint32_t) 0);
	res = vkCreateBuffer(*device, &indInfo,NULL,indBuffer);
	*indBufferMemory = getDevMem(*physicalDevice,*device,*indBuffer);
	res = vkBindBufferMemory(*device,*indBuffer,*indBufferMemory,0);
	LOGE("vkBindBufferMemory uint res = %d\n",res);
	
	VkBufferCreateInfo outInfo = getBufferCreateInfo(computeQueueFamilyIndex, dLength, (float) 0);
	res = vkCreateBuffer(*device, &outInfo,NULL,outBuffer);
	*outBufferMemory = getDevMem(*physicalDevice,*device,*outBuffer);
	res = vkBindBufferMemory(*device,*outBuffer,*outBufferMemory,0);
	LOGE("vkBindBufferMemory float res = %d\n",res);	
	//Prep kernel descriptor info (maps onto the shader language). Will be used way further down in descriptor creation
	VkDescriptorBufferInfo inBufferInfo{*inBuffer, 0, dLength*sizeof(float)};
	VkDescriptorBufferInfo outBufferInfo{*outBuffer, 0, dLength*sizeof(float)};
	VkDescriptorBufferInfo indBufferInfo{*indBuffer, 0, numElements*sizeof(uint32_t)};

	//Copy arrays to GPU
	copyVectorToGPU(*device,*inBufferMemory,dataIn);
	copyVectorToGPU(*device,*indBufferMemory,indices);
	
	//Create the shader
	VkShaderModuleCreateInfo shaderModuleCreateInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, NULL, 0, (*shaderContents).size(), reinterpret_cast<const uint32_t*>(shaderContents->data())};

	res = vkCreateShaderModule(*device, &shaderModuleCreateInfo, NULL, shaderModule);
	LOGE("vkCreateShaderModule res = %d\n",res);	
	//Create descriptorset
	const std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBinding = {
		VkDescriptorSetLayoutBinding{0, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER , 1, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,NULL },
		VkDescriptorSetLayoutBinding{1, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER , 1, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,NULL },
		VkDescriptorSetLayoutBinding{2, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER , 1, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,NULL }
	};
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, NULL, 0, (uint32_t)descriptorSetLayoutBinding.size(), descriptorSetLayoutBinding.data()};
	
	res = vkCreateDescriptorSetLayout(*device, &descriptorSetLayoutCreateInfo, NULL, descriptorSetLayout);
	LOGE("vkCreateDescriptorSetLayout res = %d\n",res);	
	//Add push constants
	VkPushConstantRange pushConstantRange{VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, 0, 3*sizeof(uint32_t)};
	//Create pipeline
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,NULL,0, 1,descriptorSetLayout,1,&pushConstantRange};
	
	res = vkCreatePipelineLayout(*device,&pipelineLayoutCreateInfo,NULL,pipelineLayout);
	LOGE("vkCreatePipelineLayout res = %d\n",res);	
	
	VkPipelineCacheCreateInfo vkPipelineCacheCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,NULL,0,0,NULL};
	res = vkCreatePipelineCache(*device,&vkPipelineCacheCreateInfo,NULL,pipelineCache);
	LOGE("vkCreatePipelineCache res = %d\n",res);
	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,NULL,0, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,*shaderModule,"main",NULL};
	VkComputePipelineCreateInfo computePipelineCreateInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,NULL,0,	pipelineShaderStageCreateInfo, *pipelineLayout, VK_NULL_HANDLE, -1};			// Pipeline Layout
	res = vkCreateComputePipelines(*device,*pipelineCache,1, &computePipelineCreateInfo,NULL,computePipeline);
	LOGE("vkCreateComputePipelines res = %d\n",res);
	if(res != 0){
		return;
	}
	//Create descriptor pool
	VkDescriptorPoolSize descriptorPoolSize{VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (uint32_t) descriptorSetLayoutBinding.size()};
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,NULL,0, 1, 1, &descriptorPoolSize};
	
	res = vkCreateDescriptorPool(*device,&descriptorPoolCreateInfo,NULL,descriptorPool);
	LOGE("vkCreateDescriptorPool res = %d\n",res);

	VkDescriptorSetAllocateInfo descriptorSetAllocInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,NULL,*descriptorPool, 1, descriptorSetLayout};
	res = vkAllocateDescriptorSets(*device, &descriptorSetAllocInfo,descriptorSet);
	LOGE("vkAllocateDescriptorSets res = %d\n",res);

	std::vector<VkWriteDescriptorSet> writeDescriptorSets{
		VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,NULL,*descriptorSet, 0, 0, 1, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, NULL, &inBufferInfo, NULL},
		VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,NULL,*descriptorSet, 1, 0, 1, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, NULL, &outBufferInfo, NULL},
		VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,NULL,*descriptorSet, 2, 0, 1, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, NULL, &indBufferInfo, NULL}};
	
	vkUpdateDescriptorSets(*device,3,writeDescriptorSets.data(), 0, NULL); //Write descriptorset onto the GPU
	
	//Create command pool
	VkCommandPoolCreateInfo commandPoolCreateInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,NULL,VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,computeQueueFamilyIndex};
	res = vkCreateCommandPool(*device,&commandPoolCreateInfo,NULL,commandPool);
	LOGE("vkCreateCommandPool res = %d\n",res);
	VkCommandBufferAllocateInfo commandBufferAllocInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,NULL,*commandPool,VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY,1};
	res = vkAllocateCommandBuffers(*device, &commandBufferAllocInfo,cmdBuffer);
	LOGE("vkAllocateCommandBuffers res = %d\n",res);
	vkGetDeviceQueue(*device,computeQueueFamilyIndex, 0, queue);	//Get device queue

	

	
	
}

void VulkanFFT::compute(){
	//Prep the command buffer
	cmdBufferBeginInfo  =  new VkCommandBufferBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,NULL,VkCommandBufferUsageFlagBits:: VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,NULL};
	VkFenceCreateInfo fenceCreateInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,NULL,VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT};
	VkResult res = vkCreateFence(*device,&fenceCreateInfo,NULL,fence);
	LOGE("vkCreateFence res = %d\n",res);
	VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO,NULL,0,NULL,NULL,1,cmdBuffer,0,NULL};  // List of command buffers

	t = numElements / 2;
    aIndex = 1; // I/O initially placed in index 0 after mod operation
	p = 1;
	printBufferFloat("InBuffer",*device, *inBufferMemory, numElements);
	auto start = std::chrono::high_resolution_clock::now();
	values[2] = numElements/2;
	for(uint32_t p = 1; p < numElements; p*=2){
		aIndex = (aIndex + 1) % 2;	//Set aIndex for this iteration
		//Update push_constants for the iteration
		values[0] = p;	//Update p for this execution of the algorithm
		values[1] = aIndex;	//Update algorithm parameters
		vkResetFences(*device,1,fence);	//Reset the fence
		vkBeginCommandBuffer(*cmdBuffer, cmdBufferBeginInfo);
		vkCmdPushConstants(*cmdBuffer,*pipelineLayout,VK_SHADER_STAGE_COMPUTE_BIT,0,sizeof(values),&values);	//push_constants
		vkCmdBindPipeline(*cmdBuffer,VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE , *computePipeline);
		vkCmdBindDescriptorSets(*cmdBuffer,VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE ,*pipelineLayout,0,1,descriptorSet,0,NULL);
		vkCmdDispatch(*cmdBuffer,t, 1, 1);
		vkEndCommandBuffer(*cmdBuffer);
		vkQueueSubmit(*queue, 1, &submitInfo, *fence);
		vkWaitForFences(*device,1,fence,true, uint64_t(-1));
		//LOGE("p %02d\n",p);
		//printBufferFloat("IN: ", device, inBufferMemory, numElements);
		//printBufferFloat("OUT: ", device, outBufferMemory, numElements);
		
	}
	
	
	VkDeviceMemory bOut = aIndex == 1 ? *inBufferMemory : *outBufferMemory; //Note that aIndex was incremented after last execution!
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Vulkan uSec "<< duration.count() << std::endl;
	float* output = getArray(*device, bOut, numElements*2, (float) 0);
	//Get amplitudes
	std::string oPut =  "Output res: {";
	for (int i = 0; i<numElements/2+1; ++i){
		oPut.append(" "+std::to_string(round(sqrt(pow(output[2*i],2)+pow(output[2*i+1],2))/(double) (numElements/2)*10000.)/10000.));
	}
	std::cout << oPut << "}" << std::endl;

	delete output;	//Free the allocated output memory
}

VulkanFFT::~VulkanFFT(){
	LOGE("In Destructor\n");
	vkFreeMemory(*device,*inBufferMemory,NULL);
	vkDestroyBuffer(*device,*inBuffer,NULL);
	vkFreeMemory(*device,*indBufferMemory,NULL);
	vkDestroyBuffer(*device,*indBuffer,NULL);
	vkFreeMemory(*device,*outBufferMemory,NULL);
	vkDestroyBuffer(*device,*outBuffer,NULL);
	
	vkDestroyFence(*device,*fence,NULL);
	vkDestroyDescriptorSetLayout(*device,*descriptorSetLayout,NULL);
	vkDestroyPipelineLayout(*device,*pipelineLayout,NULL);
	vkDestroyPipelineCache(*device,*pipelineCache,NULL);
	vkDestroyShaderModule(*device,*shaderModule, NULL);
	vkDestroyPipeline(*device,*computePipeline, NULL);
	vkDestroyDescriptorPool(*device,*descriptorPool,NULL);
	vkDestroyCommandPool(*device,*commandPool,NULL);
	vkDestroyDevice(*device,NULL);
	vkDestroyInstance(*instance, NULL);
	shaderContents->clear();
	delete shaderContents;
	gpuDevices->clear();
	delete gpuDevices;

	//Delete objects pointed to
	delete instance; //VkInstance;
	delete device; //VkDevice;
	delete inBuffer; //VkBuffer;
	delete indBuffer; //VkBuffer;
	delete outBuffer; //VkBuffer;
	delete inBufferMemory; //VkDeviceMemory;
	delete indBufferMemory; //VkDeviceMemory;
	delete outBufferMemory; //VkDeviceMemory;
	delete descriptorSetLayout; //VkDescriptorSetLayout;
	delete shaderModule; //VkShaderModule;
	delete commandPool; //VkCommandPool;
	delete cmdBuffer; //VkCommandBuffer;
	delete fence; //VkFence;
	delete queue; //VkQueue;
	delete computePipeline; //VkPipeline;
	delete pipelineLayout; //VkPipelineLayout;
	delete pipelineCache; //VkPipelineCache;
	delete descriptorPool; //VkDescriptorPool;
	delete physicalDevice; //VkPhysicalDevice;
	delete cmdBufferBeginInfo; //VkCommandBufferBeginInfo;
	delete descriptorSet; //VkDe
	
}