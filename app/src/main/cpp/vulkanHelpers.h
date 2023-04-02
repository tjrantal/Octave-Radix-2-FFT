#ifndef VULKAN_HELPERS_H
#define VULKAN_HELPERS_H

#include <vulkan/vulkan_core.h>

template<typename TYPE>
VkBufferCreateInfo getBufferCreateInfo(uint32_t queueFamilyIndex, uint32_t dLength, TYPE type){ 
	return VkBufferCreateInfo{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		NULL,
		VkBufferCreateFlags(),					// Flags
		dLength*sizeof(TYPE),						// Size
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,	// Usage
		VkSharingMode::VK_SHARING_MODE_EXCLUSIVE,				// Sharing mode
		1,											// Number of queue family indices
		&queueFamilyIndex							// List of queue family indices
	};
}

VkDeviceMemory getDevMem(VkPhysicalDevice physicalDevice, VkDevice device,VkBuffer buffer){
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device,buffer,&memoryRequirements);
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice,&memoryProperties);
	uint32_t memoryTypeIndex = uint32_t(~0);
	VkDeviceSize memoryHeapSize = uint64_t(~0);
	for (uint32_t currentMemoryTypeIndex = 0; currentMemoryTypeIndex < memoryProperties.memoryTypeCount; ++currentMemoryTypeIndex)
	{
		VkMemoryType memoryType = memoryProperties.memoryTypes[currentMemoryTypeIndex];
		if ((VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & memoryType.propertyFlags) &&
			(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & memoryType.propertyFlags))
		{
			memoryHeapSize = memoryProperties.memoryHeaps[memoryType.heapIndex].size;
			memoryTypeIndex = currentMemoryTypeIndex;
			break;
		}
	}
	VkMemoryAllocateInfo memAlloInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,NULL,memoryRequirements.size,memoryTypeIndex};
	VkDeviceMemory deviceMem;
	VkResult res = vkAllocateMemory(device, &memAlloInfo, NULL, &deviceMem);
	return deviceMem;
}

template<typename TYPE>
void copyVectorToGPU(VkDevice device,VkDeviceMemory bufferMemory,std::vector<TYPE> dataIn){
	void* bufferPtr;
	vkMapMemory(device, bufferMemory, 0, dataIn.size()*sizeof(TYPE) ,0, &bufferPtr);
	memcpy (bufferPtr, dataIn.data(), sizeof(TYPE)*dataIn.size());
	vkUnmapMemory(device,bufferMemory);
}

template<typename TYPE>
TYPE* getArray(VkDevice device, VkDeviceMemory bufferMemory,uint32_t numElements, TYPE type){
	void* bufferPtr;
	vkMapMemory(device, bufferMemory, 0, numElements*sizeof(TYPE) ,0, &bufferPtr);
	TYPE* ret = new TYPE[numElements];
	memcpy (ret, bufferPtr, sizeof(TYPE)*numElements);
	vkUnmapMemory(device,bufferMemory);
	return ret;
}

#endif  // VULKAN_HELPERS_H