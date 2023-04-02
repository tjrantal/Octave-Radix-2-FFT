#include <jni.h>	//Enable calling the code from java
#include <android/asset_manager_jni.h>	//Loading the shader code
#include "vulkanFFTNative.h"

VulkanFFT* vFFT;	//Global var vulkanFFT

static std::vector<float> jfloatArrayToVector(JNIEnv* env, const jfloatArray& fromArray){
    float* inCArray = env->GetFloatArrayElements(fromArray, NULL);
    if (NULL == inCArray)
        return std::vector<float>();
    int32_t length = env->GetArrayLength(fromArray);

    std::vector<float> outVector(inCArray, inCArray + length);
    return outVector;
}

static jfloatArray vectorToJFloatArray(JNIEnv* env, const std::vector<float>& fromVector){
    jfloatArray ret = env->NewFloatArray(fromVector.size());
    if (NULL == ret)
        return NULL;
    env->SetFloatArrayRegion(ret, 0, fromVector.size(), fromVector.data());
    return ret;
}

void readAndSetSPV(AAssetManager* assetManager,char* shaderPath){
	AAsset* file = AAssetManager_open(assetManager,shaderPath, AASSET_MODE_BUFFER);
	size_t fileLength = AAsset_getLength(file);
	vFFT->getShaderContentsVector()->resize(fileLength, '\0');
	AAsset_read(file, vFFT->getShaderContentsVector()->data(), fileLength);	//Read the .spv into vFFT memory		
}


extern "C"{
	JNIEXPORT jboolean JNICALL
    Java_timo_home_vulkanFFT_VulkanFFT_prepVulkan(JNIEnv* env, jobject thiz, jobject assetManager, jint sampleLength){
		vFFT = new VulkanFFT();	//Initialise Vulkan
		char* cString = "shaders/fft.comp.spv";
		readAndSetSPV(AAssetManager_fromJava(env, assetManager),cString); //Read the shader .spv with AAsset
		vFFT->prepSignal((int32_t) sampleLength);	//Prep signals
		return JNI_TRUE;
    }
	
    JNIEXPORT jfloatArray JNICALL
    Java_timo_home_vulkanFFT_VulkanFFT_compute(JNIEnv* env, jobject thiz, jfloatArray signalIn){
        std::vector<float> signalVector = jfloatArrayToVector(env, signalIn);
		vFFT->compute();
		return vectorToJFloatArray(env, signalVector);	//Return the input vector
    }
	
	JNIEXPORT void JNICALL Java_timo_home_vulkanFFT_VulkanFFT_delete(JNIEnv* env, jobject thiz){
		delete vFFT;	//Clear vulkan here
    }

}

