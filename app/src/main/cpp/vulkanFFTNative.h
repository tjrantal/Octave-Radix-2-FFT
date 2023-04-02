#ifndef VULKAN_NATIVE_H
#define VULKAN_NATIVE_H

#include <vulkan/vulkan_core.h>
#include <vector> //std::vector

class VulkanFFT{
	private:
	
		std::vector<float> dataIn; //Check if this needs to be a pointer
		std::vector<int> indices;	//check if this needs to be a pointer
		uint32_t values[3];
		uint32_t numElements;
		uint32_t t;
		uint32_t aIndex; // I/O initially placed in index 0 after mod operation
		uint32_t p;
		uint32_t computeQueueFamilyIndex;

		std::vector<char> *shaderContents = new std::vector<char>();

		//Vulkan
		VkInstance* instance = new VkInstance;
		VkDevice* device = new VkDevice;
		VkBuffer* inBuffer = new VkBuffer;
		VkBuffer* indBuffer = new VkBuffer;
		VkBuffer* outBuffer = new VkBuffer;
		VkDeviceMemory* inBufferMemory = new VkDeviceMemory;
		VkDeviceMemory* indBufferMemory = new VkDeviceMemory;
		VkDeviceMemory* outBufferMemory = new VkDeviceMemory;
		VkDescriptorSetLayout* descriptorSetLayout = new VkDescriptorSetLayout;
		VkShaderModule* shaderModule = new VkShaderModule;
		VkCommandPool* commandPool = new VkCommandPool;
		VkCommandBuffer* cmdBuffer = new VkCommandBuffer;
		VkFence* fence = new VkFence;
		VkQueue* queue = new VkQueue;
		VkPipeline* computePipeline = new VkPipeline;
		VkPipelineLayout* pipelineLayout = new VkPipelineLayout;
		VkPipelineCache* pipelineCache = new VkPipelineCache;
		VkDescriptorPool* descriptorPool = new VkDescriptorPool;
		std::vector<VkPhysicalDevice>* gpuDevices;
		VkPhysicalDevice* physicalDevice = new VkPhysicalDevice;
		VkCommandBufferBeginInfo* cmdBufferBeginInfo = new VkCommandBufferBeginInfo;
		VkDescriptorSet* descriptorSet = new VkDescriptorSet;
		
	public:
		//Function declarations
		std::vector<char>* getShaderContentsVector(){return shaderContents;};
		void prepSignal(int sampleLength); //Function to prepare the computation
		void compute();
		VulkanFFT();	//Constructor
		~VulkanFFT();
		
	
};

#endif  // VULKAN_NATIVE_H