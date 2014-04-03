LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := main

LOCAL_SRC_FILES := main.cpp \
				   shader.cpp \
				   model.cpp \
				   reader.cpp

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
	LOCAL_CFLAGS := -DHAVE_NEON=1
	LOCAL_ARM_NEON := true
endif

LOCAL_STATIC_LIBRARIES := cpufeatures
LOCAL_LDLIBS := -llog -landroid -lGLESv3
LOCAL_CFLAGS := -mfloat-abi=softfp -mfpu=neon -march=armv7 -O3
include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)
