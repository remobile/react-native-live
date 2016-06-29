LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_FILENAME := libLiveVideoCoreSDK

LOCAL_SRC_FILES := \
	LiveVideoCoreSDK.cpp

LOCAL_C_INCLUDES := \
	 $(LOCAL_PATH)/RtmpLivePushSDK \
	 $(LOCAL_PATH)/RtmpLivePushSDK/boost \
	 $(LOCAL_PATH)/RtmpLivePushSDK/glm \


include $(BUILD_SHARED_LIBRARY)

